#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

int BUF_SIZE = 255;
int FORMATTING_VAR_SIZE = 10;

int string_write(int fd,char * console_buffer, int buffer_size)
{
    /* Calls the syscall with SYS_write*/
    return syscall(SYS_write,fd,console_buffer,buffer_size);
}

int main(int argc, char* argv[])
{
    struct dirent *dp;
    struct stat filestats;
    DIR *directory;
    char *file_name;    
    char console_buffer[BUF_SIZE];
    char stat_buf[BUF_SIZE];
    char formatter[FORMATTING_VAR_SIZE];
    char *format_unset = "\e[0m\e[21m  ";
    char *directory_name = (argc>1)?argv[1]:".";
    char *new_line_buffer = "\n";
    directory = opendir(directory_name);
    while ((dp=readdir(directory)) != NULL) {
        if (strcmp(dp->d_name, "..") && strcmp(dp->d_name, ".")) {
            file_name = dp->d_name;

            /* Write directory_name and file_name in the form, directory_name/file_name */
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

            /* Write to the formatted string to the buffer */
            strcpy(console_buffer,formatter);
            strcat(console_buffer,file_name);
            strcat(console_buffer,format_unset);

            string_write(1,console_buffer,strlen(console_buffer));
        }
    }
    closedir(directory);
    string_write(1,new_line_buffer,strlen(new_line_buffer));
    return 0;
}
