#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int strlen_new(const char * s1);
int strcmp_new(const char * s1,const char *s2);

int strcmp_new(const char * s1,const char * s2)
{
        while(*s1 == *s2 && *s1 != '\0')
        {
                s1++;
                s2++;
        }
        return *s1-*s2;
}
int strlen_new(const char * s1)
{
        int len=0;
        while(*s1++!='\0')
                len++;
        return len;
}
char * strcpy_new(char * dest,const char * src)
{
        char * temp;
        temp = dest;
        while((*temp++ = *src++)!='\0')
		;
        return dest;
}

char * strcat_new(char * dest,const char * src)
{
	char * temp = dest;
	while(*temp != '\0')
		temp++;
	while((*temp++ = *src++)!='\0')
		;
	*temp='\0';
	return dest;
}

int main(int argc, char ** argv)
{
char dest[20];
//printf("%d %d\n",strlen_new(argv[1]),strlen_new(argv[2]));
//printf("\n%d\n",strcmp_new(argv[1],argv[2]));
printf("%s\n",strcpy_new(dest,argv[1]));
printf("%s",strcat_new(dest,argv[1]));
}
