#define __DEBUG__ 1

#ifdef __DEBUG__  
	#define debug_print(...)  printf(__VA_ARGS__);  
#else
	#define debug_print(...)
#endif

#define LENARR(x) (sizeof(x)/sizeof(x[0]))

enum cmd_t {cmd_bin, cmd_script, cmd_bg, cmd_pipe, cmd_builtin };

enum builtin_t { builtin_none, builtin_cd, builtin_exit };

struct s_builtins {
	enum builtin_t type;
	char * command;
};
