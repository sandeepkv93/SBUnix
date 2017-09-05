#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/syscall.h>

int open_new(char * filename, int flags)
{
        return syscall(SYS_open,filename,flags);
}

int read_new(int fd,char * buf, int size)
{
	//return depends on the system call. The size read will be returned unless there is an error. 0 on EOF.
	return syscall(SYS_read,fd,buf,size);
}
int write_new(int fd,char * buf, int size)
{
	return syscall(SYS_write,fd,buf,size);
}

int BUF_SIZE = 255;
int main(int argc, char ** argv)
{
        int fd_read;
        char buf[255];
        int read_size;
        int i;
        for(i=1;i<argc;i++)
        {
                if((fd_read = open_new(argv[i],O_RDONLY))==-1)
                        printf("Error opening the file %s for write",argv[1]);
                while((read_size = read_new(fd_read,buf,BUF_SIZE))>0)
                        write_new(1,buf,read_size);
        }
}