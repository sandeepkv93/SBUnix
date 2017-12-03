#ifndef _DIRENT_H
#define _DIRENT_H

#define NAME_MAX 255
/* TODO: Remove it once merged with master and have access to this macro */
#ifndef O_DIRECTORY
#define O_DIRECTORY 00200000 /* must be a directory */
#endif
typedef struct
{
    char d_name[NAME_MAX + 1];
} dirent;

typedef struct
{
    int fd;
} DIR;

DIR* opendir(const char* name);
dirent* readdir(DIR* dirp);
int closedir(DIR* dirp);

#endif