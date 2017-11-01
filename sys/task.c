#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/task.h>

extern void switch_to(task_struct*, task_struct*);
uint8_t second_stack[4096] __attribute__((aligned(16)));
task_struct tasks[2];
task_struct *me, *next;

void
yield()
{
    switch_to(me, next);
}

void
thread2()
{
    kprintf("Thread X! Hello from the other side\n");
    while (1)
        ;
    yield();
}

void
trial_sched()
{
    me = &tasks[0];
    next = &tasks[1];
    uint64_t* stack_top;
    stack_top = (uint64_t*)&second_stack[4096];
    stack_top--;
    stack_top--;
    stack_top--;
    stack_top--;
    stack_top--;
    stack_top--;
    *stack_top = (uint64_t)thread2;
    stack_top--;
    *stack_top = (uint64_t)next;
    next->regs.rsp = (uint64_t)stack_top;
    next->regs.rbp = (uint64_t)stack_top;
    yield();
    kprintf("Thread 1! Hello from the other side\n");
}
