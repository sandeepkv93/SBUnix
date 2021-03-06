#define __DEBUG__ 1
#ifdef __DEBUG__
#define debug_print(x) puts(x);
#else
#define debug_print(...)
#endif

#define error_print(x) putstr(x);

#define LENARR(x) (sizeof(x) / sizeof(x[0]))

#define STR_EQUALS "="
#define STR_PIPE "|"
#define STR_BG "&"
#define STR_SPACE " "
#define STR_DEFAULT_PS1 "sbush> "

#define CHAR_EQUALS '='
#define CHAR_PIPE '|'
#define CHAR_BG '&'
#define CHAR_SPACE ' '
#define CHAR_COLON ':'

#define STDIN_FD 0
#define STDOUT_FD 1

#define MODE_INTERACTIVE 21
#define MODE_SCRIPT 22

enum cmd_t
{
    cmd_bin,
    cmd_script,
    cmd_bg,
    cmd_pipe,
    cmd_builtin
};

enum builtin_t
{
    builtin_none,
    builtin_cd,
    builtin_exit,
    builtin_export,
    builtin_comment
};

struct s_builtins
{
    enum builtin_t type;
    char* command;
};
