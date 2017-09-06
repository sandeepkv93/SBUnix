#include <stdlib.h>

void _start(void) {
  /*
	 __asm__(
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
		*/
	 exit( main(1,NULL, NULL));
 }
