#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "stringll.h"
#include "stringlib.h"

void append(struct stringllnode** head_reference, char *new_data) {
	struct stringllnode* new_node = (struct stringllnode*) malloc(sizeof(struct stringllnode));
	
	new_node->data = malloc(sizeof(char)*(strlen(new_data)+1));
	strncpy(new_node->data,new_data,(strlen(new_data)+1));
	new_node->next_node = NULL;
	
	struct stringllnode *last = *head_reference;
	if (*head_reference == NULL) {
		*head_reference = new_node;
		return;
	}
	while (last->next_node != NULL) {
		last = last->next_node;
	}
	last->next_node = new_node;
	return;
}

void append_all(struct stringllnode** head_reference, char** new_data_array, int array_size) {
	int index;
	for(index = 0;index < array_size; ++index) {
		append(head_reference,*(new_data_array+index));
	} 
}

void print_list(struct stringllnode *node) {
	while (node != NULL) {
		puts(node->data);
		node = node->next_node;
	}
}

void free_list(struct stringllnode *node) {
	struct stringllnode * prev = node;
	while(node != NULL) {
		prev = node;
		node = node->next_node;
		free(prev);
	}
	free(node);
}

int get_node(struct stringllnode *node, int position, char* string) {
	while(node) {
		if (!position) {
			strcpy(string,node->data);
			return 0;
		}
		position--;
		node = node->next_node;
	}
	return -1;
}
