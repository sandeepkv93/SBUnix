#ifndef _TASKLIST_H
#define _TASKLIST_H
#include <sys/task.h>
void tasklist_add_task(task_struct* task);
bool tasklist_remove_task(pid_t pid);
task_struct* tasklist_schedule_task();
void tasklist_exit(uint64_t exit_code);
pid_t tasklist_wait(int status);
pid_t tasklist_waitpid(pid_t child_pid);
void tasklist_set_task_state(pid_t pid, task_state state);
task_struct* tasklist_find_task(task_state state);
void tasklist_decrement_sleep_time();
#endif
