#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "stringlib.h"
#include "stringll.h"

int lib_str_find(char * str, char x) {
	int i = 0;
	while (*str != '\0') {
		if (*str ==  x) {
			return i;
		}
		i++;
		str++;
	}
	return -1;
}

int lib_str_remove_extra_spaces(char *a) {
	int len = strlen(a)+1;
	char *arr = (char *) malloc(sizeof(char)*len);
	strcpy(arr,a);
	int i=0;
	int j=0;

	while(j<len && arr[j]==' '){
		j++;
	}
	while(j<len){

		if(arr[j]==' '){

			arr[i]=arr[j];
			i++;j++;

			while(j<len && arr[j]==' '){
					j++;
			}

		}

		else if((arr[j]=='.' || arr[j]==',' || arr[j]=='?') && arr[i-1]==' '){
			if(arr[i-1]==' ')
					arr[i-1]=arr[j];
			j++;			
		}
		else{
			arr[i]=arr[j];
			i++;j++;
		}	

	}	
	if(i<j){

		i--;
		if(arr[i]==' '){
			arr[i]='\0';
			i++;
		}
		while(i<len){
			arr[i]='\0';
			i++;
			}

	}
	strcpy(a,arr);
	return 0;
}

int lib_str_split(const char *string_, char *delimiter,struct stringllnode **start ) {
	int len = strlen(string_);
	char *st = (char *) malloc(sizeof(char)*len);
	strcpy(st,string_);
	char* token;
	
	for (token = strtok(st, delimiter); token; token = strtok(NULL, delimiter))
	{
			lib_str_remove_extra_spaces(token);
			append(start,token);
	}
	return 0;
}

