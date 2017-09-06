#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv, char** envp) {
	int i;
	//	char buff[100];
	//	gets(buff);
	//	puts(buff);

	for(i=0; i<argc; i++) {
		puts(argv[i]);
	}

	for(i=0; envp[i] != NULL; i++) {
		puts(envp[i]);
	}
	return 0;
}
