#ifndef STRINGLL_H
#define STRINGLL_H
struct stringllnode {
	char *data;
	struct stringllnode *next_node;
};
#endif

void append(struct stringllnode **, char *);
void append_all(struct stringllnode **, char **, int);
void print_list(struct stringllnode *);
