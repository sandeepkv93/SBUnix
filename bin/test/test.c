#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv, char** envp) {
	char buff[100];
	gets(buff);
	puts(buff);
	return 0;
}
