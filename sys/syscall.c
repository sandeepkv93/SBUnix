#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/syscall.h>
#pragma GCC push_options
#pragma GCC optimize("O0")
extern void syscall_isr_return(long);
long
syscall_write(uint64_t fd, char* buff, uint64_t count)
{
    return -1;
}
long
syscall_wrapper(long syscall_num, long arg1, long arg2, long arg3)
{
    long ret_val = 69;
    switch (syscall_num) {
        case _SYS__open:
        /* For each case implement syscall, update ret_val */
        case _SYS__close:
        case _SYS__read:
        case _SYS__write:
            ret_val =
              syscall_write((uint64_t)arg1, (char*)arg2, (uint64_t)arg3);
        case _SYS__fork:
        case _SYS__execve:
        case _SYS__wait4:
        case _SYS__exit:
        case _SYS__chdir:
        case _SYS__acces:
        case _SYS__pipe:
        case _SYS__dup:
        case _SYS__brk:
        default:
            kprintf("Call num %d, args %d, %d, %d, ret_val %d", syscall_num,
                    arg1, arg2, arg3, ret_val);
    }
    return ret_val;
}
#pragma GCC pop_options
