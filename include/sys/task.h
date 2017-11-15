#ifndef _TASK_H
#define _TASK_H

/*
 * Don't change the order of members of the following structures
 * Context switch might fail
 */
typedef struct _regstruct
{
    uint64_t rbp;
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsp;
    uint64_t flags;
} __attribute__((packed)) __attribute__((aligned(64))) reg_struct;

typedef struct _taskstruct
{
    reg_struct regs;
    pid_t pid;
    pid_t ppid;
    void* stack_page;
} __attribute__((packed)) __attribute__((aligned(64))) task_struct;

void trial_sched();
void task_trial_userland();
task_struct* task_create();
void task_destroy(task_struct* t);
void task_yield();
task_struct* task_get_this_task_struct();
void task_enter_ring3(void* start);
#endif
