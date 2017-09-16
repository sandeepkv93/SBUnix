#include <sys/keyboard.h>
#include <sys/kprintf.h>
#include <sys/pic.h>
#include <sys/string.h>

extern char g_keymap[], g_keymap_shift[];

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
pic_get_idt_entry(void* address)
{
    long addr = (long)address;
    struct_idt_entry member;
    member.offset_1 = (addr & 0xffff);
    member.offset_2 = ((addr >> 16) & 0xffff);
    member.offset_3 = ((addr >> 32) & 0xffffffff);
    member.selector = 8;
    member.ist = 0;
    member.zero = 0;
    member.type_attr = 0x8E;
    return member;
}

void
register_isr(int intn, void* handler)
{
    idt[intn] = pic_get_idt_entry(handler);
}

void
print_time(int seconds)
{
    char str[20];
    int len = 0;
    len = sprintf(str, "Time since boot %d", seconds);
    cursor_move(24, 80 - (len));
    kprintf(str);
}
void
timer_handler()
{
    static int seconds = 0;
    static int counter = 0;
    push_regs();
    counter++;
    if (counter == 41) {
        seconds++;
        counter = 0;
        print_time(seconds);
    }
    outb(PIC1_COMMAND, PIC_EOI);
    pop_regs();
    return_isr();
}

void
timer_setup()
{
    __asm__("mov $0x34, %al;"
            "out %al, $0x43;"   // load control register
            "mov $29102, %eax;" // 1193182 = (41 * 29102) = 41*2*14551//
            "out %al, $0x40;"
            "mov %ah, %al;"
            "out %al, $0x40;");
}

// TODO
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
    char* s = "Key press:%c%c\0";
    char a = ' ', b;
    push_regs();
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
                cursor_move(24, (80 - 35));
                a = is_ctrl_pressed ? '^' : ' ';
                b = is_shift_pressed ? g_keymap_shift[code] : g_keymap[code];
                kprintf(s, a, b);
            }
    }

    outb(PIC1_COMMAND, PIC_EOI);
    pop_regs();
    return_isr();
}

void
fake_isr()
{
    push_regs();
    kprintf("unregistered interrupt");
    outb(PIC1_COMMAND, PIC_EOI);
    /*
        if (irq > 7)
            outb(PIC2_COMMAND, PIC_EOI);
    */
    pop_regs();
    return_isr();
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
    int i;
    for (i = 0; i < 256; i++) {
        idt[i] = pic_get_idt_entry((void*)fake_isr);
    }
    timer_setup(); // setup PIT
    register_isr(0x20, (void*)timer_handler);
    register_isr(0x21, (void*)kb_isr);
    lidt(idt, sizeof(struct_idt_entry) * 256 - 1);
}
