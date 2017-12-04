#include <sys/alloc.h>
#include <sys/defs.h>
#include <sys/interrupts.h>
#include <sys/kprintf.h>
#include <sys/paging.h>
#include <sys/task.h>

#define INIT_PID 1

typedef struct __tasklist_node
{
    task_struct* task;
    struct __tasklist_node* next;
    struct __tasklist_node* prev;
} tasklist_node;

tasklist_node* tasklist_head = NULL;
void
tasklist_add_task(task_struct* task)
{

    /*
     * We add any new tasks to the end of the list. i.e, prev of head
     * This would allow us to prefer tasks that were running already rather than
     * the newly created process.
     */
    tasklist_node* t_node = (tasklist_node*)kmalloc(sizeof(tasklist_node));

    t_node->task = task;

    if (tasklist_head == NULL) {
        // First node
        t_node->next = t_node;
        t_node->prev = t_node;
        tasklist_head = t_node;
        return;
    }

    t_node->next = tasklist_head;
    t_node->prev = tasklist_head->prev;

    tasklist_head->prev->next = t_node;
    tasklist_head->prev = t_node;
}

tasklist_node*
tasklist_find_node(pid_t pid)
{
    tasklist_node* curr = tasklist_head;
    while (curr) {
        if (curr->task->pid == pid) {
            // Found
            return curr;
        }
        curr = curr->next;

        if (curr == tasklist_head) {
            // One loop complete. Not found
            return NULL;
        }
    }
    return curr;
}

task_struct*
tasklist_get_task(pid_t pid, task_state state)
{
    tasklist_node* node = tasklist_find_node(pid);
    if (node && (state == task_any_state || node->task->state == state))
        return node->task;
    return NULL;
}

task_struct*
tasklist_find_task(task_state state)
{
    // returns head if state is task_any_state
    if (!tasklist_head)
        return NULL;
    tasklist_node* list_iter = tasklist_head;
    do {
        if (state == task_any_state || list_iter->task->state == state) {
            return list_iter->task;
        }
        list_iter = list_iter->next;
    } while (list_iter != tasklist_head);
    return NULL;
}

task_struct*
tasklist_find_one_child(pid_t ppid, task_state state)
{
    // TODO iterate over list and check for matching ppid and state.
    if (!tasklist_head)
        return NULL;
    tasklist_node* list_iter = tasklist_head;
    do {
        if ((list_iter->task->ppid == ppid) &&
            (state == task_any_state || list_iter->task->state == state)) {
            return list_iter->task;
        }
        list_iter = list_iter->next;
    } while (list_iter != tasklist_head);
    return NULL;
}

void
tasklist_set_task_state(pid_t pid, task_state state)
{
    // TODO should NULL be handled?
    task_struct* task = tasklist_get_task(pid, task_any_state);
    task->state = state;
}

bool
tasklist_remove_task(pid_t pid)
{
    tasklist_node* curr = tasklist_find_node(pid);
    if (curr->next == curr) {
        // Only one element
        tasklist_head = NULL;
        /*kfree((void*)curr);*/
        return TRUE;
    }

    if (tasklist_head == curr) {
        // task_head to be deleted
        tasklist_head = tasklist_head->next;
    }
    curr->next->prev = curr->prev;
    curr->prev->next = curr->next;
    /*kfree((void*)curr);*/

    return TRUE;
}

task_struct*
tasklist_schedule_task()
{
    // We track the task to be scheduled next using task_head
    // TODO: return only runnable tasks
    tasklist_node* list_iter = tasklist_head;
    while (1) {
        if (list_iter->task->state == task_runnable) {
            tasklist_head = list_iter->next; // we will give the next process
                                             // priority since all the previous
                                             // are non runnable;
            return list_iter->task;
        }
        list_iter = list_iter->next;
    }
}

// TODO: if none of the children is in zombie state(NULL is returned), change
// the process' state to task_sleep_wait and yield.
// when exit is called on a process, check if the parent is sleeping on
// task_sleep_wait, change it's state to runnable.

void
task_clean(task_struct* task)
{
    while (task->vma_list != NULL) {
        /*kfree(task->vma_list);*/
        task->vma_list = task->vma_list->vma_next;
    }

    // release file objects for 0,1,2. 1000 is dummy intial value
    for (int i = 0; i < 3; i++) {
        /*if ((uint64_t)task->filetable[i] != 1000)
        kfree(task->filetable[i]);*/
    }
    for (int i = 3; i < TASK_FILETABLE_SIZE; i++) {
        /*if (task->filetable[i] != NULL)
        kfree(task->filetable[i]);*/
    }
}

pid_t
tasklist_waitpid(pid_t child_pid)
{
    task_struct* child = tasklist_get_task(child_pid, task_zombie);
    while (1) {
        if (child == NULL) {
            tasklist_set_task_state(task_get_this_task_struct()->pid,
                                    task_sleep_wait);
            task_yield();
        } else {
            task_clean(child);
            return child_pid;
        }
    }
}

pid_t
tasklist_wait(int status)
{
    // if none of the children is in zombie state(NULL is returned), change
    // the process' state to task_sleep_wait and yield.
    pid_t pid = task_get_this_task_struct()->pid;
    pid_t child_pid;
    task_struct* child;
    while (1) {
        child = tasklist_find_one_child(pid, task_zombie);
        if (child == NULL) {
            tasklist_set_task_state(pid, task_sleep_wait);
            task_yield();
        }
        // free child
        else {
            child_pid = child->pid;
            task_clean(child);
            return child_pid;
        }
    }
}

void
tasklist_reparent(pid_t ppid)
{
    tasklist_node* list_iter = tasklist_head;
    if (!tasklist_head) {
        do {
            if (list_iter->task->ppid == ppid) {
                list_iter->task->ppid = INIT_PID;
            }
            list_iter = list_iter->next;
        } while (list_iter != tasklist_head);
    }
}

void
tasklist_exit(uint64_t exit_code)
{
    pid_t pid = task_get_this_task_struct()->pid;
    task_get_this_task_struct()->exit_code = exit_code;
    tasklist_set_task_state(pid, task_zombie);
    // wake up parent process
    // a function that takes pid,current state,new state
    task_struct* parent =
      tasklist_get_task(task_get_this_task_struct()->ppid, task_sleep_wait);
    if (parent)
        parent->state = task_runnable;
    tasklist_reparent(pid);
    paging_free_pagetables((uint64_t*)PAGING_PML4_SELF_REFERENCING, 1);
    task_yield();
}
