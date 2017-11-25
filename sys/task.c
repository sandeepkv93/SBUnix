#include <sys/alloc.h>
#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/interrupts.h>
#include <sys/kprintf.h>
#include <sys/paging.h>
#include <sys/string.h>
#include <sys/task.h>
#include <sys/tasklist.h>

#define ARGV_ENV_MAX 200

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
    stack_top = (uint64_t*)((uint64_t)this_task->stack_page + PAGING_PAGE_SIZE);
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

    // Enable interrupts in new thread. enable_interrupts(TRUE)
    this_task->regs.flags = 0x200;

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
task_exec_ring3(char* bin_name, char** argv, char** envp)
{
    // TODO We have some hardcoding here and lot of ugly code, needs to be
    // cleaned

    // Populate VMAs in the task_struct
    elf_read(bin_name);

    // Get one page_frame mapped in stack region, we'll use it to store argv and
    // env
    // Refer crt1.c for a diagram

    uint64_t stackpage_v_addr = PAGING_KERNMEM - PAGING_PAGE_SIZE;
    uint64_t stackpage_p_addr = (uint64_t)paging_pagelist_get_frame();
    paging_add_pagetable_mapping(stackpage_v_addr, stackpage_p_addr);

    uint64_t* argv_envp_data = (uint64_t*)stackpage_v_addr;
    char* curr_string_data = (char*)(argv_envp_data + ARGV_ENV_MAX);

    uint64_t argc = 0;
    uint64_t envc = 0;

    for (int i = 0; argv[i] != NULL; i++) {
        strcpy(curr_string_data, argv[i]);
        argv_envp_data[i + 1] = (uint64_t)curr_string_data;
        curr_string_data += strlen(curr_string_data) + 1; // 1 for '\0'
        argc++;
    }

    // First value on stack is argc
    argv_envp_data[0] = argc;

    // We need a NULL after all the argv
    argv_envp_data[1 + argc + 1] = (uint64_t)NULL;

    for (int i = 0; envp[i] != NULL; i++) {
        strcpy(curr_string_data, envp[i]);
        argv_envp_data[i + argc + 2] = (uint64_t)curr_string_data;
        curr_string_data += strlen(curr_string_data) + 1;
        envc++;
    }

    // We need a NULL after all the envs
    argv_envp_data[1 + (argc + 1) + envc + 1] = (uint64_t)NULL;

    // TODO: Remove below
    // Get another page_frame for which the user programs can use to write,
    // not needed after auto-growing stack

    stackpage_v_addr -= PAGING_PAGE_SIZE;
    stackpage_p_addr = (uint64_t)paging_pagelist_get_frame();
    paging_add_pagetable_mapping(stackpage_v_addr, stackpage_p_addr);

    // Stack grows downwards so we need to give the address of next page
    set_tss_rsp((void*)me->stack_page + PAGING_PAGE_SIZE);
    sched_enter_ring3((uint64_t*)(stackpage_v_addr + PAGING_PAGE_SIZE),
                      (void*)task_get_this_task_struct()->entry_point);
}
