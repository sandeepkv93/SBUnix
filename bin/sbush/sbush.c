#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
char* command_list[20];
char exec_binary_name[64];
char pwd[10];
char* g_ps1 = "[ >> ] ";
#if 0
#include "sbush.h"
#include "stringlib.h"
#include "stringll.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

char* environ[500];

/* sbush shell */

int
is_bg_process(char* line)
{
    return (lib_str_find(line, STR_BG) > 0);
}

int
is_piped(char* line)
{
    return (lib_str_find(line, STR_PIPE) > 0);
}

enum builtin_t
get_shell_builtin(char* line)
{
    int i;
    struct stringllnode* cmd_list = NULL;
    struct s_builtins builtins_db[] = { { builtin_cd, "cd" },
                                        { builtin_exit, "exit" },
                                        { builtin_export, "export" } };

    lib_str_split(line, CHAR_SPACE, &cmd_list);
    if (!cmd_list) {
        return builtin_none;
    }
    if (cmd_list->data[0] == '#') {
        return builtin_comment;
    }
    for (i = 0; i < LENARR(builtins_db); i++) {
        if (!strcmp(cmd_list->data, builtins_db[i].command)) {
            free_list(cmd_list);
            return builtins_db[i].type;
        }
    }
    free_list(cmd_list);
    return builtin_none;
}

void
get_env(char* key, char* value)
{
    /*
     * Searches for key in environ variable
     * and sets key=value aptly
     */
    int i = 0;
    char buff[100];

    strcpy(buff, key);
    strcat(buff, STR_EQUALS);

    while (environ[i] != NULL) {
        if (!lib_str_find(environ[i], buff)) {
            if (lib_str_split_get_member(environ[i], CHAR_EQUALS, 1, value)) {
                strcpy(value, STR_DEFAULT_PS1);
            }
            return;
        }
        i++;
    }
    if (!strcmp(key, "PS1")) {
        strcpy(value, STR_DEFAULT_PS1);
        return;
    }
    *value = '\0';
}

void
set_env(char* key, char* value)
{
    /*
     * Searches for key in environ variable
     * and sets key=value aptly
     */
    int i = 0;
    char buff[500];

    strcpy(buff, key);
    strcat(buff, STR_EQUALS);

    while (environ[i] != NULL) {
        if (!lib_str_find(environ[i], buff)) {
            strcpy(environ[i], buff);
            strcat(environ[i], value);
            return;
        }
        i++;
    }

    environ[i] = malloc(strlen(buff) + strlen(value) + 2);
    strcpy(environ[i], buff);
    strcat(environ[i], value);
    environ[i + 1] = NULL;
}

int
is_dir(char* filename)
{
    int fd = open(filename, O_DIRECTORY);
    if (fd > 0) {
        close(fd);
        return 1;
    }
    return 0;
}

int
get_executable_path(char* file)
{
    char path[500];
    struct stringllnode* path_list = NULL;

    if (file[0] == '/') {
        return 1;
    }

    get_env("PATH", path);
    lib_str_split(path, CHAR_COLON, &path_list);
    do {
        strcat(path_list->data, "/");
        strcat(path_list->data, file);
        if (access(path_list->data, F_OK) < 0) {
            path_list = path_list->next_node;
            continue;
        }
        if (is_dir(path_list->data)) {
            path_list = path_list->next_node;
            continue;
        }

        strcpy(file, path_list->data);
        return 1;
    } while (path_list != NULL);
    if (access(file, F_OK) >= 0) {
        return 1;
    }
    return 0;
}
int
handle_export(char* line)
{
    /*
     * Handles statements of the form:
     * 		export key=value
     */
    char export_statement[100];
    char env_key[100];
    char env_value[100];

    if (lib_str_split_get_member(line, CHAR_SPACE, 1, export_statement) ||
        lib_str_split_get_member(export_statement, CHAR_EQUALS, 0, env_key) ||
        lib_str_split_get_member(export_statement, CHAR_EQUALS, 1, env_value)) {
        error_print("Usage export <key>=<value>\n");
    }

    debug_print("Setting ");
    debug_print(env_key);
    debug_print(env_value);
    set_env(env_key, env_value);
    return 0;
}

int
handle_cd(char* line)
{
    char buff[100];
    if (lib_str_split_get_member(line, CHAR_SPACE, 1, buff)) {
        // set_pwd($HOME);
        error_print("Usage cd <Path>\n");
    } else {
        // Path is given
        if (chdir(buff)) {
            debug_print("Failed to chdir()");
            puts("Failed to cd. Please check the path");
        }
    }
    return 0;
}

enum cmd_t
get_command_type(char* input_line)
{

    if (get_shell_builtin(input_line) != builtin_none) {
        return cmd_builtin;
    } else if (is_bg_process(input_line)) {
        return cmd_bg;
    } else if (is_piped(input_line)) {
        return cmd_pipe;
    }
    return cmd_bin;
}

int
get_bg_command(char* input_line)
{
    struct stringllnode* cmd_curr = NULL;
    lib_str_split(input_line, CHAR_BG, &cmd_curr);
    strcpy(input_line, cmd_curr->data);
    free_list(cmd_curr);
    return 0;
}

int
get_arglist(char* input_line, char** arg_list)
{
    /*
     * Given an input such as:
     * echo hello world
     *
     * returns ["echo","hello","world"]
     */
    struct stringllnode* cmd_head = NULL;
    struct stringllnode* cmd_curr = NULL;
    int i = 0;

    lib_str_split(input_line, CHAR_SPACE, &cmd_curr);
    cmd_head = cmd_curr;
    while (cmd_curr) {
        arg_list[i] = cmd_curr->data;
        cmd_curr = cmd_curr->next_node;
        i++;
    }
    free_list(cmd_head);
    arg_list[i] = NULL;
    return 0;
}

