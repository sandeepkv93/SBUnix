#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "sbush.h"
#include "stringll.h"
#include "stringlib.h"

/* sbush shell */

int is_bg_process(char * line) {
	return (lib_str_find(line,'&')>0);
}

int is_piped(char * line) {
	return (lib_str_find(line,'|')>0);
}

enum builtin_t get_shell_builtin(char * line) {
	int i;
	struct stringllnode * cmd_list = NULL;
	struct s_builtins builtins_db[] = {
		{builtin_cd, "cd"},
		{builtin_exit, "exit"}
	};

	lib_str_split(line," ", &cmd_list);
	if (!cmd_list) {
			return builtin_none;
	}
	for (i=0; i < LENARR(builtins_db); i++) {
		if (!strcmp(cmd_list->data,builtins_db[i].command)) {
				return builtins_db[i].type;
		}
	}
	free(cmd_list);
	return builtin_none;
}
	
enum cmd_t get_command_type(char* input_line) {
	
	if (get_shell_builtin(input_line) != builtin_none) {
		return cmd_builtin;
	} else if(is_bg_process(input_line)) {
		return cmd_bg;
	} else if(is_piped(input_line)) {
		return cmd_pipe;
	}
	return cmd_bin;
}

int get_bg_command(char * input_line) {
	struct stringllnode * cmd_curr = NULL;
	lib_str_split(input_line, "&", &cmd_curr);
	strcpy(input_line, cmd_curr->data);
	free_list(cmd_curr);
	return 0;
}

int get_arglist(char * input_line, char ** arg_list) {
	struct stringllnode * cmd_head = NULL;
	struct stringllnode * cmd_curr = NULL;
	int i=0;
	lib_str_split(input_line," ", &cmd_curr);
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

int main(int argc, char *argv[], char *envp[]) {
	int i =0;
	char input_line[1000];
	char *ps1 = "\033[93msbush>\033[0m";
	char *arglist[100];
	pid_t pid;
	enum cmd_t command_type;
	puts(ps1);
	debug_print("Env variables:\n");
	while(envp[i] != NULL) {
		debug_print("%s\n",envp[i]);
		i++;
	}
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
				break;
			case cmd_bg:
				get_bg_command(input_line);
				debug_print("Is BG\n");
				debug_print("Filtered to %s \n", input_line);
			case cmd_pipe:
			case cmd_script:
			case cmd_bin:
				pid = fork();
				if (pid) {
					//Parent
					if (command_type != cmd_bg) {
						wait(NULL);
					}
				} else {
					//child
					get_arglist(input_line, arglist);
					debug_print("Execing: %s\n", input_line);
					execvp(arglist[0], arglist);
				}
		}

		// Print PS1.
		puts(ps1);
	}
	return 0;
}
