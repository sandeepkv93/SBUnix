#include <sys/defs.h>
#include <sys/task.h>
#include <sys/term.h>

extern void sched_switch_kthread(task_struct*, task_struct*);
extern void sched_enter_ring3(uint64_t* rsp, void* __start);
uint8_t second_stack[4096] __attribute__((aligned(16)));
task_struct tasks[2];
task_struct *me, *next;

void
task_sched()
{
    task_struct* tmp;
    tmp = next;
    next = me;
    me = tmp;
}

void
task_yield()
{
    task_sched();
    sched_switch_kthread(me, next);
}

void
sample_thread()
{
    while (1) {
        task_yield();
        term_set_glyph('2');
    }
}

void
task_create_thread(void thread_callback())
{
    // TODO use kmalloc to allocate stack space. create task on the go
    uint64_t* stack_top;
    me = &tasks[0];
    next = &tasks[1];
    stack_top = (uint64_t*)&second_stack[4096];
    stack_top--;
    *stack_top = (uint64_t)thread_callback;
    stack_top--;
    *stack_top = (uint64_t)next;
    next->regs.rsp = (uint64_t)stack_top;

    // TODO This call is a hack, because we need to setup next and me initially
    task_sched();
}

void
trial_sched()
{
    task_create_thread(sample_thread);

    while (1) {
        task_yield();
        term_set_glyph('1');
    }
}

void
sample_userthread()
{
    while (1)
        ;
}

void
task_trial_userland()
{
    uint64_t small_stack[10];
    sched_enter_ring3(small_stack, sample_userthread);
}
