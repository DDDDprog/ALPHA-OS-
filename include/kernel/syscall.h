/**
 * Alpha OS - System Call Interface
 * 
 * System call numbers and handlers
 */

#ifndef SYSCALL_H
#define SYSCALL_H

#include "../kernel/types.h"

/* ============================================================
 * System Call Numbers
 * ============================================================ */

/* Process management */
#define SYS_EXIT         1
#define SYS_FORK         2
#define SYS_EXECVE       3
#define SYS_WAITPID      4
#define SYS_GETPID       5
#define SYS_GETPPID      6
#define SYS_KILL         7
#define SYS_NICE         8
#define SYS_SCHED_YIELD  9
#define SYS_CLONE       10

/* File operations */
#define SYS_OPEN         11
#define SYS_CLOSE        12
#define SYS_READ         13
#define SYS_WRITE        14
#define SYS_LSEEK        15
#define SYS_STAT         16
#define SYS_FSTAT        17
#define SYS_MKDIR        18
#define SYS_RMDIR        19
#define SYS_UNLINK       20
#define SYS_RENAME       21
#define SYS_CHDIR        22
#define SYS_GETCWD       23

/* Memory */
#define SYS_BRK          24
#define SYS_MMAP         25
#define SYS_MUNMAP       26
#define SYS_MPROTECT     27

/* Time */
#define SYS_TIME         28
#define SYS_GETTIMEOFDAY 29
#define SYS_ALARM        30
#define SYS_NANOSLEEP    31
#define SYS_UTIME        32

/* IPC */
#define SYS_PIPE         33
#define SYS_SHMGET       34
#define SYS_SHMAT        35
#define SYS_SHMDT        36
#define SYS_MSGGET       37
#define SYS_MSGSND       38
#define SYS_MSGRCV       39
#define SYS_MSGCTL       40
#define SYS_SEMGET       41
#define SYS_SEMOP        42
#define SYS_SEMCTL       43

/* Networking */
#define SYS_SOCKET       44
#define SYS_BIND         45
#define SYS_CONNECT      46
#define SYS_LISTEN       47
#define SYS_ACCEPT       48
#define SYS_SEND         49
#define SYS_RECV         50
#define SYS_SENDTO       51
#define SYS_RECVFROM     52

/* Info */
#define SYS_GETUID       53
#define SYS_SYSINFO      54
#define SYS_UNAME        55
#define SYS_GETHOSTNAME  56
#define SYS_GETDOMAINNAME 57

/* I/O */
#define SYS_DUP          58
#define SYS_DUP2         59
#define SYS_IOCTL        60
#define SYS_FCNTL        61

/* Signals */
#define SYS_SIGACTION    62
#define SYS_SIGPROCMASK 63
#define SYS_SIGPENDING   64
#define SYS_KILL         65

/* System */
#define SYS_REBOOT       66
#define SYS_HALT         67
#define SYS_POWEROFF     68

/* Number of syscalls */
#define NR_SYSCALLS      69

/* ============================================================
 * System Call Arguments
 * ============================================================ */

typedef struct {
    uint32_t arg0;
    uint32_t arg1;
    uint32_t arg2;
    uint32_t arg3;
    uint32_t arg4;
    uint32_t arg5;
} syscall_args_t;

/* ============================================================
 * System Call Return Values
 * ============================================================ */

#define SYSCALL_SUCCESS  0
#define SYSCALL_ERROR   -1

/* ============================================================
 * System Call Handler Type
 * ============================================================ */

typedef int (*syscall_handler_t)(syscall_args_t* args);

/* ============================================================
 * System Call Table Entry
 * ============================================================ */

typedef struct {
    const char* name;
    syscall_handler_t handler;
} syscall_entry_t;

/* ============================================================
 * API Functions
 * ============================================================ */

/* Initialize system call interface */
int syscall_init(void);
int syscall_shutdown(void);

/* Register system call handler */
int syscall_register(int num, const char* name, syscall_handler_t handler);

