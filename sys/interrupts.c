#include <sys/interrupts.h>
#include <sys/keyboard.h>
#include <sys/kprintf.h>
#include <sys/paging.h>
#include <sys/string.h>
#include <sys/syscall.h>
#include <sys/tarfs.h>
#include <sys/task.h>
#include <sys/term.h>
#include <sys/timer.h>
#include <sys/vma.h>
extern void* g_exceptions[];
extern char g_keymap[], g_keymap_shift[];
extern void syscall_isr_asm();
extern void timer_isr_asm();
extern void kb_isr_asm();
extern void page_fault_isr_asm();

#define RING0 0
#define RING3 3

/* Refer http://wiki.osdev.org/Interrupt_Descriptor_Table#Structure_AMD64 */
#define IDT_ENTRY_PRESENT 0x80
#define IDT_ENTRY_RING0 0x00
#define IDT_ENTRY_RING3 0x60
#define IDT_ENTRY_INTERRUPT 0x0E
#define IDT_ENTRY_TRAP 0x0F

void
outb(uint16_t port, uint8_t value)
{
    __asm__("outb %0, %1;" : : "a"(value), "Nd"(port));
}

uint8_t
inb(uint16_t port)
{
    uint8_t value;
    __asm__("inb %1,%0;" : "=a"(value) : "Nd"(port));
    return value;
}

bool
are_interrupts_enabled()
{
    unsigned long flags;
    __asm__ __volatile("pushf\n\t"
                       "pop %0"
                       : "=g"(flags));
    if (flags & (1 << 9)) {
        return TRUE;
    } else {
        return FALSE;
    }
}
// TODO we need to be able to enable and disable specific interrupts

void
enable_interrupts(bool status)
{
    if (status) {
        __asm__("sti;");
    } else {
        __asm__("cli;");
    }
}

inline void
push_regs()
{
    // Push registers
    __asm__("push %rsi;"
            "push %rdi;"
            "push %rbp;"
            "push %rax;"
            "push %rbx;"
            "push %rcx;"
            "push %rdx;");
}
inline void
pop_regs()
{
    // Pop registers
    __asm__("pop %rdx;"
            "pop %rcx;"
            "pop %rbx;"
            "pop %rax;"
            "pop %rbp;"
            "pop %rdi;"
            "pop %rsi;"
            "add $0x8,%rsp;"); // Because gcc adds 'sub 0x8, RSP' ¯\_(ツ)_/¯
}

inline void
return_isr()
{
    __asm__("iretq;");
}

struct_idt_entry
pic_get_idt_entry(void* address, uint8_t ring)
{
    long addr = (long)address;
    struct_idt_entry member;
    member.offset_1 = (addr & 0xffff);
    member.offset_2 = ((addr >> 16) & 0xffff);
    member.offset_3 = ((addr >> 32) & 0xffffffff);
    member.selector = 8;
    member.ist = 0;
    member.zero = 0;
    if (ring == RING3) {
        member.type_attr = IDT_ENTRY_PRESENT | IDT_ENTRY_RING3 | IDT_ENTRY_TRAP;
    } else {
        member.type_attr =
          IDT_ENTRY_PRESENT | IDT_ENTRY_RING0 | IDT_ENTRY_INTERRUPT;
    }
    return member;
}

void
register_isr(int intn, void* handler, uint8_t ring)
{
    idt[intn] = pic_get_idt_entry(handler, ring);
}

// TODO Make more readable
void
pic_init()
{
    outb(0x20, 0x11);

    outb(0xa0, 0x11);

    /*Remaps IRQ0-IRQ7 to 0x20-0x27 in interrupt vector table*/

    outb(0x21, 0x20);

    /*Remaps IRQ8-IRQ15 to 0x28-0x2F in interrupt vector table*/

    outb(0xa1, 0x28);

    /*PIC2 is connected to PIC1 via IRQ2*/

    outb(0x21, 0x04);

    outb(0xa1, 0x02);

    /*Enables 8086/88 mode*/

    outb(0x21, 0x01);

    outb(0xa1, 0x01);

    /*Disables all interrupts from IRQ0-IRQ7*/

    outb(0x21, 0xfc);

    /*Disables all interrupts from IRQ8-IRQ15*/

    outb(0xa1, 0xff);
}

