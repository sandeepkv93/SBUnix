#include<stdio.h>
#include<unistd.h>
//#include<stdlib.h>
#include<sys/syscall.h>
#include<string.h>
#include "stringll.h"
#include "stringlib.h"
union header
{
	struct
	{
		union header  *next;
		unsigned int size;
	}meta;
	long double x; //for alignment to support the most restrictive type
};

typedef union header Header;

Header start;
Header *freep = NULL;

void * brk_new(void * addr)
{
	return (void *)syscall(SYS_brk, addr); //returns current boundary when invalid address is passed. Sets the 
}

void * pls_giv_mem(int num_bytes)
{
	void * cur_brk = brk_new(NULL); // provides the current boundary -> start of the memory block that gets allocated in this call.
	brk_new(cur_brk + num_bytes); // increment data segment boundary
	return cur_brk;
}

void free_(void * new) // user will send different pointers. different types can be typecasted to void
{
	Header *temp = freep->meta.next;
	Header * new_block;
	new_block = (Header*)new - 1;
	
//	while(!(new_block>temp && new_block<temp->meta.next))
//	{
//		printf("in free_mem");
//		fflush(stdout);
//		printf("\n%ld %ld",new_block,temp->meta.next);
//		temp = temp->meta.next;
//		exit(0);
//	}
	while(temp->meta.next!=freep)
		temp=temp->meta.next;
	temp->meta.next = new_block;
	new_block->meta.next = freep;
//	new_block->meta.next = freep->meta.next;
//	freep->meta.next = new_block;
}
void print_free_list()
{
	Header * temp = freep->meta.next;
	printf("free list of pointers:\n");
	while(temp != freep)
	{
		printf("addr:%ld size:%d\n",(long)(temp + 1),temp->meta.size);
		temp = temp->meta.next;
	}
}

void print_malloc_list()
{
        Header *temp = start.meta.next;
        printf("malloc list of pointers:\n");
        while(temp != &start)
        {
                printf("addr:%ld size:%d\n",(long)(temp + 1),temp->meta.size);
                temp = temp->meta.next;
        }
}
Header * get_mem(unsigned num_units)
{
	Header * new;
	//handle out of memory/mem limit exceed
	new = (Header *)pls_giv_mem(num_units*sizeof(Header));
	new->meta.size = num_units;
	free_((void *)(new+1));
	return freep; 
}


void* malloc_(unsigned num_bytes)
{
        unsigned num_units;
        Header *cur, *prev;
        
        //initialisation. list is empty.
        if(freep == NULL)
        {
                start.meta.next = freep = prev = &start;
                start.meta.size = 0;
        }

	
        num_units = ((num_bytes + sizeof(Header)-1)/sizeof(Header)) + 1; // one extra block for the header. 
	
	prev = freep;
	cur = prev->meta.next; // start with the ds	

        while(1)
        {
		printf("checking %ld\n",(long)(cur+1));
		if(cur->meta.size>=num_units)
		{
			printf("found %ld\n",(long)(cur+1));
			prev->meta.next = cur->meta.next;
			return (void*)(cur+1); // address of the free space has to be sent and not the address of the header.
		}
		if(cur == freep) // free block not available
		{
			cur = get_mem(num_units);
			//error handling
		}
		prev = cur;
		cur = cur->meta.next;
        }
}

//////////////////////////////////////////////////////////////////
//*******************TESTING FUNCTIONS****************************
//////////////////////////////////////////////////////////////////


void append(struct stringllnode** head_reference, char *new_data) {
        struct stringllnode* new_node = (struct stringllnode*) malloc_(sizeof(struct stringllnode));

        new_node->data = malloc_(sizeof(char)*(strlen(new_data)+1));
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
	printf("Pointers being freed\n");
        while(node->next_node != NULL) {
                prev = node;
                node = node->next_node;
		printf("%ld\n",(long)prev);
		printf("%ld\n",(long)prev->data);
		free_(prev->data);
                free_(prev);
        }
	printf("%ld\n",(long)node);
	printf("%ld\n",(long)node->data);
	free_(node->data);
        free_(node);
}



