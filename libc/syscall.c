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

void exit_ (int value) {
	syscall_(_SYS__exit,value,0,0);
}

int write_ (int fd, const void * buf, size_t count) {
	return syscall_(_SYS__write,(long)fd, (long)buf, (long) count);
}

int open_(const char * filename, int flags, mode_t mode) {
	return syscall_(_SYS__open,(long)filename, (long)flags, (long) mode);
}

int read_(int fd, void * buf, size_t count) {
	return syscall_(_SYS__read, (long)fd, (long)buf, (long) count);
}

#ifdef __TEST__
void _start() {
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

int main(int argc, char**argv){
	int ret;
	int fd;
	char buff[100];
	char * hello = "Nicee world\n";
	char arg_string[2];
	arg_string[0] = '0' + argc;
	arg_string[1] = '\0';

	write_(1, hello,12);	
	write_(1, arg_string,1);	
	fd = open_(argv[1],0,0);
	read_(fd, buff, 10);
	write_(1, buff,10);	

	exit_(77);
	return 0;
}
#endif
