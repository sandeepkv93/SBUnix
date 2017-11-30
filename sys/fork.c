#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/paging.h>
#include <sys/string.h>
#include <sys/task.h>

extern uint64_t sched_fork_wrapper(task_struct*, task_struct*);
void paging_mark_pages(int flags);
extern void paging_enable(void*);
uint64_t* t_stack_top;
uint64_t
fork_cow(uint64_t* page_va_addr, int level)
{
    uint64_t *temp_va, *pagetable;
    uint64_t frame_addr, table_entry_addr, pt_offset, next_table_addr;
    frame_addr = (uint64_t)paging_pagelist_get_frame();
    temp_va =
      (uint64_t*)PAGING_PAGE_COPY_TEMP_VA + (level - 1) * PAGING_PAGE_SIZE;
    pt_offset = PAGING_PAGE_TABLE_OFFSET((uint64_t)temp_va);

    paging_flush_tlb();
    paging_add_pagetable_mapping((uint64_t)temp_va, frame_addr);

    for (int i = 0; i < PAGING_TABLE_ENTRIES; i++) {

        if (!(page_va_addr[i] & PAGING_PAGE_PRESENT)) {
            continue;
        }

        next_table_addr = ((uint64_t)page_va_addr << 9) | (i << 12);

        if (page_va_addr[i] & PAGING_PT_LEVEL4) {

            if (next_table_addr < PAGING_KERNMEM) {
                page_va_addr[i] =
                  (page_va_addr[i] | PAGING_PAGE_COW) & ~PAGING_PAGE_W_ONLY;
            }

            table_entry_addr = page_va_addr[i];
            paging_inc_ref_count(next_table_addr);

        } else if (level == 1 && i == PAGING_TABLE_ENTRIES - 1) {

            table_entry_addr = frame_addr;

        } else {

            table_entry_addr = fork_cow((uint64_t*)next_table_addr, level + 1);
        }

        temp_va[i] = table_entry_addr;
        temp_va[i] |= PAGING_PAGETABLE_PERMISSIONS;
    }

    pagetable = paging_get_pt_vaddr((uint64_t)temp_va);
    pagetable[pt_offset] = PAGING_PAGE_NOT_PRESENT;
    paging_flush_tlb();

    return frame_addr;
}

pid_t
fork_copy_stack(task_struct* parent_task, task_struct* child_task,
                uint64_t parent_stackend)
{
    // Only one enters, but two processes exit. How cool!
    memcpy(child_task->stack_page, task_get_this_task_struct()->stack_page,
           PAGING_PAGE_SIZE);

    child_task->regs.rsp =
      (uint64_t)((parent_stackend - (uint64_t)parent_task->stack_page) +
                 child_task->stack_page);

    t_stack_top = (uint64_t*)child_task->regs.rsp;

    t_stack_top--;
    *t_stack_top = (uint64_t)task_initial_setup;

    t_stack_top--;
    *t_stack_top = (uint64_t)child_task;

    child_task->regs.rsp = (uint64_t)t_stack_top;

    child_task->regs.rax = 0;

    // Add child_task_struct & task_initial_setup on child task
    return child_task->pid;
}

pid_t
fork(void)
{
    task_struct* child_task = task_create(NULL);
    task_struct* parent_task = task_get_this_task_struct();
    uint64_t current_pml4_va = PAGING_PML4_SELF_REFERENCING;

    // TODO add task_yield here if we need most recent regs, careful because
    // child might get scheduled

    child_task->regs = parent_task->regs;
    child_task->regs.cr3 = fork_cow((void*)current_pml4_va, 1);
    child_task->ppid = parent_task->pid;
    strcpy(child_task->binary_name, parent_task->binary_name);

    // TODO deep copy this
    child_task->vma_list = parent_task->vma_list;

    /*
    uint64_t* temp_va = (uint64_t*)PAGING_PAGE_COPY_TEMP_VA;
    paging_add_pagetable_mapping((uint64_t)temp_va,
    (uint64_t)child_task->pml4_frame_addr);
    paging_enable(child_task->pml4_frame_addr);
    */

    for (int i = 0; i < TASK_FILETABLE_SIZE; i++) {
        child_task->filetable[i] = parent_task->filetable[i];
    }

    return sched_fork_wrapper(parent_task, child_task);

    return 0;
}
