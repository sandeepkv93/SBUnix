#ifdef __DEBUG__
	#define debug_print(...)  printf(__VA_ARGS__);  
#else
	#define debug_print(...)
#endif

#define error_print(x) puts(x);  

#define LENARR(x) (sizeof(x)/sizeof(x[0]))

#define STR_EQUALS "="
#define STR_PIPE "|"
#define STR_BG   "&"
#define STR_SPACE " "
#define STR_DEFAULT_PS1 "\033[93msbush>\033[0m"

#define CHAR_EQUALS '='
#define CHAR_PIPE '|'
#define CHAR_BG   '&'
#define CHAR_SPACE ' '

#define STDIN_FD 0
#define STDOUT_FD 1

enum cmd_t {cmd_bin, cmd_script, cmd_bg, cmd_pipe, cmd_builtin };

enum builtin_t { builtin_none, builtin_cd, builtin_exit, builtin_export };

struct s_builtins {
	enum builtin_t type;
	char * command;
};
