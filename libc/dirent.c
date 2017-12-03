#include <dirent.h>
#include <stdlib.h>
#include <sys/string.h>
#include <unistd.h>

DIR*
opendir(const char* name)
{
    int fd = open(name, O_DIRECTORY);
    DIR* dir;
    if (fd != -1) {
        dir = (DIR*)malloc(sizeof(DIR));
        dir->fd = fd;
        return dir;
    }
    return NULL;
}

dirent*
readdir(DIR* dirp)
{
    dirent* dirent_object;
    char name_buffer[NAME_MAX + 1] = { 0 };
    int read_status = read(dirp->fd, name_buffer, sizeof(name_buffer));
    if (read_status != -1) {
        dirent_object = (dirent*)malloc(sizeof(dirent));
        strcpy(dirent_object->d_name, name_buffer);
        return dirent_object;
    }
    return NULL;
}

int
closedir(DIR* dirp)
{
    return close(dirp->fd);
}
