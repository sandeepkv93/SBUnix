#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "sbush.h"
#include "stringll.h"
#include "stringlib.h"

extern char ** environ;
/* sbush shell */

#if 0
int pipe_(int fds[]) 
{
	return syscall(SYS_pipe,fds);
}

int dup_(int fd)
{
	return syscall(SYS_dup,fd);
}

int close_(int fd)
{
	return syscall(SYS_close,fd);
}

int wait_(int pid)
{
	return  syscall(SYS_wait4,-1, NULL, 0, NULL);
}
#endif

int is_bg_process(char * line) 
{
	return (lib_str_find(line,STR_BG)>0);
}

int is_piped(char * line) 
{
	return (lib_str_find(line,STR_PIPE)>0);
}

enum builtin_t get_shell_builtin(char * line) 
{
	int i;
	struct stringllnode * cmd_list = NULL;
	struct s_builtins builtins_db[] = {
		{builtin_cd, "cd"},
		{builtin_exit, "exit"},
		{builtin_export, "export"}
	};

	lib_str_split(line,STR_SPACE, &cmd_list);
	if (!cmd_list) {
			return builtin_none;
	}
	for (i=0; i < LENARR(builtins_db); i++) {
		if (!strcmp(cmd_list->data,builtins_db[i].command)) {
				free_list(cmd_list);
				return builtins_db[i].type;
		}
	}
	free_list(cmd_list);
	return builtin_none;
}

void get_env(char* key, char *value)
{
	/* 
	 * Searches for key in environ variable
	 * and sets key=value aptly
	 */
	int i = 0;
	char buff[100];

	strcpy(buff, key);
	strcat(buff,STR_EQUALS);

	while(environ[i] != NULL) {
		if(!lib_str_find(environ[i], buff)) {
			if(lib_str_split_get_member(environ[i], STR_EQUALS, 1, value)) {
				strcpy(value, STR_DEFAULT_PS1);
			} 
			return;
		}
		i++;
	}
	strcpy(value, STR_DEFAULT_PS1);
}

void set_env(char* key, char * value)
{
	/* 
	 * Searches for key in environ variable
	 * and sets key=value aptly
	 */
	int i = 0;
	char buff[100];

	strcpy(buff, key);
	strcat(buff,STR_EQUALS);

	while(environ[i] != NULL) {
		if(!lib_str_find(environ[i], buff)) {
			strcpy(environ[i], buff);
			strcat(environ[i], value);
		}
		i++;
	}

	// Hack
	strcpy(environ[i-1], buff);
	strcat(environ[i-1], value);
	environ[i] = NULL;
}

int handle_export(char * line) 
{
	/* 
	 * Handles statements of the form:
	 * 		export key=value
	 */
	char export_statement[100];
	char env_key[100];
	char env_value[100];

	if (lib_str_split_get_member(line, STR_SPACE, 1, export_statement) ||
		lib_str_split_get_member(export_statement, STR_EQUALS, 0, env_key) ||
		lib_str_split_get_member(export_statement, STR_EQUALS, 1, env_value)) {
		error_print("Usage export <key>=<value>\n");
	}

	debug_print("Setting %s : %s\n", env_key, env_value);
	set_env(env_key, env_value);
	return 0;
}

int handle_cd(char * line) 
{
	char buff[100];
	if(lib_str_split_get_member(line, STR_SPACE, 1, buff)) {
		// set_pwd($HOME);
		error_print("Usage cd <Path>\n");
	} else {
		// Path is given
		if(chdir(buff)) {
			debug_print("Failed to chdir()");
		}
	}
	return 0;
}
	
enum cmd_t get_command_type(char* input_line) 
{
	
	if (get_shell_builtin(input_line) != builtin_none) {
		return cmd_builtin;
	} else if(is_bg_process(input_line)) {
		return cmd_bg;
	} else if(is_piped(input_line)) {
		return cmd_pipe;
	}
	return cmd_bin;
}

int get_bg_command(char * input_line) 
{
	struct stringllnode * cmd_curr = NULL;
	lib_str_split(input_line,STR_BG, &cmd_curr);
	strcpy(input_line, cmd_curr->data);
	free_list(cmd_curr);
	return 0;
}

