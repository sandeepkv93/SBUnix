#ifndef _DEBUG_H
#define _DEBUG_H
#include <sys/kprintf.h>
void
debugDumpData(const void* p, int size)
{
    int i = 0;
    char* addr = (char*)p;
    for (i = 0; i < size; i++) {
        kprintf("%x", *addr);
        addr++;
    }
}
void
dummy()
{
}
#ifdef _DEBUG
#define debug_print kprintf
#else
#define debug_print dummy
#endif
#endif
