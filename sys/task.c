#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/task.h>

extern void switch_to(task_struct*, task_struct*);
uint8_t second_stack[4096] __attribute__((aligned(16)));
task_struct tasks[2];
task_struct *me, *next;

void
sched()
{
    next = &tasks[0];
    me = &tasks[1];
}

void
yield()
{
    switch_to(me, next);
}
void
yield2()
{
    switch_to(next, me);
}

void
thread2()
{
    while (1) {
        yield2();
        signalme('-');
    }
}

void
trial_sched()
{
    me = &tasks[0];
    next = &tasks[1];
    uint64_t* stack_top;
    stack_top = (uint64_t*)&second_stack[4096];
    stack_top--;
    *stack_top = (uint64_t)thread2;
    stack_top--;
    *stack_top = (uint64_t)next;
    next->regs.rsp = (uint64_t)stack_top;
    while (1) {
        yield();
        signalme('|');
    }
}
