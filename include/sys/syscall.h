#define _SYS__read				0
#define _SYS__write				1
#define _SYS__open				2
#define _SYS__close				3
#define _SYS__stat				4
#define _SYS__fstat				5
#define _SYS__lstat				6
#define _SYS__poll				7

#define _SYS__lseek				8
#define _SYS__mmap				9
#define _SYS__mprotect			10
#define _SYS__munmap			11
#define _SYS__brk				12
#define _SYS__rt_sigaction		13
#define _SYS__rt_sigprocmas		14
#define _SYS__rt_sigreturn		15

#define _SYS__ioctl				16
#define _SYS__pread64			17
#define _SYS__pwrite64			18
#define _SYS__readv				19
#define _SYS__writev			20
#define _SYS__acces				21
#define _SYS__pipe				22
#define _SYS__select			23

#define _SYS__sched_yield		24
#define _SYS__mremap			25
#define _SYS__msync				26
#define _SYS__mincore			27
#define _SYS__madvise			28
#define _SYS__shmge				29
#define _SYS__shmat				30
#define _SYS__shmctl			31

#define _SYS__dup				32
#define _SYS__dup2				33
#define _SYS__pause				34
#define _SYS__nanosleep			35
#define _SYS__getitimer			36
#define _SYS__alarm				37
#define _SYS__setitimer			38
#define _SYS__getpid			39

#define _SYS__sendfile			40
#define _SYS__socket			41
#define _SYS__connect			42
#define _SYS__accept			43
#define _SYS__sendto			44
#define _SYS__recvfrom			45
#define _SYS__sendmsg			46
#define _SYS__recvmsg			47

#define _SYS__shutdown			48
#define _SYS__bind				49
#define _SYS__listen			50
#define _SYS__getsockname		51
#define _SYS__getpeername		52
#define _SYS__socketpair		53
#define _SYS__setsockopt		54
#define _SYS__getsockopt		55

#define _SYS__clone				56
#define _SYS__fork				57
#define _SYS__vfork				58
#define _SYS__execve			59
#define _SYS__exit				60
#define _SYS__wait4				61
#define _SYS__kill				62
#define _SYS__uname				63

#define _SYS__semget			64
#define _SYS__semop				65
#define _SYS__semctl			66
#define _SYS__shmdt				67
#define _SYS__msgget			68
#define _SYS__msgsnd			69
#define _SYS__msgrcv			70
#define _SYS__msgctl			71

#define _SYS__fcntl				72
#define _SYS__flock				73
#define _SYS__fsync				74
#define _SYS__fdatasync			75
#define _SYS__truncate			76
#define _SYS__ftruncate			77
#define _SYS__getdents			78
#define _SYS__getcwd			79

#define _SYS__chdir				80
#define _SYS__fchdir			81
#define _SYS__rename			82
#define _SYS__mkdir				83
#define _SYS__rmdir				84
#define _SYS__creat				85
#define _SYS__link				86
#define _SYS__unlink			87

#define _SYS__symlink			88
#define _SYS__readlink			89
#define _SYS__chmod				90
#define _SYS__fchmod			91
#define _SYS__chown				92
#define _SYS__fchown			93
#define _SYS__lchown			94
#define _SYS__umask				95

#define _SYS__gettimeofday		96
#define _SYS__getrlimit			97
#define _SYS__getrusage			98
#define _SYS__sysinfo			99
#define _SYS__times				100
#define _SYS__ptrace			101
#define _SYS__getuid			102
#define _SYS__syslog			103

	/* at the very end the stuff that never runs during the benchmarks */
#define _SYS__getgid			104
#define _SYS__setuid			105
#define _SYS__setgid			106
#define _SYS__geteuid			107
#define _SYS__getegid			108
#define _SYS__setpgid			109
#define _SYS__getppid			110
#define _SYS__getpgrp			111

#define _SYS__setsid			112
#define _SYS__setreuid			113
#define _SYS__setregid			114
#define _SYS__getgroups			115
#define _SYS__setgroups			116
#define _SYS__setresuid			117
#define _SYS__getresuid			118
#define _SYS__setresgid			119

#define _SYS__getresgid			120
#define _SYS__getpgid			121
#define _SYS__setfsuid			122
#define _SYS__setfsgid			123
#define _SYS__getsid			124
#define _SYS__capget			125
#define _SYS__capset			126

#define _SYS__rt_sigpending		127
#define _SYS__rt_sigtimedwait	128
#define _SYS__rt_sigqueueinfo	129
#define _SYS__rt_sigsuspend		130
#define _SYS__sigaltstack		131
#define _SYS__utime				132
#define _SYS__mknod				133

#define _SYS__uselib			134
#define _SYS__personality		135

#define _SYS__ustat				136
#define _SYS__statfs			137
#define _SYS__fstatfs			138
#define _SYS__sysfs				139

#define _SYS__getpriority				140
#define _SYS__setpriority				141
#define _SYS__sched_setparam			142
#define _SYS__sched_getparam			143
#define _SYS__sched_setscheduler		144
#define _SYS__sched_getscheduler		145
#define _SYS__sched_get_priority_max	146
#define _SYS__sched_get_priority_min	147
#define _SYS__sched_rr_get_interval	148

#define _SYS__mlock						149
#define _SYS__munlock					150
#define _SYS__mlockall					151
#define _SYS__munlockall				152

#define _SYS__vhangup					153

#define _SYS__modify_ldt				154

#define _SYS__pivot_root				155

#define _SYS___sysctl					156

#define _SYS__prctl						157
#define _SYS__arch_prctl				158

