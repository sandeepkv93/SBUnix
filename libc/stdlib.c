#include <sys/syscall.h>
#include <unistd.h>

union header {
	struct
	{
		union header  *next;
		unsigned int size;
	} meta;
	long double x; //for alignment to support the most restrictive type
};

typedef union header Header;

Header start;
Header *freep = NULL;

/*
void * brk_new(void * addr)
{
	return (void *)syscall(SYS_brk, addr); //returns current boundary when invalid address is passed. Sets the 
}*/

void * pls_giv_mem(int num_bytes)
{
	void * cur_brk = brk(NULL); // provides the current boundary -> start of the memory block that gets allocated in this call.
	brk(cur_brk + num_bytes); // increment data segment boundary
	return cur_brk;
}

void free(void * new) // user will send different pointers. different types can be typecasted to void
{
	Header *temp = freep->meta.next;
	Header * new_block;
	if (new == NULL) {
		return;
	}
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
	//printf("free list of pointers:\n");
	while(temp != freep)
	{
		//printf("addr:%ld size:%d\n",(long)(temp + 1),temp->meta.size);
		temp = temp->meta.next;
	}
}

void print_malloc_list()
{
        Header *temp = start.meta.next;
        //printf("malloc list of pointers:\n");
        while(temp != &start)
        {
                //printf("addr:%ld size:%d\n",(long)(temp + 1),temp->meta.size);
                temp = temp->meta.next;
        }
}
Header * get_mem(unsigned num_units)
{
	Header * new;
	//handle out of memory/mem limit exceed
	new = (Header *)pls_giv_mem(num_units*sizeof(Header));
	new->meta.size = num_units;
	free((void *)(new+1));
	return freep; 
}


void* malloc(size_t num_bytes)
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
		//printf("checking %ld\n",(long)(cur+1));
		if(cur->meta.size>=num_units)
		{
			//printf("found %ld\n",(long)(cur+1));
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


#ifdef __TEST__
int main(int argc, char ** argv)
{

printf("START %ld\n",(long)brk_(NULL));
struct stringllnode *Head=NULL;
append_all(&Head,argv,argc);
print_list(Head);
print_malloc_list();
printf("first alloc %ld Head = %ld\n",(long)brk_(NULL),(long)Head);
free_list(Head);
printf("after free %ld Head = %ld\n",(long)brk_(NULL),(long)Head);

Head = NULL;

print_free_list();
print_malloc_list();
append_all(&Head,argv,argc);
print_list(Head);
print_malloc_list();
printf("first alloc %ld Head = %ld\n",(long)brk_(NULL),(long)Head);
free_list(Head);
printf("after free %ld Head = %ld\n",(long)brk_(NULL),(long)Head);

Head = NULL;

print_free_list();

}
#endif


