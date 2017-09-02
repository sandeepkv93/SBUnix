#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "sbush.h"

/* sbush shell */

int is_bg_process(char * line) {
	int len = strlen(line);
	return (line[len-1] == '&');
}

enum cmd_type get_command_type(char* input_line) {
	if (is_bg_process(input_line)) {
		return cmd_bg;
	}
	return cmd_bin;
}

int main(int argc, char *argv[], char *envp[]) {
	char input_line[1000];
	char new_input_line[1000];
	char *ps1 = "\033[93msbush>\033[0m";
	char *arglist[2];
	pid_t pid;
	enum cmd_type command_type;
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

		if (command_type == cmd_bg) {
			debug_print("Is BG\n");
			strncpy(new_input_line, input_line, strlen(input_line)-2);
			memset(input_line,0,strlen(input_line));
			strncpy(input_line, new_input_line, strlen(new_input_line));
			debug_print("Filtered to %s \n", input_line);
		}
		pid = fork();
		if (pid) {
			//Parent
			if (command_type != cmd_bg) {
				wait(NULL);
			}
			
		} else {
			//child
			arglist[1] = NULL;
			arglist[0] = input_line;
			debug_print("Execing: %s\n", input_line);
			execvp(input_line, arglist);
		}
		// Print PS1.
		puts(ps1);
	}
	return 0;
}