#define _SYS__adjtimex					159

#define _SYS__setrlimit					160

#define _SYS__chroot					161

#define _SYS__sync						162

#define _SYS__acct						163

#define _SYS__settimeofday				164

#define _SYS__mount						165
#define _SYS__umount2					166

#define _SYS__swapon					167
#define _SYS__swapoff					168

#define _SYS__reboot					169

#define _SYS__sethostname				170
#define _SYS__setdomainname				171

#define _SYS__iopl						172
#define _SYS__ioperm					173

#define _SYS__create_module				174
#define _SYS__init_module				175
#define _SYS__delete_module				176
#define _SYS__get_kernel_syms			177
#define _SYS__query_module				178

#define _SYS__quotactl					179

#define _SYS__nfsservctl				180

	/* reserved for LiS/STREAMS */
#define _SYS__getpmsg				181
#define _SYS__putpmsg				182

	/* reserved for AFS */
#define _SYS__afs_syscall			183

	/* reserved for tux */
#define _SYS__tuxcall				184

#define _SYS__security				185

#define _SYS__gettid				186

#define _SYS__readahead				187
#define _SYS__setxattr				188
#define _SYS__lsetxattr				189
#define _SYS__fsetxattr				190
#define _SYS__getxattr				191
#define _SYS__lgetxattr				192
#define _SYS__fgetxattr				193
#define _SYS__listxattr				194
#define _SYS__llistxattr			195
#define _SYS__flistxattr			196
#define _SYS__removexattr			197
#define _SYS__lremovexattr			198
#define _SYS__fremovexattr			199
#define _SYS__tkill					200
#define _SYS__time					201
#define _SYS__futex					202
#define _SYS__sched_setaffinity		203
#define _SYS__sched_getaffinity		204
#define _SYS__set_thread_area		205
#define _SYS__io_setup				206
#define _SYS__io_destroy			207
#define _SYS__io_getevents			208
#define _SYS__io_submit				209
#define _SYS__io_cancel				210
#define _SYS__get_thread_area		211
#define _SYS__lookup_dcookie		212
#define _SYS__epoll_create			213
#define _SYS__epoll_ctl_old			214
#define _SYS__epoll_wait_old		215
#define _SYS__remap_file_pages		216
#define _SYS__getdents64			217
#define _SYS__set_tid_address		218
#define _SYS__restart_syscall		219
#define _SYS__semtimedop			220
#define _SYS__fadvise64				221
#define _SYS__timer_create			222
#define _SYS__timer_settime			223
#define _SYS__timer_gettime			224
#define _SYS__timer_getoverrun		225
#define _SYS__timer_delete			226
#define _SYS__clock_settime			227
#define _SYS__clock_gettime			228
#define _SYS__clock_getres			229
#define _SYS__clock_nanosleep		230
#define _SYS__exit_group			231
#define _SYS__epoll_wait			232
#define _SYS__epoll_ctl				233
#define _SYS__tgkill				234
#define _SYS__utimes				235
#define _SYS__vserver				236
#define _SYS__mbind					237
#define _SYS__set_mempolicy			238
#define _SYS__get_mempolicy			239
#define _SYS__mq_open				240
#define _SYS__mq_unlink				241
#define _SYS__mq_timedsend			242
#define _SYS__mq_timedreceive		243
#define _SYS__mq_notify				244
#define _SYS__mq_getsetattr			245
#define _SYS__kexec_load			246
#define _SYS__waitid				247
#define _SYS__add_key				248
#define _SYS__request_key			249
#define _SYS__keyctl				250
#define _SYS__ioprio_set			251
#define _SYS__ioprio_get			252
#define _SYS__inotify_init			253
#define _SYS__inotify_add_watch		254
#define _SYS__inotify_rm_watch		255
#define _SYS__migrate_pages			256
#define _SYS__openat				257
#define _SYS__mkdirat				258
#define _SYS__mknodat				259
#define _SYS__fchownat				260
#define _SYS__futimesat				261
#define _SYS__newfstatat			262
#define _SYS__unlinkat				263
#define _SYS__renameat				264
#define _SYS__linkat				265
#define _SYS__symlinkat				266
#define _SYS__readlinkat			267
#define _SYS__fchmodat				268
#define _SYS__faccessat				269
#define _SYS__pselect6				270
#define _SYS__ppoll					271
#define _SYS__unshare				272
#define _SYS__set_robust_list		273
#define _SYS__get_robust_list		274
#define _SYS__splice				275
#define _SYS__tee					276
#define _SYS__sync_file_range		277
#define _SYS__vmsplice				278
#define _SYS__move_pages			279
#define _SYS__utimensat				280
#define _SYS__epoll_pwait			281
#define _SYS__signalfd				282
#define _SYS__timerfd_create		283
#define _SYS__eventfd				284
#define _SYS__fallocate				285
#define _SYS__timerfd_settime		286
#define _SYS__timerfd_gettime		287
#define _SYS__accept4				288
#define _SYS__signalfd4				289
#define _SYS__eventfd2				290
#define _SYS__epoll_create1			291
#define _SYS__dup3					292
#define _SYS__pipe2					293
#define _SYS__inotify_init1			294
#define _SYS__preadv				295
#define _SYS__pwritev				296
#define _SYS__rt_tgsigqueueinfo		297
#define _SYS__perf_event_open		298

#define mode_t long
#define size_t long

int write_ (int, const void *, size_t);
int syscall_ (long, long, long, long);
void exit_ (int);
int read_(int , void * , size_t );
int open_(const char * , int , mode_t );

