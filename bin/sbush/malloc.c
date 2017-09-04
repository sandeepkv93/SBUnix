#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/syscall.h>
#include<string.h>


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

void free_mem(void * new) // user will send different pointers. different types can be typecasted to void
{
	Header * temp = freep;
	Header * new_block;
	new_block = (Header*)new - 1;
/*	
	while(!(new_block>temp && new_block<temp->meta.next))
	{
		printf("in free_mem");
		fflush(stdout);
		printf("\n%ld %ld",new_block,temp->meta.next);
		temp = temp->meta.next;
		exit(0);
	}
*/	
	new_block->meta.next = freep->meta.next;
	freep->meta.next = new_block;
}

Header * get_mem(unsigned num_units)
{
	Header * new;
	//handle out of memory/mem limit exceed
	new = (Header *)pls_giv_mem(num_units*sizeof(Header));
	new->meta.size = num_units;
	free_mem((void *)(new+1));
	return freep; 
}


void* memalloc(unsigned num_bytes)
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
		if(cur->meta.size>=num_units)
		{
			prev->meta.next = cur->meta.next;
			freep = prev;
			return (void*)(cur+1); // address of the free space has to be spent and not the address of the header.
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


int main(int argc, char ** argv)
{
/*	printf("%ld",(long)brk_new(NULL));

	char * p;
	p  = (char *)pls_giv_mem(sizeof(char));
	*p = 'c';
	printf("%c\n",*p);

        printf("%ld\n",(long)brk_new(NULL));
	p = (char *)pls_giv_mem(sizeof(char));
        *p = 'd';
        printf("%c\n",*p);
	printf("%ld",(long)brk_new(NULL));
*/
	printf(" START %ld\n",(long)brk_new(NULL));
	char * p;
	p =(char *)memalloc(sizeof(char));
	*p = 'a';
	printf("%c\n",*p);
	int * q;
	printf("first alloc %ld p=%ld\n",(long)brk_new(NULL),(long)p);
	q = (int *)memalloc(sizeof(int));
	*q = 2;
	printf("%d\n",*q);
	printf("second alloc %ld q=%ld\n",(long)brk_new(NULL),(long)q);
	free_mem(p);
	p = NULL;
	printf("after free %ld\n ",(long)brk_new(NULL));
        p =(char *)memalloc(sizeof(char));
        *p = 'x';
        printf("%c\n",*p);
	printf("another alloc but should use freed mem%ld p=%ld\n",(long)brk_new(NULL),(long)p);

	printf("\n****derived types****\n");

	char * str = (char *)memalloc(sizeof(char)*10);
	strcpy(str,"Hello\0");
	printf("\narray of chars %s\n brk = %ld str = %ld",str,(long)brk_new(NULL),(long)str);
	free_mem(str);
	str = NULL;
	str = (char *)memalloc(sizeof(char)*10);
	strcpy(str,"Hello\0");
        printf("\narray of chars %s\n brk =%ld str = %ld",str,(long)brk_new(NULL),(long)str);
}
