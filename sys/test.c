#include <string.h>
#include <sys/alloc.h>
#include <sys/kprintf.h>
#include <sys/vma.h>
#include <test.h>

uint64_t test_address;

struct test_struct
{
    int num;
    char* str;
};

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
test_get_free_page()
{
    uint64_t* p = (uint64_t*)get_free_page();
    *p = 20;
    kprintf("First page alloc\nValue at %x is %d\n", p, *p);
    p = (uint64_t*)get_free_page();
    *p = 40;
    kprintf("Second page alloc\nValue at %x is %d\n", p, *p);

    // create a linked list with two struct pagelist_t
    kprintf("Third page alloc\n");
    struct pagelist_t* ele1 = (struct pagelist_t*)get_free_page();

    ele1->present = 1;
    ele1->next = NULL;
    struct pagelist_t* ele2 = (struct pagelist_t*)(ele1 + 1);
    ele2->present = 0;
    ele2->next = ele1;
    kprintf("addr of ele1 %p\naddr of ele2 %p\n ele2->ele1? %p\n", ele1, ele2,
            ele2->next);
    kprintf("ele1 present %d\nele2 present %d\n", ele1->present, ele2->present);
}

void
test_kmalloc_kfree()
{
    // basic tests for different data types
    int* p[257];
    int i;
    int loop_size = 0;
    for (i = 0; i < loop_size; i++) {
        p[i] = kmalloc(sizeof(int));
        kprintf("address %p\n", p[i]);
        *(p[i]) = i;
        kprintf("value %d\n", *(p[i]));
    }
    // simulate a call to get_free_page from a different function
    void* dummy = get_free_page();
    kprintf("Dummy page %p", dummy);
    for (i = 0; i < loop_size; i++) {
        kfree(p[i]);
    }
    for (i = 0; i < loop_size; i++) {
        p[i] = kmalloc(sizeof(int));
        kprintf("address %p\n", p[i]);
        *(p[i]) = i;
        kprintf("value %d\n", *(p[i]));
    }

    struct test_struct* struct_p[4096];
    for (i = 0; i < 3; i++) {
        struct_p[i] =
          (struct test_struct*)kmalloc(sizeof(struct test_struct) * 4096);
        kprintf("address %p\n", struct_p[i]);
        struct_p[i]->num = i;
        struct_p[i]->str = kmalloc(sizeof(char) * 20);
        (struct_p[i]->str)[0] = 'H';
        (struct_p[i]->str)[1] = 'e';
        (struct_p[i]->str)[2] = '\0';
        // strcpy(struct_p[i]->str, "Hello world!");
        kprintf("str = %s  num = %d\n", struct_p[i]->str, struct_p[i]->num);
    }
}
