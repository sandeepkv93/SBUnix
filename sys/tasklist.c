#include <sys/alloc.h>
#include <sys/defs.h>
#include <sys/interrupts.h>
#include <sys/kprintf.h>
#include <sys/task.h>
#include <sys/vma.h>

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
    kprintf("task++\n");
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

bool
tasklist_remove_task(pid_t pid)
{
    kprintf("task--\n");
    tasklist_node* curr = tasklist_head;
    while (curr) {
        if (curr->task->pid == pid) {
            // Found
            break;
        }
        curr = curr->next;

        if (curr == tasklist_head) {
            // One loop complete. Not found
            return FALSE;
        }
    }

    if (!curr) {
        return FALSE;
    }

    if (curr->next == curr) {
        // Only one element
        tasklist_head = NULL;
        kfree((void*)curr);
        return TRUE;
    }

    if (tasklist_head == curr) {
        // task_head to be deleted
        tasklist_head = tasklist_head->next;
    }

    curr->next->prev = curr->prev;
    curr->prev->next = curr->next;
    kfree((void*)curr);

    return TRUE;
}

task_struct*
tasklist_schedule_task()
{
    // We track the task to be scheduled next using task_head
    tasklist_node* t;
    if (tasklist_head) {
        t = tasklist_head;
        tasklist_head = tasklist_head->next;
        return t->task;
    }
    return NULL;
}
