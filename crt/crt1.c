#include <stdio.h>
#include <stdlib.h>

long __argc;
char* __argv;
void
_start(void)
{
    __asm__("popq %%rsi;"
            "popq %%rsi;"
            "movq %%rsi, %0;"
            "movq %%rsp, %1;"
            : "=r"(__argc), "=r"(__argv)
            :
            : "%rsi", "%rsp", "%rdx");
    exit(main(__argc, (char**)__argv, (char**)((char**)__argv + __argc + 1)));
}

/*

------------------------- Higher MEM
|.          |
|-----------|
|NULL       |
|---        |
|.          |
|---        |
|.          |
|---        |
|envp 1     |
|---        |
|envp 0     |
|-----------|
|NULL       |
|----       |
|argv n-1   |
|---        |  |
|.          |  | Stack grows this way
|---        |  V
|.          |
|---        |
|argv 3     |
|---        |
|argv 1     |
|---        |
|argv 0     |
|-----------|
|argc = n   |
|---<<<<<<<<<<<<<< RSP --- Lower MEM

*/
