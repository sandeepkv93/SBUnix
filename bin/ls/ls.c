#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

int
main(int argc, char** argv)
{
    char buf[64] = { 0 };
    struct dirent* dirent_object;
    DIR* dir;
    if (argc > 1) {
        strcpy(buf, argv[1]);
    } else {
        getcwd(buf, 10);
    }
    dir = opendir(buf);
    if (dir == NULL) {
        puts("Please give a valid directory\n");
        exit(1);
    }
    if (dir->fd != -1) {
        /*
        buf[0] = dir->fd + '0';
        write(1, buf, 1);
        */
        while (1) {
            dirent_object = readdir(dir);
            if (dirent_object == NULL) {
                break;
            }
            puts(dirent_object->d_name);
        }
    }
    closedir(dir);
}
