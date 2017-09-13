#include <sys/syscall.h>
#include <unistd.h>

union header
{
    struct
    {
        union header* next;
        unsigned int size;
    } meta;
    long double x;
};

typedef union header Header;

Header start;
Header* freep = NULL;

void*
pls_giv_mem(int num_bytes)
{
    void* cur_brk = brk(NULL);
    brk(cur_brk + num_bytes);
    return cur_brk;
}

void
free(void* new)
{
    Header* temp = freep->meta.next;
    Header* new_block;
    if (new == NULL) {
        return;
    }
    new_block = (Header*)new - 1;

    while (temp->meta.next != freep)
        temp = temp->meta.next;
    temp->meta.next = new_block;
    new_block->meta.next = freep;
}

void
print_free_list()
{
    Header* temp = freep->meta.next;
    while (temp != freep) {
        temp = temp->meta.next;
    }
}

void
print_malloc_list()
{
    Header* temp = start.meta.next;
    while (temp != &start) {
        temp = temp->meta.next;
    }
}
Header*
get_mem(unsigned num_units)
{
    Header* new;
    new = (Header*)pls_giv_mem(num_units * sizeof(Header));
    new->meta.size = num_units;
    free((void*)(new + 1));
    return freep;
}

void*
malloc(size_t num_bytes)
{
    unsigned num_units;
    Header *cur, *prev;
    if (freep == NULL) {
        start.meta.next = freep = prev = &start;
        start.meta.size = 0;
    }
    num_units = ((num_bytes + sizeof(Header) - 1) / sizeof(Header)) + 1;
    prev = freep;
    cur = prev->meta.next;
    while (1) {
        if (cur->meta.size >= num_units) {
            prev->meta.next = cur->meta.next;
            return (void*)(cur + 1);
        }
        if (cur == freep) {
            cur = get_mem(num_units);
        }
        prev = cur;
        cur = cur->meta.next;
    }
}

#ifdef __TEST__
int
main(int argc, char** argv)
{

    printf("START %ld\n", (long)brk_(NULL));
    struct stringllnode* Head = NULL;
    append_all(&Head, argv, argc);
    print_list(Head);
    print_malloc_list();
    printf("first alloc %ld Head = %ld\n", (long)brk_(NULL), (long)Head);
    free_list(Head);
    printf("after free %ld Head = %ld\n", (long)brk_(NULL), (long)Head);

    Head = NULL;

    print_free_list();
    print_malloc_list();
    append_all(&Head, argv, argc);
    print_list(Head);
    print_malloc_list();
    printf("first alloc %ld Head = %ld\n", (long)brk_(NULL), (long)Head);
    free_list(Head);
    printf("after free %ld Head = %ld\n", (long)brk_(NULL), (long)Head);

    Head = NULL;

    print_free_list();
}
#endif
