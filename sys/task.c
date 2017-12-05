#include <sys/alloc.h>
#include <sys/defs.h>
#include <sys/gdt.h>
#include <sys/interrupts.h>
#include <sys/kprintf.h>
#include <sys/paging.h>
#include <sys/string.h>
#include <sys/task.h>
#include <sys/tasklist.h>
#include <sys/term.h>
#include <sys/vma.h>

#define ARGV_ENV_MAX 200

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

uint64_t
task_initial_setup()
{
    me = next;
    set_tss_rsp((void*)me->stack_page + PAGING_PAGE_SIZE);
    return 0;
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
    this_task->state = task_runnable;
    // Enable interrupts in new thread. enable_interrupts(TRUE)
    this_task->regs.flags = 0x200;

    for (int i = 0; i < TASK_FILETABLE_SIZE; i++) {
        this_task->filetable[i] = NULL;
    }
    this_task->filetable[0] = (vfs_file_object*)TERM_VFS_OBJ;
    this_task->filetable[1] = (vfs_file_object*)TERM_VFS_OBJ;
    this_task->filetable[2] = (vfs_file_object*)TERM_VFS_OBJ;
    strcpy(this_task->cwd, "/");
    tasklist_add_task(this_task);
    return this_task;
}

void
task_destroy(task_struct* task)
{
    tasklist_remove_task(task->pid);
    while (task->vma_list != NULL) {
        kfree(task->vma_list);
        task->vma_list = task->vma_list->vma_next;
    }

    // release file objects for 0,1,2. TERM_VFS_OBJ is dummy intial value
    for (int i = 0; i < TASK_FILETABLE_SIZE; i++) {
        if (task->filetable[i] != NULL &&
            task->filetable[i] != (vfs_file_object*)TERM_VFS_OBJ)
            kfree(task->filetable[i]);
    }
    alloc_free_page(task->stack_page);
    kfree(task);
}

void
task_yield()
{
    term_set_glyph(0, ' ');
    term_set_glyph(1, ' ');
    next = tasklist_schedule_task();
    term_set_glyph(0, '0' + next->pid);
    sched_switch_kthread(me, next);
    me = next;
    set_tss_rsp((void*)me->stack_page + PAGING_PAGE_SIZE);
    term_set_glyph(1, '0' + me->pid);
}

void
task_save_state()
{
    sched_switch_kthread(me, me);
}

void
task_exec_ring3(char* bin_name, char** argv, char** envp)
{
    uint64_t argc = 0;
    uint64_t envc = 0;

    // We use the temporary page to copy data because we are going to a new
    // address space
    uint64_t stackpage_v_addr = PAGING_KERNMEM - PAGING_PAGE_SIZE;
    uint64_t copy_page_va = PAGING_PAGE_COPY_TEMP_VA;
    uint64_t stackpage_p_addr = (uint64_t)paging_pagelist_get_frame();
    uint64_t* argv_envp_data = (uint64_t*)copy_page_va;
    char* curr_string_data = (char*)(argv_envp_data + ARGV_ENV_MAX);
    uint64_t* pml4_va = (uint64_t*)PAGING_PML4_SELF_REFERENCING;

    // TODO We have some hardcoding here and lot of ugly code, needs to be
    // cleaned

    // TODO: Walk current VMA list, for every virtual address (page boundary)
    // check pagetable (PT) and empty the entry. If all entries in PT are 0, we
    // need to free PT frame and mark PD with empty entry and so on. This is
    // complex, there will be a simple alternative. Find it :P
    // For now we can mark the PML4 entries in VMA to present 0

    // Populate VMAs in the task_struct
    if (!vma_read_elf(bin_name)) {
        return;
    }

    // Get one page_frame mapped in stack region, we'll use it to store argv and
    // env. Refer crt1.c for a diagram

    paging_add_pagetable_mapping(copy_page_va, stackpage_p_addr, FALSE);
    paging_flush_tlb();

    for (int i = 0; argv[i] != NULL; i++) {
        strcpy(curr_string_data, argv[i]);
        argv_envp_data[i + 1] =
          ((uint64_t)curr_string_data) - (copy_page_va - stackpage_v_addr);
        curr_string_data += strlen(curr_string_data) + 1; // 1 for '\0'
        argc++;
    }

    // First value on stack is argc
    argv_envp_data[0] = argc;

    // We need a NULL after all the argv
    argv_envp_data[argc + 1] = (uint64_t)NULL;

    for (int i = 0; envp[i] != NULL; i++) {
        strcpy(curr_string_data, envp[i]);
        argv_envp_data[i + argc + 2] =
          ((uint64_t)curr_string_data) - (copy_page_va - stackpage_v_addr);
        curr_string_data += strlen(curr_string_data) + 1;
        envc++;
    }

    // We need a NULL after all the envs
    argv_envp_data[1 + (argc + 1) + envc] = (uint64_t)NULL;

    // TODO: Since it might be COW we might need to decrement ref count, walk
    // PTs
    // TODO remove the below part and use commented out code
    for (int i = 0; i < 510; i++) {
        pml4_va[i] = 0;
    }

    /*
    struct vma_struct* list_iter;
    uint64_t va_iter;
    list_iter = task_get_this_task_struct()->vma_list;

    while (list_iter != NULL) {
        va_iter = list_iter->vma_start;
        while (va_iter < list_iter->vma_end) {

            pml4_va[(PAGING_PML4_OFFSET(va_iter)] = 0;
            va_iter += PAGING_PAGE_SIZE;
        }
        list_iter = list_iter->vma_next;
    }
    */

    // Gotta flush your shitz
    paging_flush_tlb();

    // We made the stack page go away, add the mapping back
    paging_add_pagetable_mapping(stackpage_v_addr, stackpage_p_addr, TRUE);

    // TODO: Is this needed?
    set_tss_rsp((void*)me->stack_page + PAGING_PAGE_SIZE);

    task_save_state(); //?
    // Stack grows downwards so we need to give the address of next page
    sched_enter_ring3((uint64_t*)(stackpage_v_addr),
                      (void*)task_get_this_task_struct()->entry_point);
}

pid_t
task_getpid()
{
    return task_get_this_task_struct()->pid;
}

pid_t
task_getppid()
{
    return task_get_this_task_struct()->ppid;
}

char*
task_get_state_string(task_state s)
{
    switch (s) {
        case task_runnable:
            return "Running";
        case task_zombie:
            return "Zombie";
        case task_sleep_wait:
        case task_sleep_keyboard:
        case task_sleep_timer:
            return "Sleeping";
        case task_any_state:
            return "??";
    }
    return "??";
}
