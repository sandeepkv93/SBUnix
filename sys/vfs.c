#include <sys/alloc.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/nary.h>
#include <sys/string.h>
#include <sys/tarfs.h>
#include <sys/task.h>
#include <sys/utility.h>
#include <sys/vfs.h>

int
find_free_fd()
{
    int i;
    for (i = 0; i < TASK_FILETABLE_SIZE; ++i) {
        if (task_get_this_task_struct()->filetable[i] == NULL) {
            return i;
        }
    }
    return TASK_FILETABLE_SIZE + 1;
}

int
vfs_open(char* pathname, int flags)
{
    char path[4096];
    if (pathname[0] == '/') {
        /*Relative Path */
        strcpy(path, pathname);
    } else {
        /*Relative Path */
        strcpy(path, task_get_this_task_struct()->cwd);
        strcat(path, pathname);
    }
    struct fs_node_entry* fs_node = findNaryNode(path);
    if (fs_node != NULL) {
        vfs_file_object* file_obj = kmalloc(sizeof(vfs_file_object));
        strcpy(file_obj->file_name, fs_node->node_id);
        file_obj->size = fs_node->size;
        file_obj->cursor = 0;
        file_obj->fs_node = *fs_node;
        file_obj->reference_count = 1;
        int fd = find_free_fd();
        task_get_this_task_struct()->filetable[fd] = file_obj;
        return fd;
    }
    return -1;
}

int
vfs_close(int fd)
{
    /*
    Check the file_table with fd as index.
    In the file object it points to, decrement the ref_count
    if ref_count = 0,
            free it
    Put Null at the index = fd;
    */
    return 0;
}

int
vfs_dup(int fd)
{
    /*
    Check for First Free Entry in the fdtable
    Put value in fd_table[fd] into that fd
    and return it.
    */
    return 0;
}

int
vfs_seek(int fd, uint64_t offset)
{
    vfs_file_object* file_obj = task_get_this_task_struct()->filetable[fd];
    if (file_obj == NULL) {
        return -1;
    } else if (offset > file_obj->size) {
        return -2;
    }
    file_obj->cursor = offset;
    return 0;
}

unsigned int
vfs_read(int fd, void* buffer, unsigned int count)
{
    /*
    Goto File object corresponding to fd;
    Goto the Nary Tree Node pointed by file object
    Check fs type.
            Call appropriate fs_read(Fobj.cursor, size)
            increment FObject.cursor by the return value of fs_read
    */
    vfs_file_object* file_obj = task_get_this_task_struct()->filetable[fd];
    if (file_obj != NULL) {
        switch (file_obj->fs_node.fs_type) {
            case TARFS_FILE_TYPE:
                /*
                kprintf("Read: %s\n", file_obj->fs_node.name);
                kprintf("Size: %d\n", file_obj->size);
                */
                kprintf("Starting to read.. Cursor at: %d\n", file_obj->cursor);
                if (count > file_obj->fs_node.size - file_obj->cursor) {
                    count = file_obj->fs_node.size - file_obj->cursor;
                }
                char* file_starting_address =
                  (char*)(file_obj->fs_node.struct_address +
                          sizeof(struct posix_header_ustar) + file_obj->cursor);
                int i = 0;
                char* reader = (char*)buffer;
                for (; i < count; ++i) {
                    reader[i] = *file_starting_address;
                    ++file_starting_address;
                }
                reader[i] = '\0';
                file_obj->cursor += i;
                kprintf("Finished reading.. Cursor at: %d\n", file_obj->cursor);
                return i;
            case NORMAL_FILE_TYPE:
                break;
            default:
                break;
        }
    }

    return -1;
}
