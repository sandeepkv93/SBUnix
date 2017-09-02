enum cmd_type {cmd_bin, cmd_script, cmd_bg, cmd_pipe };

#define __DEBUG__ 1

#ifdef __DEBUG__  
	#define debug_print(...)  printf(__VA_ARGS__);  
#else
	#define debug_print(...)
#endif
