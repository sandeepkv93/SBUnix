#include <sys/alloc.h>
#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/interrupts.h>
#include <sys/kprintf.h>
#include <sys/task.h>
#include <sys/tasklist.h>
#include <sys/vma.h>

extern void sched_switch_kthread(task_struct*, task_struct*);
extern void sched_enter_ring3(uint64_t* stack_top, void* __start);

task_struct t;
task_struct *next, *me = &t;
pid_t g_pid = 1;

task_struct*
task_get_this_task_struct()
{
    return me;
}
void
task_initial_setup()
{
    me = next;
}

task_struct*
task_create(void* callback)
{
    uint64_t* stack_top;

    // Allocate memory for task struct and stack
    task_struct* this_task = kmalloc(sizeof(task_struct));
    this_task->stack_page = alloc_get_page();

    // Stack grows downwards, we'll start stack_top from page boundary
    stack_top = (uint64_t*)((uint64_t)this_task->stack_page + VMA_PAGE_SIZE);
    stack_top--;

    // This will allow us to call the callback once initialization is run
    *stack_top = (uint64_t)callback;
    stack_top--;

    // For first time, we need some initialization to run
    *stack_top = (uint64_t)task_initial_setup;
    stack_top--;

    // Save the task_struct on the stack
    *stack_top = (uint64_t)this_task;

    this_task->regs.rsp = (uint64_t)stack_top;
    this_task->pid = g_pid++;

    tasklist_add_task(this_task);
    return this_task;
}

void
task_destroy(task_struct* t)
{
    alloc_free_page(t->stack_page);
    kfree(t);
}

void
task_yield()
{
    next = tasklist_schedule_task();
    sched_switch_kthread(me, next);
    me = next;
}

void
task_enter_ring3(void* __start)
{
    // Get one page_frame for stack, we'll make user stack as the page just
    // below the kernel boundary
    uint64_t stackpage_v_addr = VMA_KERNMEM - VMA_PAGE_SIZE;
    uint64_t stackpage_p_addr = (uint64_t)vma_pagelist_get_frame();
    vma_add_pagetable_mapping(stackpage_v_addr, stackpage_p_addr);

    // Stack grows downwards so we need to give the address of next page
    set_tss_rsp((void*)me->stack_page + VMA_PAGE_SIZE);
    sched_enter_ring3((uint64_t*)stackpage_v_addr + VMA_PAGE_SIZE, __start);
}
