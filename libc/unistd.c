#include <sys/defs.h>
#include <sys/syscall.h>
#include <unistd.h>
long
syscall(long sys_no, long arg1, long arg2, long arg3)
{
    /*
     * Note we deviate from linux behavior for convenience.
     * By aligning syscall convention to the x86_64 calling conventions let's
     * avoid assembly code required to save registers and move values. Win(?)
     *
     * sys_no : Our OS -> rdi : Linux -> rax
     * arg1   : Our OS -> rsi : Linux -> rdi
     * arg2   : Our OS -> rdx : Linux -> rsi
     * arg3   : Our OS -> rcx : Linux -> rdx
     *
     * return : Our OS -> rax : Linux -> rax
     */

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

    /* We'll keep the below code commented to test code on linux
    __asm__("movq	%1,%%rax;"
            "movq	%2,%%rdi;"
            "movq	%3,%%rsi;"
            "movq	%4,%%rdx;"
            "syscall;"
            "movq    %%rax,%0;"
            : "=r"(x)
            : "r"(sys_no), "r"(arg1), "r"(arg2), "r"(arg3)
            : "%rax", "%rsi", "%rdi", "%rdx");
    */
    return x;
}

void
exit(int value)
{
    //    while (1)
    //        yield();
    syscall(_SYS__exit, value, 0, 0);
}

ssize_t
write(int fd, const void* buf, size_t count)
{
    return syscall(_SYS__write, (long)fd, (long)buf, (long)count);
}

int
open(const char* filename, int flags)
{
    return syscall(_SYS__open, (long)filename, (long)flags, (long)0);
}

ssize_t
read(int fd, void* buf, size_t count)
{
    return syscall(_SYS__read, (long)fd, (long)buf, (long)count);
}

pid_t
fork(void)
{
    return syscall(_SYS__fork, 0, 0, 0);
}

int
execvpe(const char* filename, char* const argv[], char* const envp[])
{
    return syscall(_SYS__execve, (long)filename, (long)argv, (long)envp);
}

pid_t
waitpid(pid_t pid, int* wstatus, int options)
{
    return syscall(_SYS__wait4, (long)pid, (long)wstatus, (long)options);
}

pid_t
wait(int* wstatus)
{
    return syscall(_SYS__wait4, (long)wstatus, 0, 0);
}

int
pipe(int fds[])
{
    return syscall(_SYS__pipe, (long)fds, 0, 0);
}

int
access(const char* pathname, int mode)
{
    return syscall(_SYS__acces, (long)pathname, (long)mode, 0);
}

int
dup(int fd)
{
    return syscall(_SYS__dup, (long)fd, 0, 0);
}

int
close(int fd)
{
    return syscall(_SYS__close, (long)fd, 0, 0);
}

void*
brk(void* addr)
{
    return (void*)syscall(_SYS__brk, (long)addr, 0, 0);
}

int
chdir(const char* path)
{
    return syscall(_SYS__chdir, (long)path, 0, 0);
}

void
yield()
{
    syscall(_SYS__sched_yield, 0, 0, 0);
}

int
getcwd(char* buf, size_t size)
{
    return syscall(_SYS__getcwd, (long)buf, (long)size, 0);
}

pid_t
getpid(void)
{
    return syscall(_SYS__getpid, 0, 0, 0);
}

pid_t
getppid(void)
{
    return syscall(_SYS__getppid, 0, 0, 0);
}

int
unlink(const char* pathname)
{
    return syscall(_SYS__unlink, (long)pathname, 0, 0);
}

unsigned int
sleep(unsigned int seconds)
{
    return syscall(_SYS__nanosleep, (long)seconds, 0, 0);
}

#ifdef __TEST__
void
_start()
{
    asm("xorl %ebp,%ebp;" // the outermost frame is marked by making ebp xero
        "popq %rsi;"      // argc,argv,envp -> order in which they are stored on
                          // stack. This line loads argc into rsi
        "movq %rsp, %rdx;" // after the previous pop, esp is pointing to argv.
                           // move it to rdx
        "andq $~15, %rsp;" // make stack pointer point to an address that is a
                           // multiple of 16. this is for optimization
        "pushq %rsp;"      // push back the aligned stack pointer
        "push %rdx;"       // push argv
        "push %rsi;"       // push argc
        "call main;"       // call main
        "movq $60,%rax;"   // exit
        "movq $0,%rdi;"
        "syscall;");
}

int
main(int argc, char** argv)
{
    pid_t pid;
    int ret;
    int fd;
    char buff[100];
    char* hello = "Nicee world\n";
    char* parent = "Parent here\n";
    char* child = "Child here\n";
    char arg_string[2];
    char* exec_args[] = { "/bin/ls", "-ltr", NULL };
    char* exec_env[] = { NULL };
    int read_count;
    arg_string[0] = '0' + argc;
    arg_string[1] = '\n';

    write_(1, hello, 12);
    /*
    write_(1, arg_string,2);
    */
    fd = open_("/home/rsoori/test.txt", 0, 0);
    read_(fd, buff, 10);
    write_(1, buff, 10);
    close_(fd);
    read_count = read_(fd, buff, 10);
    arg_string[0] = '9' + read_count;
    arg_string[1] = '\n';
    write_(1, arg_string, 1);

    if ((pid = fork_())) {
        waitpid_(pid, NULL, 0);
        write_(1, parent, 12);
    } else {
        write_(1, child, 12);
        execvp_(exec_args[0], exec_args, exec_env);
    }

    exit_(77);

    return 0;
}
#endif
