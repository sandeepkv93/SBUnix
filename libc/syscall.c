#include <syscall.h>
int syscall_( long sys_no,long a, long b, long c)
{
	/*
	 * sys_no -> rax
	 * arg1   -> rdi
	 * arg2   -> rsi
	 * arg4   -> rdx
	 */
	long x = 0;
	char * str = "ok working\n";
	asm(
			"movq	%1,%%rax;"
			"movq	%2,%%rdi;"
			"movq	%3,%%rsi;"
			"movq	%4,%%rdx;"
			"syscall;"
			"movq    %%rax,%0;"
			:"=r"(x)
			:"r"(sys_no),"r"(a),"r"(b),"r"(c)
			:"%rax","%rsi","%rdi","%rdx"
	   );
	return x;
}

void exit_ (int value) 
{
	syscall_(_SYS__exit,value,0,0);
}

int write_ (int fd, const void * buf, size_t count) 
{
	return syscall_(_SYS__write,(long)fd, (long)buf, (long) count);
}

int open_(const char * filename, int flags, mode_t mode) 
{
	return syscall_(_SYS__open,(long)filename, (long)flags, (long) mode);
}

int read_(int fd, void * buf, size_t count) 
{
	return syscall_(_SYS__read, (long)fd, (long)buf, (long) count);
}

pid_t fork_(void) 
{
	return syscall_(_SYS__fork, 0,0,0);	
}

int execvp_(const char * filename, char * const argv[], char * const envp[]) 
{
	return syscall_(_SYS__execve, (long)filename,(long)argv,(long)envp);	
}

pid_t waitpid_(pid_t pid, int * wstatus, int options) 
{
	return syscall_(_SYS__wait4, (long)pid, (long)wstatus, (long)options); 
}

int pipe_(int fds[])
{
	return syscall_(_SYS__pipe,(long)fds,0,0);
}

int dup_(int fd)
{
	return syscall_(_SYS__dup,(long)fd,0,0);
}

int close_(int fd)
{
	return syscall_(_SYS__close,(long)fd,0,0);
}
#ifdef __TEST__
void _start() 
{
	 asm(
        "xorl %ebp,%ebp;" //the outermost frame is marked by making ebp xero
        "popq %rsi;" // argc,argv,envp -> order in which they are stored on stack. This line loads argc into rsi
        "movq %rsp, %rdx;" //after the previous pop, esp is pointing to argv. move it to rdx
        "andq $~15, %rsp;" //make stack pointer point to an address that is a multiple of 16. this is for optimization
        "pushq %rsp;" //push back the aligned stack pointer
        "push %rdx;" // push argv
        "push %rsi;" // push argc
        "call main;"//call main
        "movq $60,%rax;" //exit
        "movq $0,%rdi;"
        "syscall;"
    );
}

int main(int argc, char**argv)
{
	pid_t pid;
	int ret;
	int fd;
	char buff[100];
	char * hello = "Nicee world\n";
	char * parent = "Parent here\n";
	char * child = "Child here\n";
	char arg_string[2];
	char * exec_args[] = {"/bin/ls", "-ltr",NULL};
	char * exec_env[] = {NULL};
	int read_count;
	arg_string[0] = '0' + argc;
	arg_string[1] = '\n';
	

	write_(1, hello,12);	
	/*
	write_(1, arg_string,2);	
	*/
	fd = open_("/home/rsoori/test.txt",0,0);
	read_(fd, buff, 10);
	write_(1, buff,10);	
	close_(fd);
	read_count = read_(fd, buff, 10);
	arg_string[0] = '9' + read_count;
	arg_string[1] = '\n';
	write_(1, arg_string, 1);

	
	if((pid=fork_())) {
		waitpid_(pid,NULL,0);
		write_(1, parent,12);	
	} else {
		write_(1, child,12);	
		execvp_(exec_args[0], exec_args, exec_env);
	}
	

	exit_(77);

	return 0;
}
#endif
