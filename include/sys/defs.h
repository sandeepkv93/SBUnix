#ifndef _DEFS_H
#define _DEFS_H

#define O_RDONLY 0x0000
#define O_DIRECTORY 00200000
#define NULL ((void*)0)
#define TRUE 1
#define FALSE 0
#define STDIN 0
#define STDOUT 1
#define STDERR 2

typedef unsigned long uint64_t;
typedef long int64_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned char uint8_t;
typedef char int8_t;

typedef uint64_t size_t;
typedef int64_t ssize_t;

typedef uint64_t off_t;

typedef uint32_t pid_t;
typedef uint32_t mode_t;
typedef uint32_t bool;

#endif
