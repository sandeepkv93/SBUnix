#include <stdarg.h>
#include <string.h>
#include <sys/ahci.h>
#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/kprintf.h>
#include <sys/pic.h>
#include <sys/tarfs.h>

#define INITIAL_STACK_SIZE 4096
uint8_t initial_stack[INITIAL_STACK_SIZE] __attribute__((aligned(16)));
uint32_t* loader_stack;
extern char kernmem, physbase;

static inline int
are_interrupts_enabled()
{
    unsigned long flags;
    __asm__ __volatile("pushf\n\t"
                       "pop %0"
                       : "=g"(flags));
    return flags & (1 << 9);
}

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
            kprintf("Available Physical Memory [%p-%p]\n", smap->base,
                    smap->base + smap->length);
        }
    }
    kprintf("physfree %p\n", (uint64_t)physfree);
    kprintf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);

    char* s = "Alice";
    char* st = "Wonderland";
    char ch = 'I';
    long j, i = 100;
    signalme('H');
    kprintf_("Hello, %c am %s. Welcome to %s. Your score is %d. Well done! "
             "Your score in hex is %x",
             ch, s, st, i, &i);
    register_idt();
    pic_init();
    enable_interrupts(TRUE);
    are_interrupts_enabled() ? signalme('Y') : signalme('N');
    while (1) {
        for (i = 0; i != 32768; i++) {
            for (j = 0; j != 32768; j++)
                j = j + 1 - 1;
        }
        are_interrupts_enabled() ? signalme('Y') : signalme('N');
    };
}

void
boot(void)
{
    // note: function changes rsp, local stack variables can't be practically
    // used
    // register char *temp1, *temp2;
    register char* temp2;

    for (temp2 = (char*)0xb8001; temp2 < (char*)0xb8000 + 160 * 25; temp2 += 2)
        *temp2 = 7 /* white */;
    __asm__("cli;"
            "movq %%rsp, %0;"
            "movq %1, %%rsp;"
            : "=g"(loader_stack)
            : "r"(&initial_stack[INITIAL_STACK_SIZE]));
    init_gdt();
    start((uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem -
                      (uint64_t)&physbase),
          (uint64_t*)&physbase, (uint64_t*)(uint64_t)loader_stack[4]);
    /*
    for(temp1 = "\0", temp2 = (char*)0xb8000; *temp1; temp1 += 1, temp2 += 2) {
            *temp2 = *temp1;
    }
    */
    while (1)
        ;
}
