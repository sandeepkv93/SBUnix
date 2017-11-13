#include <sys/exceptions.h>
#include <sys/interrupts.h>
#include <sys/kprintf.h>

void
exception_handler31()
{
    kprintf("Exception!! 31");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler30()
{
    kprintf("Exception!! 30");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler29()
{
    kprintf("Exception!! 29");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler28()
{
    kprintf("Exception!! 28");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler27()
{
    kprintf("Exception!! 27");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler26()
{
    kprintf("Exception!! 26");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler25()
{
    kprintf("Exception!! 25");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler24()
{
    kprintf("Exception!! 24");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler23()
{
    kprintf("Exception!! 23");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler22()
{
    kprintf("Exception!! 22");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler21()
{
    kprintf("Exception!! 21");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler20()
{
    kprintf("Exception!! 20");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler19()
{
    kprintf("Exception!! 19");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler18()
{
    kprintf("Exception!! 18");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler17()
{
    kprintf("Exception!! 17");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler16()
{
    kprintf("Exception!! 16");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler15()
{
    kprintf("Exception!! 15");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler14()
{
    kprintf("Pagefault Exception");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler13()
{
    kprintf("Exception!! 13");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler12()
{
    kprintf("Exception!! 12");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler11()
{
    kprintf("Exception!! 11");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler10()
{
    kprintf("Exception!! 10");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler9()
{
    kprintf("Exception!! 9");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler8()
{
    kprintf("Exception!! 8");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler7()
{
    kprintf("Exception!! 7");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler6()
{
    kprintf("Exception!! 6");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler5()
{
    kprintf("Exception!! 5");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler4()
{
    kprintf("Exception!! 4");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler3()
{
    kprintf("Exception!! 3");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler2()
{
    kprintf("Exception!! 2");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler1()
{
    kprintf("Exception!! 1");
    outb(PIC1_COMMAND, PIC_EOI);
}

void
exception_handler0()
{
    kprintf("Exception!! 0");
    outb(PIC1_COMMAND, PIC_EOI);
}
