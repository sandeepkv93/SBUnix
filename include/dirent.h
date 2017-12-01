#ifndef _DIRENT_H
#define _DIRENT_H

#define NAME_MAX 255

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