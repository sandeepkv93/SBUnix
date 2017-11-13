#include <sys/defs.h>
#include <sys/interrupts.h>
#include <sys/kprintf.h>
#include <sys/task.h>
#include <sys/term.h>
#include <sys/vma.h>

extern void sched_switch_kthread(task_struct*, task_struct*);
extern void sched_enter_ring3(uint64_t* stack_top, void* __start);
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
smalle(uint8_t* x)
{
    if (*x > 100)
        *x = 20;
}

void
sample_userthread__start()
{
    uint8_t i = 30;
    kprintf("user process starts");
    while (1) {
        i += 1;
        term_set_glyph((char)i);
        smalle(&i);
    }
}

void
task_trial_userland()
{
    // Get one page_frame for stack, we'll make user stack as the page just
    // below the kernel boundary
    uint64_t stackpage_v_addr = VMA_KERNMEM - PAGE_SIZE;
    uint64_t stackpage_p_addr = (uint64_t)vma_pagelist_getpage();
    vma_add_pagetable_mapping(stackpage_v_addr, stackpage_p_addr);

    // Stack grows downwards so we need to give the address of next page
    sched_enter_ring3((uint64_t*)stackpage_v_addr + PAGE_SIZE,
                      sample_userthread__start);
}
