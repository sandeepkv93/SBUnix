#ifndef _VFS_H
#define _VFS_H
typedef struct
{
    char file_name[100];
    int size;
    int cursor;
    struct fs_node_entry fs_node;
} file;
#endif