void
kb_isr()
{
    // TODO use string functions and remove hard coding
    static bool is_shift_pressed = FALSE, is_ctrl_pressed = FALSE;
    uint8_t code;
    code = inb(0x60);
    switch (code) {
        case KEYCODE_SHIFT:
        case KEYCODE_LSHIFT:
            is_shift_pressed = TRUE;
            break;
        case (KEYCODE_SHIFT + KEYCODE_RELEASE):
        case (KEYCODE_LSHIFT + KEYCODE_RELEASE):
            is_shift_pressed = FALSE;
            break;
        case KEYCODE_LCTRL:
            is_ctrl_pressed = TRUE;
            break;
        case (KEYCODE_LCTRL + KEYCODE_RELEASE):
            is_ctrl_pressed = FALSE;
            break;
        default:
            if ((code > 0) && (code < g_keymap[0])) {
                term_set_keypress(code, is_ctrl_pressed, is_shift_pressed);
            }
    }

    outb(PIC1_COMMAND, PIC_EOI);
}

void
page_fault_handler(uint64_t v_addr)
{
    struct vma_struct* list = task_get_this_task_struct()->vma_list;
    uint64_t p_addr;
    // remove these variables
    char* p;
    void* addr = (void*)0xffffffff812507c0 + sizeof(struct posix_header_ustar);

    volatile uint64_t offset;
    while (list != NULL) {
        if (v_addr >= list->vma_start && v_addr < list->vma_end) { // alloc page
            p_addr = (uint64_t)paging_pagelist_get_frame();
            // TODO: Take care if offset is big
            paging_add_pagetable_mapping(v_addr & 0xfffffffffffff000, p_addr);
            // TODO: open,read,close
            // TODO: if anon mapping, skip above step.
            addr += list->vma_file_offset;
            p = (char*)list->vma_start;
            offset = list->vma_file_size;
            while (offset--) {
                *p = *(char*)addr;
                p++;
                addr++;
            }
            break;
        }

        list = list->vma_next;
    }
    if (list == NULL) {
        // TODO : kill process, Seg fault
        /*kprintf("Segmentation fault. Be prepared to die.");*/
        term_set_glyph(10, 'S');
        while (1)
            ;
    }
}

void
fake_isr()
{
    kprintf("unregistered interrupt :( ");
    outb(PIC1_COMMAND, PIC_EOI);
    /*
        if (irq > 7)
            outb(PIC2_COMMAND, PIC_EOI);
    */
}

void
lidt(void* idt_address, uint16_t size)
{
    struct
    {
        uint16_t length;
        uint64_t base;
    } __attribute__((packed)) s_idt = { size, (uint64_t)idt_address };

    __asm__("lidt %0;" : : "m"(s_idt));
}

void
register_idt()
{

    for (int i = 0; i < 32; i++) {
        idt[i] = pic_get_idt_entry(g_exceptions[i], RING0);
    }
    for (int i = 32; i < 256; i++) {
        idt[i] = pic_get_idt_entry((void*)fake_isr, RING0);
    }

    timer_setup(); // setup PIT
    register_isr(0x0E, (void*)page_fault_isr_asm, RING0);
    register_isr(0x20, (void*)timer_isr_asm, RING0);
    register_isr(0x21, (void*)kb_isr_asm, RING0);
    // Using int 0x46 for syscalls. This is to denote deviation from linux
    register_isr(0x46, (void*)syscall_isr_asm, RING3);

    lidt(idt, sizeof(struct_idt_entry) * 256 - 1);
}
