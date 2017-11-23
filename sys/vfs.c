#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/nary.h>
#include <sys/string.h>
#include <sys/utility.h>
#include <sys/vfs.h>

int
vfs_open(const char* pathname, int flags)
{
    struct fs_node_entry* fs_node = findNaryNode("bin/ls");
    file file_obj;
    strcpy(file_obj.file_name, fs_node->node_id);
    file_obj.size = char_array_to_int(fs_node->size);
    file_obj.cursor = 0;
    file_obj.fs_node = *fs_node;
    kprintf("File Object Name: %s\n", file_obj.file_name);
    return 0;
}

int
vfs_close(int fd)
{
    return 0;
}

int
vfs_dup(int fd)
{
    return 0;
}

unsigned int
vfs_read(int fd, void* buf, unsigned int count)
{
    return 0;
}