int
run_script(char** argv, int argc)
{
    char* script_args[100];
    int i;
    for (i = 1; i < argc; i++) {
        script_args[i - 1] = argv[i];
    }
    script_args[i] = NULL;
    debug_print("Going to exec");
    execvpe(script_args[0], script_args, environ);
    error_print("Failed to run script! Please check the script\n");
    exit(1);
    return 0;
}

int
run_cmd(char* input_line, enum cmd_t command_type)
{
    /* Given a link list head with commands as the data,
     * runs each command and pipes them onto the next command.
     * If no pipe is found executes command.
     */

    struct stringllnode *cmd_curr = NULL, *cmd_head;
    char* arglist[100];
    int fds[2];
    int read_end = -1;
    int write_end = -1;
    int waste;
    pid_t pid;
    // Split on | and read the commands into a link list
    lib_str_split(input_line, CHAR_PIPE, &cmd_curr);
    cmd_head = cmd_curr;

    while (cmd_curr) {
        if (pipe(fds) == -1) {
            debug_print("Failed to create pipe.");
        }
        write_end = fds[1];
        if (!(pid = fork())) {
            if (cmd_head != cmd_curr) {
                // Not first command, read from pipe
                close(STDIN_FD);
                waste = dup(read_end);
            }
            if (cmd_curr->next_node) {
                // Not last command, write to pipe
                close(STDOUT_FD);
                waste = dup(write_end);
            }
            get_arglist(cmd_curr->data, arglist);

            if (!get_executable_path(arglist[0])) {
                error_print("!!Executable not found in PATH\n");
                exit(1);
            }
            debug_print("Execing: ") debug_print(cmd_curr->data);
            execvpe(arglist[0], arglist, environ);
            error_print("Unknown command : ");
            error_print(arglist[0]);
            error_print("\n");
            exit(1);
        } else {
            if (command_type != cmd_bg) {
                debug_print("waiting");
                waitpid(pid, NULL);
            }
            close(write_end);
            if (read_end != -1) {
                close(read_end);
            }
            read_end = fds[0];
        }
        cmd_curr = cmd_curr->next_node;
        waste++;
    }
    free_list(cmd_curr);
    return 0;
}

void
print_ps1()
{
    char ps1[300];
    get_env("PS1", ps1);
    putstr(ps1);
}

int
is_whitespace(char a)
{
    if (a == ' ' || a == '\t' || a == '\n' || a == '\0') {
        return 1;
    }
    return 0;
}

int
is_empty_str(char* s)
{
    int i;
    for (i = 0; i < strlen(s); ++i) {
        if (!is_whitespace(s[i])) {
            return 0;
        }
    }
    return 1;
}

void
read_envlist(char** envp)
{
    int i = 0;
    while (envp[i] != NULL) {
        debug_print(".");
        environ[i] = (char*)malloc(strlen(envp[i]) + 1);
        debug_print("0");
        debug_print(envp[i]);
        strcpy(environ[i], envp[i]);
        debug_print("1");
        i++;
    }
    environ[i] = NULL;
}

int
main(int argc, char* argv[], char* envp[])
{
    puts("[ SBUSH starting ]");
    int mode = MODE_INTERACTIVE;
    char input_line[1000];
    enum cmd_t command_type;
    int input_fd = 0;

    read_envlist(envp);

    if (argc > 1) {
        mode = MODE_SCRIPT;
        if ((input_fd = open(argv[1], O_RDONLY)) == -1) {
            error_print("Unable to read the script! : ");
            error_print(argv[0]);
            error_print("\n");
        }
    } else {
        print_ps1();
    }

    while (fgets(input_fd, input_line)) {
        if (is_empty_str(input_line)) {
            // print PS1
            if (mode == MODE_INTERACTIVE) {
                print_ps1();
            }
            continue;
        }
        /*
         * Maintain PATH
         * Read the args and run script, else:
         * 1. Check for built in commands
         * 2. Check for |, split into commands
         * 3. Check for &, set BG
         * 4. Fork and exec binary. check BG
         */
        command_type = get_command_type(input_line);
        switch (command_type) {
            case cmd_builtin:
                switch (get_shell_builtin(input_line)) {
                    case builtin_exit:
                        exit(0);
                    case builtin_cd:
                        debug_print("Trying to cd..\n");
                        handle_cd(input_line);
                        break;
                    case builtin_export:
                        handle_export(input_line);
                        break;
                    case builtin_none:
                        break;
                    case builtin_comment:
                        break;
                }

                break;
            case cmd_bg:
                get_bg_command(input_line);
                debug_print("BG process. ");
                debug_print("Filtered to ") debug_print(input_line);
                debug_print("\n") case cmd_pipe
                  : case cmd_script : case cmd_bin
                                      : run_cmd(input_line, command_type);
        }
        if (mode == MODE_INTERACTIVE) {
            print_ps1();
        }
    }
    return 0;
}
#endif

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
    write(2, g_ps1, strlen(g_ps1));
}

int
main(int argc, char* argv[], char* envp[])
{
    char input_line[400];
    pid_t pid;
    int dummy_status = 1;
    print_ps1();
    while (fgets(0, input_line)) {
        if (check_for_command_validity(input_line) == -1) {
            puts("Invalid Command. Please Try Again");
            print_ps1();
            continue;
        }
        command_tokenizer(input_line);
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

            puts("Failed to run command, please check again");
            exit(1);
        } else {
            wait(&dummy_status); // dummy status
        }
        print_ps1();
    }
}
