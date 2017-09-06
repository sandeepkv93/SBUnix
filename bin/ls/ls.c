#include <unistd.h>
#include <sys/syscall.h>
#include <string.h>
//#include <sys/stat.h>
#include <fcntl.h>

#define BUF_SIZE 1024
#define CONSOLE_BUF_SIZE 255
#define FORMATTING_VAR_SIZE 10

struct new_dirent {
    long inode_num;
    off_t d_off;
    unsigned short d_reclen;
    char d_name[];
};

int main(int argc, char* argv[])
{
    //struct stat filestats;
    int fd;
    int nread;
    char dirent_buffer[BUF_SIZE];
    struct new_dirent *new_dirent_structure;
    int directory_position;
    char *file_name;    
    char console_buffer[CONSOLE_BUF_SIZE];
    //char stat_buf[CONSOLE_BUF_SIZE];
    //char formatter[FORMATTING_VAR_SIZE];
//    char *format_unset = "\e[0m\e[21m  ";
    char *directory_name = (argc>1)?argv[1]:".";
    char *new_line_buffer = "\n";
    char error_message[CONSOLE_BUF_SIZE];

    /* Open the directory*/
    fd = open(directory_name, O_RDONLY | O_DIRECTORY,0);
    while(1) {
        nread = syscall(_SYS__getdents, fd,(long) dirent_buffer, BUF_SIZE);
        if (nread == -1) {
            strcpy(error_message,"ls: cannot access \'");
            strcat(error_message,directory_name);
            strcat(error_message,"\': No such file or directory\n");
            write(1, error_message, strlen(error_message));
            exit(-1);
        }

        /* read till nread becomes zero*/
        if (nread == 0)
            break;

        for (directory_position = 0; directory_position < nread;directory_position += new_dirent_structure->d_reclen) {
            new_dirent_structure = (struct new_dirent *) (dirent_buffer + directory_position);
            
            /* Get the file name from dirent structure */
            file_name = new_dirent_structure->d_name;
            
            if (strcmp(file_name, "..") && strcmp(file_name, ".")) {

                /* Write directory_name and file_name in the form, directory_name/file_name */
#if 0
                strcpy(stat_buf,directory_name);
                strcat(stat_buf,"/");
                strcat(stat_buf,file_name);
                
                /* Fetch the file stat into filestats struct via stat system call*/
                stat(stat_buf, &filestats);

                /* Formatting the filenames based on the type of files*/
                if (S_ISDIR(filestats.st_mode)) {
                    strcpy(formatter,"\e[1m\e[34m");
                }
                else if (S_ISLNK(filestats.st_mode)) {
                    strcpy(formatter, "\e[1m\e[94m");
                }
                else if(filestats.st_mode & S_IXUSR) {
                    strcpy(formatter, "\e[1m\e[32m");
                }
                else {
                    strcpy(formatter,"");
                }
#endif

                /* Write to the formatted string to the buffer */
                //strcpy(console_buffer,formatter);
                strcat(console_buffer,file_name);
                //strcat(console_buffer,format_unset);

                write(1,console_buffer,strlen(console_buffer));
            }            
        }
    }
    close(fd);
    write(1,new_line_buffer,strlen(new_line_buffer));
    return 0;
}
