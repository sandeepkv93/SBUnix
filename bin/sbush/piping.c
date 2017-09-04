#define READ 0
#define WRITE 1

#define _GNU_SOURCE
#include<sys/syscall.h>
#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
int pipe_new(int fds[])
{
	return syscall(SYS_pipe,fds);
}

int dup_new(int fd)
{
	return syscall(SYS_dup,fd);
}

int close_new(int fd)
{
	return syscall(SYS_close,fd);
}
int wait_new(int pid)
{
	return  syscall(SYS_wait4,-1, NULL, 0, NULL);
}

#ifdef __TEST__
int main(int argc, char ** argv)
{
        int fds[2];
        int pid;
        int i=0;
	char * cmd_paths[10] = {"/bin/cat","/bin/grep","/usr/bin/wc"};
	char * cmds[10] = {"cat","grep","wc"};
	char * args[10] = {"input","abc","-w"};
	int pipe_num = 2;
	int read_end =-1;
	int  write_end = -1;
	if(pipe_new(fds)==-1)
	{       
		printf("Failed to create pipe.");
	}
                        
	if(pipe_num == 0)
	{
		return 0;
	}
	else
	{
		for(i=0;i<=pipe_num;i++)
		{
			if(read_end!=-1)
			{
				if(pipe_new(fds)==-1)
                        	{       
                                	printf("Failed to create pipe.");
                        	}
			}
			write_end = fds[1];
		        if((pid = fork()) == -1)
		                printf("error");
		        if(pid == 0)
        		{
                		printf("child %d",i);
                		fflush(stdout);
				if(i!=0)
				{
					close_new(READ);
	                		dup_new(read_end);
				}
				if(i!=pipe_num)
				{	close(WRITE);
					dup_new(write_end);
				}
                		execl(cmd_paths[i],cmds[i],args[i],(char *)0);
        		}
			else
			{
				wait_new(-1);
				close_new(write_end);
				read_end = fds[0];			
			}
		}
	}

}
#endif

/*        if(pipe_new(fds)==-1)
        {
                printf("error");
        }
        if((pid = fork()) == -1)
                printf("error");
        if(pid == 0)
        {
                //printf("child1");
                //fflush(stdout);
                close_new(1);
                dup_new(fds[1]);
                execl("/bin/cat","cat","input",(char *)0);
        }
        else
        {
                wait_new(-1);
                close_new(fds[1]);
                if((pid = fork())== -1)
                        printf("error");
                if(pid == 0)
                {
                        //printf("child2");
                        //fflush(stdout);
                        close(0);
                        dup_new(fds[0]);
                        execl("/bin/grep","grep","abc", (char *)0);
                }
                else
                {
                        wait_new(-1);
                }
	printf("parentend");
        }
}

*/