int main(int argc, char ** argv)
{

printf("START %ld\n",(long)brk_new(NULL));
struct stringllnode *Head=NULL;
append_all(&Head,argv,argc);
print_list(Head);
print_malloc_list();
printf("first alloc %ld Head = %ld\n",(long)brk_new(NULL),(long)Head);
free_list(Head);
printf("after free %ld Head = %ld\n",(long)brk_new(NULL),(long)Head);

Head = NULL;

print_free_list();
print_malloc_list();
append_all(&Head,argv,argc);
print_list(Head);
print_malloc_list();
printf("first alloc %ld Head = %ld\n",(long)brk_new(NULL),(long)Head);
free_list(Head);
printf("after free %ld Head = %ld\n",(long)brk_new(NULL),(long)Head);

Head = NULL;

print_free_list();

/*


append_all(&Head, argv,argc);
print_list(Head);
printf("second alloc  %ld Head = %ld\n",(long)brk_new(NULL),(long)Head);
free_list(Head);
printf("after free %ld Head = %ld\n",(long)brk_new(NULL),(long)Head);
Head = NULL;
append_all(&Head, argv,argc);
print_list(Head);
printf("second alloc  %ld Head = %ld\n",(long)brk_new(NULL),(long)Head);
free_list(Head);
printf("after free %ld Head = %ld\n",(long)brk_new(NULL),(long)Head);
Head = NULL;
append_all(&Head, argv,argc);
print_list(Head);
printf("second alloc  %ld Head = %ld\n",(long)brk_new(NULL),(long)Head);
free_list(Head);
printf("after free %ld Head = %ld\n",(long)brk_new(NULL),(long)Head);
Head = NULL;
append_all(&Head, argv,argc);
print_list(Head);
printf("second alloc  %ld Head = %ld\n",(long)brk_new(NULL),(long)Head);
printf("\n\n");
print_free_list();
*/
/*
	printf("%ld",(long)brk_new(NULL));

	char * p;
	p  = (char *)pls_giv_mem(sizeof(char));
	*p = 'c';
	printf("%c\n",*p);

        printf("%ld\n",(long)brk_new(NULL));
	p = (char *)pls_giv_mem(sizeof(char));
        *p = 'd';
        printf("%c\n",*p);
	printf("%ld",(long)brk_new(NULL));
	printf(" START %ld\n",(long)brk_new(NULL));

	char * p;
	p =(char *)malloc_(sizeof(char));
	*p = 'a';
	printf("%c\n",*p);
	printf("first alloc %ld p=%ld\n",(long)brk_new(NULL),(long)p);
	print_free_list();
	print_malloc_list();

	int *q;
	q = (int *)malloc_(sizeof(int));
	*q = 2;
	printf("%d\n",*q);
	printf("second alloc %ld q=%ld\n",(long)brk_new(NULL),(long)q);
	print_free_list();
	print_malloc_list();
	free_(p);
	p = NULL;
	printf("after free %ld\n ",(long)brk_new(NULL));
	print_free_list();
	print_malloc_list();
	free_(q);
        print_free_list();
        print_malloc_list();
        p =(char *)malloc
	_(sizeof(char));
        *p = 'x';
        printf("%c\n",*p);
	printf("another alloc but should use freed mem%ld p=%ld\n",(long)brk_new(NULL),(long)p);
	print_free_list();
	print_malloc_list();

	printf("\n****derived types****\n");

	char * str = (char *)memalloc(sizeof(char)*10);
	strcpy(str,"Hello\0");
	printf("\narray of chars %s\n brk = %ld str = %ld",str,(long)brk_new(NULL),(long)str);
	free_mem(str);
	str = NULL;
	str = (char *)memalloc(sizeof(char)*10);
	strcpy(str,"Hello\0");
        printf("\narray of chars %s\n brk =%ld str = %ld",str,(long)brk_new(NULL),(long)str);

*/

}
