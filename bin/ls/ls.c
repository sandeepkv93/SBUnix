#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 1024
#define CONSOLE_BUF_SIZE 255
#define FORMATTING_VAR_SIZE 10
#define	DT_DIR 4
#define DT_LNK 10

struct new_dirent {
    long inode_num;
    off_t d_off;
    unsigned short d_reclen;
    char d_name[];
};

int string_write(int fd,char * console_buffer, int buffer_size)
{
    /* Calls the syscall with SYS_write*/
    return syscall(SYS_write,fd,console_buffer,buffer_size);
}

int main(int argc, char* argv[])
{
    int fd;
    int nread;
    char dirent_buffer[BUF_SIZE];
    struct new_dirent *new_dirent_structure;
    int directory_position;
    char *file_name;
    char console_buffer[CONSOLE_BUF_SIZE];
    char formatter[FORMATTING_VAR_SIZE];
    char *format_unset = "\e[0m\e[21m  ";
    char *directory_name = (argc>1)?argv[1]:".";
    char *new_line_buffer = "\n";
    char error_message[CONSOLE_BUF_SIZE];
    char d_type;

    /* Open the directory*/
    fd = open(directory_name, O_RDONLY | O_DIRECTORY);    
    while(1) {
        nread = syscall(SYS_getdents, fd, dirent_buffer, BUF_SIZE);
        if (nread == -1) {
            strcpy(error_message,"ls: cannot access \'");
            strcat(error_message,directory_name);
            strcat(error_message,"\': No such file or directory\n");
            string_write(1, error_message, strlen(error_message));
            _exit(-1);
        }

        /* read till nread becomes zero*/
        if (nread == 0)
            break;

        for (directory_position = 0; directory_position < nread;directory_position += new_dirent_structure->d_reclen) {
            new_dirent_structure = (struct new_dirent *) (dirent_buffer + directory_position);
            
            /* Get the file name from dirent structure */
            file_name = new_dirent_structure->d_name;
            
            if (strcmp(file_name, "..") && strcmp(file_name, ".")) {
		d_type = *(dirent_buffer + directory_position + new_dirent_structure->d_reclen - 1);	
		
                /* Formatting the filenames based on the type of files*/
                if (d_type == DT_DIR) {
                    strcpy(formatter,"\e[1m\e[34m");
                }
                else if ((d_type == DT_LNK)) {
                    strcpy(formatter, "\e[1m\e[94m");
                }
                else {
                    strcpy(formatter,"");
                }

                /* Write to the formatted string to the buffer */
                strcpy(console_buffer,formatter);
                strcat(console_buffer,file_name);
                strcat(console_buffer,format_unset);

                string_write(1,console_buffer,strlen(console_buffer));
            }            
        }
    }
    close(fd);
    string_write(1,new_line_buffer,strlen(new_line_buffer));
    return 0;
}