/* Get system call name */
const char* syscall_get_name(int num);

/* Get system call number */
int syscall_get_number(const char* name);

/* System call invocation (from user space) */
int syscall(int num, uint32_t arg0, uint32_t arg1, uint32_t arg2, 
            uint32_t arg3, uint32_t arg4, uint32_t arg5);

/* ============================================================
 * System Call Handlers
 * ============================================================ */

/* Process handlers */
int sys_exit(int exit_code);
int sys_fork(void);
int sys_execve(const char* filename, char* const argv[], char* const envp[]);
int sys_waitpid(int pid, int* status, int options);
int sys_getpid(void);
int sys_getppid(void);
int sys_kill(int pid, int sig);

/* File handlers */
int sys_open(const char* pathname, int flags, int mode);
int sys_close(int fd);
int sys_read(int fd, void* buf, size_t count);
int sys_write(int fd, const void* buf, size_t count);
int sys_lseek(int fd, off_t offset, int whence);
int sys_mkdir(const char* pathname, int mode);
int sys_rmdir(const char* pathname);
int sys_unlink(const char* pathname);

/* Memory handlers */
void* sys_brk(void* addr);
void* sys_mmap(void* addr, size_t len, int prot, int flags, int fd, off_t offset);
int sys_munmap(void* addr, size_t len);

/* Time handlers */
time_t sys_time(time_t* tloc);
int sys_gettimeofday(struct timeval* tv, struct timezone* tz);
unsigned int sys_alarm(unsigned int seconds);
int sys_nanosleep(const struct timespec* req, struct timespec* rem);

/* Info handlers */
int sys_getuid(void);
int sys_sysinfo(struct sysinfo* info);
int sys_uname(struct utsname* buf);

/* Signal handlers */
int sys_sigaction(int signum, const struct sigaction* act, struct sigaction* oldact);
int sys_sigprocmask(int how, const sigset_t* set, sigset_t* oldset);

/* ============================================================
 * Common Structures
 * ============================================================ */

struct timeval {
    time_t      tv_sec;   /* seconds */
    suseconds_t tv_usec;  /* microseconds */
};

struct timezone {
    int tz_minuteswest;  /* minutes west of Greenwich */
    int tz_dsttime;     /* type of DST correction */
};

struct timespec {
    time_t tv_sec;     /* seconds */
    long   tv_nsec;    /* nanoseconds */
};

struct sysinfo {
    long uptime;        /* Seconds since boot */
    unsigned long loads[3];  /* 1, 5, and 15 minute load averages */
    unsigned long totalram;  /* Total usable main memory size */
    unsigned long freeram;   /* Available memory size */
    unsigned long sharedram; /* Amount of shared memory */
    unsigned long bufferram; /* Memory used by buffers */
    unsigned long totalswap; /* Total swap space size */
    unsigned long freeswap;  /* Swap space still available */
    unsigned short procs;    /* Number of current processes */
    unsigned short pad;      /* Padding */
    unsigned long totalhigh; /* Total high memory size */
    unsigned long freehigh;  /* Available high memory size */
    unsigned int mem_unit;   /* Memory unit size in bytes */
    char _f[20-2*sizeof(long)-sizeof(int)]; /* Padding */
};

struct utsname {
    char sysname[65];    /* Operating system name */
    char nodename[65];   /* Network name */
    char release[65];    /* OS release */
    char version[65];    /* OS version */
    char machine[65];    /* Machine type */
    char domainname[65]; /* Domain name */
};

struct sigaction {
    void (*sa_handler)(int);  /* Signal handler */
    sigset_t sa_mask;         /* Signals to block */
    int sa_flags;             /* Flags */
    void (*sa_restorer)(void); /* Restore function */
};

/* Signal numbers */
#define SIGCHLD  17
#define SIGTERM  15
#define SIGKILL   9
#define SIGSEGV  11
#define SIGBUS   10
#define SIGFPE    8
#define SIGILL    4

#endif /* SYSCALL_H */
