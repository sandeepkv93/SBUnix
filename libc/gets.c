#include <unistd.h>
char *gets(char *s) {
	int i=0;
	char a='\0';
	while(1) {
		if (read(0,&a,1) < 0){ 
			return NULL; 
		}

		if (a=='\n') {
			break;
		}

		s[i] = a;
		i++;
	} 
	s[i] = '\0';
	return s;
}
