#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "sbush.h"
#include "stringll.h"
#include "stringlib.h"


char ** environ;
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

	lib_str_split(line,CHAR_SPACE, &cmd_list);
	if (!cmd_list) {
			return builtin_none;
	}
	if(cmd_list->data[0]=='#'){
		return builtin_comment;
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
			if(lib_str_split_get_member(environ[i], CHAR_EQUALS, 1, value)) {
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

int is_dir(char * filename){
	int fd = open(filename, O_DIRECTORY,0);
	if(fd > 0)
	{
		close(fd);
		return 1;
	}
	return 0;
}

int is_file_exists(char * file){
	char path[500];
	struct stringllnode * path_list = NULL;
	get_env("PATH",path);
	lib_str_split(path, CHAR_COLON, &path_list);
	do{
		strcat(path_list->data,"/");
		strcat(path_list->data,file);
 		if( access(path_list->data, F_OK ) < 0 ){
			path_list = path_list->next_node;
			continue;
		}
		if( is_dir(path_list->data)){
                        	path_list = path_list->next_node;
                        	continue;
		}

		puts("not a directory");
		strcpy(file,path_list->data);
		return 1; 	
	}while(path_list != NULL);
	return 0;
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

	if (lib_str_split_get_member(line, CHAR_SPACE, 1, export_statement) ||
		lib_str_split_get_member(export_statement, CHAR_EQUALS, 0, env_key) ||
		lib_str_split_get_member(export_statement, CHAR_EQUALS, 1, env_value)) {
		error_print("Usage export <key>=<value>\n");
	}

	debug_print("Setting %s : %s\n", env_key, env_value);
	set_env(env_key, env_value);
	return 0;
}

int handle_cd(char * line) 
{
	char buff[100];
	if(lib_str_split_get_member(line, CHAR_SPACE, 1, buff)) {
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
	lib_str_split(input_line,CHAR_BG, &cmd_curr);
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
	
	lib_str_split(input_line,CHAR_SPACE, &cmd_curr);
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
	execvpe(script_args[0], script_args, environ);
	error_print("Failed to run script! Please check the script\n");
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
	lib_str_split(input_line,CHAR_PIPE, &cmd_curr);
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
			if(!is_file_exists(arglist[0])){
				error_print(arglist[0]);
				error_print("Executable not found\n");
				exit(1);
			}
			debug_print("Execing: %s\n", cmd_curr->data);
			execvpe(arglist[0], arglist, environ);
			error_print("Unknown command : ");
			error_print(arglist[0]);
			error_print("\n");
			exit(1);
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

void print_ps1() {
	char ps1[100];
	get_env("PS1",ps1);
	puts(ps1);
}

int is_whitespace(char a){
	if(a==' ' || a=='\t' || a=='\n' || a=='\0'){
		return 1;
	}
	return 0;
}

int is_empty_str(char *s){
	int i;		
	for(i=0;i<strlen(s);++i) {
		if(!is_whitespace(s[i])) {
			return 1;
		}
	}
	return 0;
}

int main(int argc, char *argv[], char *envp[])
{
	int mode = MODE_INTERACTIVE;
	char input_line[1000];
	enum cmd_t command_type;
	environ = envp;
	int input_fd = 0;
	debug_print("Argc = %d\n",argc);

	if (argc > 1) {
		mode = MODE_SCRIPT;
		if((input_fd = open(argv[1],O_RDONLY,0)) == -1) {
			error_print("Unable to read the script! : ");
			error_print(argv[0]);
		}
	} else {
		print_ps1();
	}

	while (fgets(input_fd,input_line)) {
		if(is_empty_str(input_line)) {
			//print PS1
			continue;
		}
		/*
		 * Maintain PATH
		 * Read the args and run script, else:
		 * 1. Check for built in commands
		 * 2. Check for |, split into commands
		 * 3. Check for &, set BG
		 * 4. Fork and exec binary. check BG
		 */
		command_type = get_command_type(input_line);
		if (mode == MODE_INTERACTIVE) {
			print_ps1();
		}
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
					case builtin_comment:
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
		if (mode==MODE_INTERACTIVE) {
			print_ps1();
		}

	}
	return 0;
}
