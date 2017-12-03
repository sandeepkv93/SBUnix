#ifndef _UNISTD_H
#define _UNISTD_H

#include <sys/defs.h>

#define F_OK 0
#define X_OK 1

long syscall(long, long, long, long);
void exit(int);
int open(const char*, int);
pid_t fork(void);
pid_t waitpid(pid_t, int*, int);
int pipe(int[]);
int dup(int);
int close(int);
void* brk(void*);

ssize_t read(int fd, void* buf, size_t count);
ssize_t write(int fd, const void* buf, size_t count);
int unlink(const char* pathname);

int chdir(const char* path);
int getcwd(char* buf, size_t size);

int execvpe(const char* file, char* const argv[], char* const envp[]);
pid_t wait(int* status);

unsigned int sleep(uint32_t seconds);

pid_t getpid(void);
pid_t getppid(void);

// OPTIONAL: implement for ``on-disk r/w file system (+10 pts)''
off_t lseek(int fd, off_t offset, int whence);
int mkdir(const char* pathname, mode_t mode);

// OPTIONAL: implement for ``signals and pipes (+10 pts)''
int pipe(int pipefd[2]);

int access(const char* pathname, int mode);
void yield();

#endif
