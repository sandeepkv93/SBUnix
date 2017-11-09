#include <sys/alloc.h>
#include <sys/kprintf.h>
#include <sys/vma.h>
#include <test.h>

uint64_t test_address;

void
testing_function()
{
    test_address = (uint64_t)vma_pagelist_getpage();
    uint8_t* test_ptr = (uint8_t*)test_address;
    for (int i = 0; i < 1024; i++) {
        test_ptr[i] = i;
    }
}

void
test_get_free_pages()
{
    uint64_t* p = (uint64_t*)get_free_pages();
    *p = 20;
    kprintf("First page alloc\nValue at %x is %d\n", p, *p);
    p = (uint64_t*)get_free_pages();
    *p = 40;
    kprintf("Second page alloc\nValue at %x is %d\n", p, *p);

    // create a linked list with two struct pagelist_t
    kprintf("Third page alloc\n");
    struct pagelist_t* ele1 = (struct pagelist_t*)get_free_pages();

    ele1->present = 1;
    ele1->next = NULL;
    struct pagelist_t* ele2 = (struct pagelist_t*)(ele1 + 1);
    ele2->present = 0;
    ele2->next = ele1;
    kprintf("addr of ele1 %p\naddr of ele2 %p\n ele2->ele1? %p\n", ele1, ele2,
            ele2->next);
    kprintf("ele1 present %d\nele2 present %d\n", ele1->present, ele2->present);
}