int get_arglist(char * input_line, char ** arg_list) 
{
	/*
	 * Given an input such as:
	 * echo hello world
	 *
	 * returns ["echo","hello","world"]
	 */
	struct stringllnode * cmd_head = NULL;
	struct stringllnode * cmd_curr = NULL;
	int i=0;
	
	lib_str_split(input_line,STR_SPACE, &cmd_curr);
	cmd_head = cmd_curr;
	while (cmd_curr) {
		arg_list[i] = cmd_curr->data;
		cmd_curr = cmd_curr->next_node;
		i++;
	}
	free_list(cmd_head);
	arg_list[i] = NULL;
	return 0;
}

int run_script(char** argv, int argc) 
{
	char * script_args[100];
	int i;
	for (i=1; i < argc; i++) {
		script_args[i-1] = argv[i];
	}
	script_args[i] = NULL;
	debug_print("Going to exec");
	execvp(script_args[0], script_args);
	error_print("Failed to run script %s", script_args[0]);
	exit(1);
	return 0;

}

int run_cmd(char * input_line, enum cmd_t command_type)
{
	/* Given a link list head with commands as the data,
	 * runs each command and pipes them onto the next command.
	 * If no pipe is found executes command.
	 */

	struct stringllnode * cmd_curr = NULL, * cmd_head;
	char *arglist[100];
	int fds[2] ;
	int read_end =-1;
	int write_end = -1;
	int waste;
	pid_t pid;
	// Split on | and read the commands into a link list
	lib_str_split(input_line,STR_PIPE, &cmd_curr);
	cmd_head = cmd_curr;

	while (cmd_curr) {
		if(pipe(fds)==-1) {
			debug_print("Failed to create pipe.");
		}
		write_end = fds[1];
		if(!(pid=fork())) {
			if(cmd_head!=cmd_curr) {
				// Not first command, read from pipe
				close(STDIN_FD);
				waste = dup(read_end);
			}
			if(cmd_curr->next_node) {
				// Not last command, write to pipe
				close(STDOUT_FD);
				waste = dup(write_end);
			}
			get_arglist(cmd_curr->data, arglist); 
			debug_print("Execing: %s\n", cmd_curr->data);
			execvp(arglist[0], arglist);
		} else {
			if (command_type != cmd_bg) {
				debug_print("waiting");
				waitpid(pid,NULL,0);
			}
			close(write_end);
			if (read_end != -1) {
				close(read_end);
			}
			read_end = fds[0];
		}
		cmd_curr = cmd_curr->next_node;
		waste++;
	}
	free_list(cmd_curr);
	return 0;
}

int main(int argc, char *argv[], char *envp[]) 
{
	int i =0;
	char input_line[1000];
	char ps1[100];
	enum cmd_t command_type;
	debug_print("Env variables:\n");
	while(environ[i] != NULL) {
		debug_print("%s\n",environ[i]);
		i++;
	}
	debug_print("Argc = %d\n",argc);
	if (argc > 1) {
		run_script(argv,argc);
	}

	get_env("PS1",ps1);
	puts(ps1);
	while (gets(input_line)) {
		/*
		 * Maintain PATH
		 * Read the args and run script, else:
		 * 1. Check for built in commands
		 * 2. Check for |, split into commands
		 * 3. Check for &, set BG
		 * 4. Fork and exec binary. check BG
		 */
		command_type = get_command_type(input_line);
		switch(command_type) {
			case cmd_builtin:
				switch (get_shell_builtin(input_line)) {
					case builtin_exit:
						exit(0);
					case builtin_cd:
						debug_print("Trying to cd..\n");
						handle_cd(input_line);
						break;
					case builtin_export:
						handle_export(input_line);
						break;
					case builtin_none:
						break;
				}
				
				break;
			case cmd_bg:
				get_bg_command(input_line);
				debug_print("BG process. ");
				debug_print("Filtered to %s \n", input_line);
			case cmd_pipe:
			case cmd_script:
			case cmd_bin:
				run_cmd(input_line,command_type);
		}

		// Print PS1.
		get_env("PS1",ps1);
		puts(ps1);
	}
	return 0;
}
