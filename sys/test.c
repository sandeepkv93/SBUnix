#include <string.h>
#include <sys/alloc.h>
#include <sys/elf64.h>
#include <sys/fork.h>
#include <sys/kprintf.h>
#include <sys/nary.h>
#include <sys/paging.h>
#include <sys/syscall.h>
#include <sys/tarfs.h>
#include <sys/task.h>
#include <sys/tasklist.h>
#include <sys/term.h>
#include <sys/timer.h>
#include <sys/vfs.h>
#include <sys/vma.h>
#include <test.h>

extern uint64_t paging_get_current_cr3();

uint64_t test_address;

struct test_struct
{
    int num;
    char* str;
};

void
test_alloc_get_page()
{
    uint64_t* p = (uint64_t*)alloc_get_page();
    *p = 20;
    kprintf("First page alloc\nValue at %x is %d\n", p, *p);
    p = (uint64_t*)alloc_get_page();
    *p = 40;
    kprintf("Second page alloc\nValue at %x is %d\n", p, *p);

    // create a linked list with two struct pagelist_t
    kprintf("Third page alloc\n");
    struct pagelist_t* ele1 = (struct pagelist_t*)alloc_get_page();

    ele1->present = 1;
    ele1->next = NULL;
    struct pagelist_t* ele2 = (struct pagelist_t*)(ele1 + 1);
    ele2->present = 0;
    ele2->next = ele1;
    kprintf("addr of ele1 %p\naddr of ele2 %p\n ele2->ele1? %p\n", ele1, ele2,
            ele2->next);
    kprintf("ele1 present %d\nele2 present %d\n", ele1->present, ele2->present);
}

void
test_kmalloc_kfree()
{
    // basic tests for different data types
    int* p[257];
    int i;
    int loop_size = 0;
    for (i = 0; i < loop_size; i++) {
        p[i] = kmalloc(sizeof(int));
        kprintf("address %p\n", p[i]);
        *(p[i]) = i;
        kprintf("value %d\n", *(p[i]));
    }
    // simulate a call to alloc_get_page from a different function
    void* dummy = alloc_get_page();
    kprintf("Dummy page %p", dummy);
    for (i = 0; i < loop_size; i++) {
        kfree(p[i]);
    }
    for (i = 0; i < loop_size; i++) {
        p[i] = kmalloc(sizeof(int));
        kprintf("address %p\n", p[i]);
        *(p[i]) = i;
        kprintf("value %d\n", *(p[i]));
    }

    struct test_struct* struct_p[4096];
    for (i = 0; i < 3; i++) {
        struct_p[i] =
          (struct test_struct*)kmalloc(sizeof(struct test_struct) * 4096);
        kprintf("address %p\n", struct_p[i]);
        struct_p[i]->num = i;
        struct_p[i]->str = kmalloc(sizeof(char) * 20);
        (struct_p[i]->str)[0] = 'H';
        (struct_p[i]->str)[1] = 'e';
        (struct_p[i]->str)[2] = '\0';
        // strcpy(struct_p[i]->str, "Hello world!");
        kprintf("str = %s  num = %d\n", struct_p[i]->str, struct_p[i]->num);
    }
}

void
test_tasklist()
{
    task_struct s[3];
    s[0].pid = 1;
    s[1].pid = 2;
    s[2].pid = 3;
    s[3].pid = 4;

    tasklist_add_task(&s[0]);
    tasklist_add_task(&s[1]);
    tasklist_add_task(&s[2]);
    tasklist_add_task(&s[3]);

    kprintf("%d\n", tasklist_remove_task(2)); // Success
    kprintf("%d\n", tasklist_remove_task(2)); // Fail
    kprintf("%d\n", tasklist_remove_task(0)); // Fail

    while (1) {
        // Set character to pid number
        term_set_glyph(2, '0' + tasklist_schedule_task()->pid);
    }
}

