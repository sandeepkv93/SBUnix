#include <sys/nary.h>

#ifndef _VFS_H
#define _VFS_H
typedef struct
{
    char file_name[100];
    int size;
    int cursor;
    struct fs_node_entry fs_node;
} file;

int vfs_open(const char* pathname, int flags);
int vfs_close(int fd);
int vfs_dup(int fd);
unsigned int vfs_read(int fd, void* buf, unsigned int count);
#endif