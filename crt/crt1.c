#include <stdlib.h>
#include <stdio.h>

long __argc;
char * __argv;
char a[2];
void fix_my_stack(void) {
	// This fixes the stack! lol
}
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
	 fix_my_stack();
	 exit( main(__argc,(char**)__argv, (char**)((char**)__argv + __argc + 1)));
 }
