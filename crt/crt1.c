#include <stdlib.h>
#include <stdio.h>

long __argc;
char * __argv;
char a[2];
void _start(void) {
	 __asm__(
			 "popq %%rsi;"
			 "popq %%rsi;"
			 "popq %%rsi;"
			 "popq %%rsi;"
			 "movq %%rsi, %0;"
			 "movq %%rsp, %1;"
			 :"=r"(__argc),"=r"(__argv)
			 :
			 : "%rsi", "%rsp"
		);
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
	 puts("argc>");
	 a[0] = '0' + __argc;
	 a[1] = '\0';
	 puts(a);
	 puts(__argv);
	 exit( main(__argc,(char**)__argv, (char**)((char**)__argv + __argc + 1)));
 }
