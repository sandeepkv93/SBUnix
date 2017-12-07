#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
char* command_list[20];
char exec_binary_name[64];
char pwd[10];
char* g_ps1 = "[ >> ] ";
bool g_mode_interative = TRUE;

int
check_for_command_validity(char* cmd)
{
    int cP = 0;
    if (cmd[cP] != '\0') {
        if (cmd[cP++] == ' ') {
            return -1;
        }
        while (cmd[cP] != '\0') {
            if (cmd[cP++] == ' ') {
                if (cmd[cP] == '\0' || cmd[cP++] == ' ') {
                    return -1;
                }
            }
        }
    }
    return 0;
}
char*
token_creater(char* src, int size)
{
    char* string = (char*)malloc(size + 1);
    strncpy(string, src, size);
    string[size] = '\0';
    return string;
}

bool
command_bg_handler(char* cmd)
{
    int i;
    for (i = 0; cmd[i]; i++) {
    }
    if (cmd[--i] == '&') {
        do {
            cmd[i--] = 0;
        } while (cmd[i] == ' ');
        return TRUE;
    }
    return FALSE;
}
void
command_tokenizer(char* cmd)
{
    int cP = 0;
    int sT = 0;
    int gP = 0;
    int size;
    while (cmd[cP] != '\0') {
        if (cmd[cP] == ' ') {
            size = cP - sT;
            command_list[gP++] = token_creater(&cmd[sT], size);
            sT = cP + 1;
        }
        cP++;
    }
    command_list[gP++] = token_creater(&cmd[sT], cP - sT);
    command_list[gP] = NULL;
}

void
setenv(char** envp, char* env_var, char* env_val)
{
    char** env;
    for (env = envp; *env != 0; env++) {
        if (strncmp(*env, env_var, strlen(env_var)) == 0) {
            strcpy(*env, env_var);
            strcat(*env, "=");
            strcat(*env, env_val);
            break;
        }
    }
    *env = malloc(strlen(env_var) + 1 /*For =*/ + strlen(env_val) + 1);
    strcpy(*env, env_var);
    strcat(*env, "=");
    strcat(*env, env_val);
}

void
getenv(char** envp, char* env_var, char** env_val)
{
    for (char** env = envp; *env != 0; env++) {
        if (strncmp(*env, env_var, strlen(env_var)) == 0) {
            *env_val = *(env) + strlen(env_var) + 1;
            return;
        }
    }
    *env_val = "";
}

void
print_ps1()
{
    if (g_mode_interative) {
        write(2, g_ps1, strlen(g_ps1));
    }
}

int
main(int argc, char* argv[], char* envp[])
{
    char input_line[400];
    pid_t pid;
    bool is_bg = FALSE;
    int input_fd = 0;
    if (argv[1] != NULL) {
        g_mode_interative = FALSE;
        if ((input_fd = open(argv[1], O_RDONLY)) == -1) {
            printf("Please check the script: %s", argv[1]);
            exit(1);
        }
    } else {
        puts("Starting sbush..");
    }
    print_ps1();
    while (fgets(input_fd, input_line)) {
        if (check_for_command_validity(input_line) == -1) {
            puts("Invalid Command. Please Try Again\n");
            print_ps1();
            continue;
        }
        if (input_line[0] == '#') {
            print_ps1();
            continue;
        }
        is_bg = command_bg_handler(input_line);
        command_tokenizer(input_line);
        if (strcmp(command_list[0], "exit") == 0) {
            puts("Exiting");
            puts("");
            exit(0);
        }
        if (strcmp(command_list[0], "pwd") == 0) {
            getcwd(pwd, 10);
            write(1, pwd, strlen(pwd));
            write(1, "\n", 1);
            print_ps1();
            continue;
        }
        if (strcmp(command_list[0], "cd") == 0) {
            if (command_list[1] == NULL) {
                puts("Usage: cd <path>");
            } else {
                if (chdir(command_list[1]) == -1) {
                    puts("Invalid Path. Please Try Again");
                }
            }
            print_ps1();
            continue;
        }
        if (strcmp(command_list[0], "getenv") == 0) {
            if (command_list[1] == NULL) {
                puts("Usage: getenv <env_var_name>");
            } else {
                char* e;
                getenv(envp, command_list[1], &e);
                write(1, e, strlen(e));
            }
            write(1, "\n", 1);
            print_ps1();
            continue;
        }
        if (strcmp(command_list[0], "setenv") == 0) {
            if (command_list[1] == NULL) {
                puts("Usage: setenv <env_var_name = env_var_value>");
            } else {
                char* equals = strchr(command_list[1], '=');
                if (equals != 0) {
                    int prefix_length =
                      (int)(strlen(command_list[1]) - strlen(equals));
                    char* prefix = malloc(prefix_length + 1);
                    strncpy(prefix, command_list[1], prefix_length);
                    prefix[prefix_length] = 0;
                    char* suffix = malloc(strlen(equals) + 1);
                    suffix = equals + 1;
                    setenv(envp, prefix, suffix);
                } else {
                    puts("Usage: setenv <env_var_name = env_var_value>");
                }
            }
            write(1, "\n", 1);
            print_ps1();
            continue;
        }
        pid = fork();
        if (pid == 0) {

            strcpy(exec_binary_name, command_list[0]);

            if (access(exec_binary_name, 0) == -1) {
                char* e;
                getenv(envp, "PATH", (char**)&e);
                strcpy(exec_binary_name, e);
                strcat(exec_binary_name, command_list[0]);
            }

            execvpe(exec_binary_name, command_list, envp);

            printf("Failed to run command, Please check again\n");
            exit(1);
        } else {
            if (!is_bg) {
                waitpid(pid, NULL); // dummy status
            }
        }
        print_ps1();
    }
}
