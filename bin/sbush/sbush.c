#include <stdio.h>

/* sbush shell */

int main(int argc, char *argv[], char *envp[]) {
	while (getchar()) {
		// Print PS1.
		puts("sbush> ");

		/*
		 * Maintain PATH
		 * Read the args and run script, else:
		 * 1. Check for built in commands
		 * 2. Check for |, split into commands
		 * 3. Check for &, set BG
		 * 4. Fork and exec binary. check BG
		 */
	}
	return 0;
}
