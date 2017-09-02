#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "stringlib.h"
#include "stringll.h"

char * remove_extra_spaces(char *a){
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
	return arr;
}

struct stringllnode* lib_str_split(const char *string_, char *delimiter) {
	int len = strlen(string_);
	char *st = (char *) malloc(sizeof(char)*len);
	strcpy(st,string_);
	char* token;
	struct stringllnode *start = NULL;
	for (token = strtok(st, delimiter); token; token = strtok(NULL, delimiter))
	{
		append(&start,remove_extra_spaces(token));
	}
	return start;
}

