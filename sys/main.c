#include <stdarg.h>
#include <string.h>
#include <sys/ahci.h>
#include <sys/defs.h>
#include <sys/elf64.h>
#include <sys/gdt.h>
#include <sys/interrupts.h>
#include <sys/kprintf.h>
#include <sys/paging.h>
#include <sys/pci.h>
#include <sys/tarfs.h>
#include <sys/task.h>
#include <sys/term.h>
#include <test.h>
#define INITIAL_STACK_SIZE 4096
uint8_t initial_stack[INITIAL_STACK_SIZE] __attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;

void
start(uint32_t* modulep, void* physbase, void* physfree)
{
    struct smap_t
    {
        uint64_t base, length;
        uint32_t type;
    } __attribute__((packed)) * smap;
    while (modulep[0] != 0x9001)
        modulep += modulep[1] + 2;
    for (smap = (struct smap_t*)(modulep + 2);
         smap < (struct smap_t*)((char*)modulep + modulep[1] + 2 * 4); ++smap) {
        if (smap->type == 1 /* memory */ && smap->length != 0) {
            paging_pagelist_add_addresses(smap->base,
                                          smap->base + smap->length);
        }
    }
    paging_pagelist_create(physfree);
    paging_create_pagetables();
    term_clear_screen();

    kprintf("physfree %p\n", (uint64_t)physfree);
    kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);

    register_idt();
    pic_init();
    enable_interrupts(TRUE);
    walk_through_tarfs(&_binary_tarfs_start);
    /*task_trial_userland();*/
    test_kprintf();
    /*test_kmalloc_kfree();*/
    /*test_tasklist();*/
    test_sched();
    elf_read();
    /*ahci_discovery();*/
    /*ahci_readwrite_test();*/
    while (1)
        ;
}

void
boot(void)
{
    // note: function changes rsp, local stack variables can't be practically
    // used
    // register char *offset1, *offset2;
    register char* offset2;

    for (offset2 = (char*)0xb8001; offset2 < (char*)0xb8000 + 160 * 25;
         offset2 += 2)
        *offset2 = 7 /* white */;
    __asm__ volatile("cli;"
                     "movq %%rsp, %0;"
                     "movq %1, %%rsp;"
                     : "=g"(loader_stack)
                     : "r"(&initial_stack[INITIAL_STACK_SIZE]));
    init_gdt();
    start((uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem -
                      (uint64_t)&physbase),
          (uint64_t*)&physbase, (uint64_t*)(uint64_t)loader_stack[4]);
    /*
    for(offset1 = "\0", offset2 = (char*)0xb8000; *offset1; offset1 += 1,
    offset2 += 2) {
            *offset2 = *offset1;
    }
    */
    while (1)
        __asm__ volatile("hlt");
}
