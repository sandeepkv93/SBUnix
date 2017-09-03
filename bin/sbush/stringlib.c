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

void lib_str_remove_extra_spaces(char *input_string) {
        int initial_string_length = strlen(input_string);
        int dst = 0;
        int src = 0;
        while (src < initial_string_length && input_string[src] == ' ') {
            ++src;
        }
        while (src < initial_string_length) {

            if (input_string[src] == ' ') {

                input_string[dst] = input_string[src];
                ++dst;
                ++src;

                while (src < initial_string_length && input_string[src] == ' ') {
                    ++src;
                }

            } else if (input_string[src] == '!' || input_string[src] == ',' || input_string[src] == '.' ||  input_string[src] == '?') {
                if (input_string[dst - 1] == ' ') {
                    input_string[dst - 1] = input_string[src];
                    src++;
                } else {
                    input_string[dst++] = input_string[src++];
                }

            } else {
                input_string[dst++] = input_string[src++];
            }
        }
        if (dst < src) {

            --dst;
            if (input_string[dst] == ' ') {
                input_string[dst++] = '\0';
            }
            while (dst < initial_string_length) {
                input_string[dst++] = '\0';
            }

        }
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