long
test_sample_syscall(long syscall, int fd, char* buff, int size)
{
    long sys_no = syscall, arg1 = fd, arg2 = (long)buff, arg3 = size;
    long x = 0;
    __asm__("movq	%1,%%rdi;"
            "movq	%2,%%rsi;"
            "movq	%3,%%rdx;"
            "movq	%4,%%rcx;"
            "int $0x46;"
            "movq    %%rax,%0;"
            : "=r"(x)
            : "r"(sys_no), "r"(arg1), "r"(arg2), "r"(arg3)
            : "%rdi", "%rsi", "%rdx", "%rcx", "%rax");
    return x;
}

void
test_sample_userspace_function()
{
    char m;
    char buff[20];
    m = test_sample_syscall(1, 1, "Hello there\n", 12);
    m = test_sample_syscall(1, 2, "Hello there\n", 12);
    m = test_sample_syscall(1, 1, "Hello there\n", 12);
    m = test_sample_syscall(1, 1, "Hello there\n", 12);
    while (1) {
        m = test_sample_syscall(0, 0, buff, 5);
        m = test_sample_syscall(1, 1, buff, strlen(buff));
        m = test_sample_syscall(1, 1, "\n", 1);
        for (int i = 0; i < 20; i++)
            buff[i] = 0;
        m += 1;
        task_yield();
    }
}

void
test_sample_thread_handler()
{
    while (1) {
        task_yield();
        term_set_glyph(0, '0' + task_get_this_task_struct()->pid);
        sleep(90);
    }
}

void
test_exec()
{
    // test_vma_list_and_page_fault
    char* argv[] = { "test", "ok", "cool", "interesting", NULL };
    char* envp[] = { "PATH=blah", "PS2=$$", "PWD=/", NULL };
    syscall_wrapper(_SYS__execve, (long)"bin/init", (long)argv, (long)envp);
}

void
test_vfs_sample_functions()
{
    char buff1[100];
    char buff2[100];
    vfs_open("bin/ls", 0);
    vfs_open("/bin/cat", 0);
    vfs_open("bin/blah", 0);
    kprintf("Bytes Read: %d\n", vfs_read(3, buff1, 10));
    kprintf("%s\n", buff1);
    kprintf("Bytes Read: %d\n", vfs_read(3, buff1, 25));
    kprintf("%s\n", buff1);

    kprintf("Bytes Read: %d\n", vfs_read(4, buff2, 10));
    kprintf("%s\n", buff2);
    kprintf("Bytes Read: %d\n", vfs_read(4, buff2, 40));
    kprintf("%s\n", buff2);
    task_yield();
}

void
test_fork()
{
    uint64_t x = 44;
    x = fork();
    x = fork();
    x += 1;
    kprintf("My cr3 is %d", task_get_this_task_struct()->regs.cr3);
    while (1) {
        term_set_glyph(0, '0' + task_get_this_task_struct()->pid);
        task_yield();
    }
}
void
test_sched()
{
#if 0
    task_create(test_sample_thread_handler);
    task_create(test_sample_thread_handler);
    task_create(test_sample_thread_handler);
    task_create(test_sample_thread_handler);
    task_create(test_vfs_sample_functions);
    task_create(test_exec);
#endif
    task_struct* init_task = task_create(test_fork);
    init_task->regs.cr3 = paging_get_current_cr3();
    task_yield();
}

void
test_kprintf()
{
    for (int i = 0; i < 20; i++) {
        kprintf("\nTest\n");
        kprintf("more %d\n", i);
        kprintf("more %d\n", i);
    }
    kprintf("This is a sample text that is going to test my kprintf big time. "
            "Let's see if we can find any bugs. The hope is that there won't "
            "be any bugs and the code works as is without any problem. Now "
            "let's go catch'em all. We are the developers. We are the testers. "
            "One ring to rule them all. If you reading this much garbage, it "
            "is time to go work on OS");
}
