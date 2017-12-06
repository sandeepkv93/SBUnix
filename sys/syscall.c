#include <sys/alloc.h>
#include <sys/defs.h>
#include <sys/fork.h>
#include <sys/kprintf.h>
#include <sys/paging.h>
#include <sys/signal.h>
#include <sys/syscall.h>
#include <sys/task.h>
#include <sys/tasklist.h>
#include <sys/term.h>
#include <sys/timer.h>
extern void syscall_isr_return(long);
extern bool is_context_switch_enabled;

long
syscall_kill(pid_t pid, int signal)
{
    return (long)signal_kill(pid, signal);
}

long
syscall_brk(void* new_brk)
{
    return (long)alloc_brk(new_brk);
}

long
syscall_yield()
{
    task_yield();
    return 0;
}

long
syscall_fork()
{
    return fork();
}

long
syscall_exit(uint64_t exit_code)
{
    tasklist_exit(exit_code);
    return -1;
}

long
syscall_wait(long child_pid, long status)
{

    if (child_pid < 0) {
        return tasklist_wait(status);
    } else {
        return tasklist_waitpid(child_pid);
    }
}

long
syscall_open(char* fname, int flags)
{
    return vfs_open(fname, flags);
}

long
syscall_exec(char* file_name, char** argv, char** envp)
{
    char* interpreter;
    char* script_argv[3] = { NULL, NULL, NULL };
    if (!vma_verfiy_elf(file_name)) {
        interpreter = vma_get_script_interpreter(file_name);
        if (interpreter == NULL) {
            return -1;
        }
        script_argv[0] = interpreter;
        script_argv[1] = file_name;
        /*
        int fd = vfs_open(file_name, 0);

        task_get_this_task_struct()->filetable[0] =
          task_get_this_task_struct()->filetable[fd];
        task_get_this_task_struct()->filetable[fd] = NULL;
        */
        task_exec_ring3(interpreter, script_argv, envp);
    } else {
        task_exec_ring3(file_name, argv, envp);
    }
    return -1;
}

long
syscall_read(uint64_t fd, char* buf, uint64_t count)
{
    long ret = -1;
    if (task_get_this_task_struct()->filetable[fd] ==
        (vfs_file_object*)TERM_READ_OBJ) {
        ret = term_read_from_buffer(buf, count);
    } else {
        ret = vfs_read(fd, buf, count);
    }

    return ret;
}

long
syscall_write(uint64_t fd, char* buff, uint64_t count)
{
    uint8_t row, column, color, temp;
    long ret = -1;
    if ((task_get_this_task_struct()->filetable[fd] ==
         (vfs_file_object*)TERM_WRITE_OBJ) ||
        (task_get_this_task_struct()->filetable[fd] ==
         (vfs_file_object*)TERM_ERROR_OBJ)) {
        if (STDERR == fd) {
            term_get_cursor(&row, &column, &color);
            term_set_cursor(row, column, term_color_red);
        }
        ret = term_write(buff, count);
        if (STDERR == fd) {
            term_get_cursor(&row, &column, &temp);
            term_set_cursor(row, column, color);
        }
    }
    return ret;
}

long
syscall_close(uint64_t fd)
{
    long ret = -1;
    if (task_get_this_task_struct()->filetable[fd] !=
        (vfs_file_object*)TERM_READ_OBJ) {
        ret = (long)vfs_close(fd);
    } else {
        ret = 0;
    }
    return ret;
}

long
syscall_chdir(char* path)
{
    return vfs_chdir(path);
}

long
syscall_getcwd(char* buf, size_t size)
{
    long ret = -1;
    ret = (long)vfs_getcwd(buf, size);
    return ret;
}

long
syscall_getpid()
{
    return (long)task_getpid();
}

long
syscall_getppid()
{
    return (long)task_getppid();
}

long
syscall_access(char* path)
{
    return (long)vfs_access(path);
}

long
syscall_dup(int fd)
{
    return (long)vfs_dup(fd);
}

long
syscall_unlink(char* path)
{
    return vfs_unlink(path);
}

long
syscall_sleep(uint32_t seconds)
{
    timer_nonblocking_sleep(seconds);
    return 0;
}

long
syscall_ps()
{
    tasklist_walk_print();
    return 0;
}

long
syscall_wrapper(long syscall_num, long arg1, long arg2, long arg3)
{
    long ret_val = -1;
    is_context_switch_enabled = FALSE;
    switch (syscall_num) {
        /* For each case implement syscall, update ret_val */
        case _SYS__write:
            ret_val =
              syscall_write((uint64_t)arg1, (char*)arg2, (uint64_t)arg3);
            break;
        case _SYS__read:
            ret_val = syscall_read((uint64_t)arg1, (char*)arg2, (uint64_t)arg3);
            break;
        case _SYS__execve:
            ret_val = syscall_exec((char*)arg1, (char**)arg2, (char**)arg3);
            break;
        case _SYS__open:
            ret_val = syscall_open((char*)arg1, (int)arg2);
            break;
        case _SYS__fork:
            ret_val = syscall_fork();
            break;
        case _SYS__sched_yield:
            syscall_yield();
            break;
        case _SYS__brk:
            // TODO
            // Create a vma in process during read_elf for heap. Grow that
            // vma
            // when
            // brk is called, use a member in vma_struct to identify the VMA
            // corresponding to heap
            ret_val = syscall_brk((void*)arg1);
            break;
        case _SYS__close:
            ret_val = syscall_close((int)arg1);
            break;
        case _SYS__chdir:
            ret_val = syscall_chdir((char*)arg1);
            break;
        case _SYS__getcwd:
            ret_val = syscall_getcwd((char*)arg1, (size_t)arg2);
            break;
        case _SYS__getpid:
            ret_val = syscall_getpid();
            break;
        case _SYS__getppid:
            ret_val = syscall_getppid();
            break;
        case _SYS__unlink:
            ret_val = syscall_unlink((char*)arg1);
            return ret_val;
        case _SYS__wait4:
            ret_val = syscall_wait(arg1, arg2);
            break;
        case _SYS__exit:
            ret_val = syscall_exit((uint64_t)arg1);
            break;
        case _SYS__acces:
            ret_val = syscall_access((char*)arg1);
            break;
        case _SYS__dup:
            ret_val = syscall_dup((int)arg1);
            break;
        case _SYS__nanosleep:
            ret_val = syscall_sleep((uint32_t)arg1);
            break;
        case _SYS__sched_getscheduler:
            ret_val = syscall_ps();
            break;
        case _SYS__kill:
            ret_val = syscall_kill((pid_t)arg1, (int)arg2);
            break;
        case _SYS__pipe:
        // TODO
        default:
            kprintf("Call num %d, args %d, %d, %d, ret_val %d", syscall_num,
                    arg1, arg2, arg3, ret_val);
    }
    /*task_yield();*/
    is_context_switch_enabled = TRUE;
    return ret_val;
}
