#include <sys/kprintf.h>
#include <sys/vma.h>
uint64_t cur_kern_heap = VMA_VIDEO + 0x10000;

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
get_free_pages()
{
    uint64_t* v_addr = (uint64_t*)cur_kern_heap;
    vma_add_pagetable_mapping_va(cur_kern_heap);
    cur_kern_heap += PAGE_SIZE;
    return (void*)v_addr;
}

void*
pls_giv_mem(int num_bytes)
{
    static void* cur_page_va = NULL;
    static int cur_page_offset = PAGE_SIZE;
    if (num_bytes + cur_page_offset > (PAGE_SIZE)) {
        cur_page_va = get_free_pages();
        cur_page_offset = 0;
    }
    cur_page_offset += num_bytes;
    return (cur_page_va + cur_page_offset);
}

void
kfree(void* new)
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
print_kfree_list()
{
    Header* temp = freep->meta.next;
    while (temp != freep) {
        temp = temp->meta.next;
    }
}

void
print_kmalloc_list()
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
    kfree((void*)(new + 1));
    return freep;
}

void*
kmalloc(size_t num_bytes)
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
