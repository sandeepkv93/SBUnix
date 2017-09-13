#include <stdio.h>
#include <stdlib.h>

void
_start(void)
{
    long __argc;
    char* __argv;
    __asm__("movq %%rsp, %%rdx;"
            "popq %%rsi;"
            "popq %%rsi;"
            "movq %%rsi, %0;"
            "movq %%rsp, %1;"
            "movq %%rdx, %%rsp"
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
