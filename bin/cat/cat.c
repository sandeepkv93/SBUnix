#include <fcntl.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

/*
int open_new(char * filename, int flags)
{
        return syscall(SYS_open,filename,flags);
}

int read_new(int fd,char * buf, int size)
{
        //return depends on the system call. The size read will be returned
unless there is an error. 0 on EOF.
        return syscall(SYS_read,fd,buf,size);
}
int write_new(int fd,char * buf, int size)
{
        return syscall(SYS_write,fd,buf,size);
}
*/

int BUF_SIZE = 255;
int
main(int argc, char** argv)
{
    int fd_read;
    char buf[255];
    int read_size;
    if (argc < 2) {
        puts("Usage: cat <file_name> ");
        exit(1);
    }
    if ((fd_read = open(argv[1], O_RDONLY)) < 0) {
        puts("Error opening the file");
    }
    while ((read_size = read(fd_read, buf, BUF_SIZE)) > 0) {
        write(1, buf, read_size);
    }
    close(fd_read);
    return 0;
}
