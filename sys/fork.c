#include <sys/paging.h>
#include <sys/string.h>
#include <sys/task.h>

extern uint64_t sched_fork_wrapper(task_struct*, task_struct*);
void paging_mark_pages(int flags);

uint64_t* t_stack_top;

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
    // Call Gaani's function

    task_struct* child_task = task_create(NULL);
    task_struct* parent_task = task_get_this_task_struct();

    // TODO add task_yield here if we need most recent regs, careful because
    // child might get scheduled

    child_task->regs = parent_task->regs;

    for (int i = 0; i < TASK_FILETABLE_SIZE; i++) {
        child_task->filetable[i] = parent_task->filetable[i];
    }

    return sched_fork_wrapper(parent_task, child_task);
}
