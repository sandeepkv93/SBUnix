#ifndef _DEBUG_H
#define _DEBUG_H

#include <sys/kprintf.h>
void debugDumpData(const void* p, int size);
void dummy();

#ifdef _DEBUG
#define debug_print kprintf
#else
#define debug_print dummy
#endif
#endif
