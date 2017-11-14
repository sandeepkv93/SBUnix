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
} __attribute__((packed)) __attribute__((aligned(64))) task_struct;

void trial_sched();
void task_trial_userland();
#endif
