#include <sys/nary.h>
#ifndef _VFS_H
#define _VFS_H
#define TARFS_FILE_TYPE 0
#define NORMAL_FILE_TYPE 1

typedef struct
{
    char file_name[100];
    int size;
    int cursor;
    int reference_count;
    struct fs_node_entry fs_node;
} vfs_file_object;

int vfs_open(char* pathname, int flags);
int vfs_close(int fd);
int vfs_dup(int fd);
unsigned int vfs_read(int fd, void* buffer, unsigned int count);
#endif