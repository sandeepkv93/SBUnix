#include <sys/alloc.h>
#include <sys/defs.h>
#include <sys/kprintf.h>
#include <sys/nary.h>
#include <sys/string.h>
#include <sys/tarfs.h>
#include <sys/task.h>
#include <sys/utility.h>
#include <sys/vfs.h>
// BAD Declaration
#define O_DIRECTORY 00200000
char g_path_open[64];
char g_path_access[64];
char g_path_unlink[64];
char g_path_directory[512];
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
    char* path = g_path_open;
    if (pathname[0] == '/') {
        /*Relative Path */
        strcpy(path, pathname);
    } else {
        /*Relative Path */
        strcpy(path, task_get_this_task_struct()->cwd);
        strcat(path, pathname);
    }
    struct fs_node_entry* fs_node = findNaryNodeData(path);
    if (fs_node != NULL) {
        if (flags == O_DIRECTORY && fs_node->typeflag[0] != DIRECTORY + '0') {
            return -1;
        }
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
    vfs_file_object* file_obj = task_get_this_task_struct()->filetable[fd];
    if (file_obj != NULL) {
        file_obj->reference_count -= 1;
        if (file_obj->reference_count == 0) {
            /*kfree(file_obj);*/
        }
        task_get_this_task_struct()->filetable[fd] = NULL;
        return 0;
    }
    return -1;
}

int
vfs_access(const char* pathname)
{
    char* path = g_path_access;
    if (pathname[0] == '/') {
        strcpy(path, pathname);
    } else {
        strcpy(path, task_get_this_task_struct()->cwd);
        strcat(path, pathname);
    }
    return checkIfExists(path);
}

int
vfs_dup(int fd)
{
    /*
    Check for First Free Entry in the fdtable
    Put value in fd_table[fd] into that fd
    and return it.
    */
    vfs_file_object* file_obj = task_get_this_task_struct()->filetable[fd];
    if (file_obj != NULL) {
        int new_fd = find_free_fd();
        task_get_this_task_struct()->filetable[new_fd] = file_obj;
    }
    return -1;
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
    char* directory_path = g_path_directory;
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
                if (file_obj->fs_node.typeflag[0] == FILE_TYPE + '0') {
                    /*kprintf("Starting to read.. Cursor at: %d\n",
                     * file_obj->cursor);*/
                    if (count > file_obj->fs_node.size - file_obj->cursor) {
                        count = file_obj->fs_node.size - file_obj->cursor;
                    }
                    char* file_starting_address =
                      (char*)(file_obj->fs_node.struct_address +
                              sizeof(struct posix_header_ustar) +
                              file_obj->cursor);
                    int i = 0;
                    char* reader = (char*)buffer;
                    for (; i < count; ++i) {
                        reader[i] = *file_starting_address;
                        ++file_starting_address;
                    }
                    file_obj->cursor += i;
                    /* kprintf("Finished reading.. Cursor at:
                     * %d\n",file_obj->cursor); */
                    return i;
                } else if (file_obj->fs_node.typeflag[0] == DIRECTORY + '0') {
                    file_obj->cursor += 1;
                    if (file_obj->fs_node.name[0] != '/') {
                        strcpy(directory_path, "/");
                        strcat(directory_path, file_obj->fs_node.name);
                    }
                    struct nary_tree_node* nth_nary_node = findNthChild(
                      findNaryNode(directory_path), file_obj->cursor);
                    if (nth_nary_node == NULL) {
                        file_obj->cursor -= 1;
                        return -1;
                    }
                    strcpy(buffer, (nth_nary_node->data).name);
                    return strlen((nth_nary_node->data).name);
                }

            case NORMAL_FILE_TYPE:
                break;
            default:
                break;
        }
    }
    return -1;
}

int
vfs_chdir(const char* path)
{
    if (checkIfExists((char*)path) == 0) {
        strcpy(task_get_this_task_struct()->cwd, path);
        task_get_this_task_struct()->cwd[strlen(path)] = '\0';
        return 0;
    }
    return -1;
}

int
vfs_getcwd(char* buf, size_t size)
{
    char* cur_work_dir = task_get_this_task_struct()->cwd;
    if (strlen(cur_work_dir) < size) {
        strcpy(buf, task_get_this_task_struct()->cwd);
        return 0;
    }
    return -1;
}

int
vfs_unlink(const char* pathname)
{
    char* path = g_path_unlink;
    if (pathname[0] == '/') {
        /*Relative Path */
        strcpy(path, pathname);
    } else {
        /*Relative Path */
        strcpy(path, task_get_this_task_struct()->cwd);
        strcat(path, pathname);
    }
    return delete_nary_node(path);
}
