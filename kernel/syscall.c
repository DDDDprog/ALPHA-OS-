/**
 * Alpha OS - System Call Implementation
 */

#include "../include/kernel/syscall.h"
#include "../include/kernel/console.h"
#include "../include/kernel/scheduler.h"
#include "../include/kernel/memory.h"
#include "../include/kernel/device.h"
#include "../include/libc/string.h"
#include "../include/libc/stdio.h"

#ifdef TEST_MODE
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#else
#endif

/* System call table */
static syscall_entry_t syscall_table[NR_SYSCALLS];
static bool syscall_initialized = false;

/* ============================================================
 * Initialization
 * ============================================================ */

#ifdef TEST_MODE

int syscall_init(void) {
    if (syscall_initialized) return 0;
    
    printf("[SYSCALL] Initializing system call interface\n");
    
    /* Initialize table */
    memset(syscall_table, 0, sizeof(syscall_table));
    
    /* Register default syscalls */
    syscall_register(SYS_EXIT, "exit", (syscall_handler_t)sys_exit);
    syscall_register(SYS_GETPID, "getpid", (syscall_handler_t)sys_getpid);
    syscall_register(SYS_GETPPID, "getppid", (syscall_handler_t)sys_getppid);
    syscall_register(SYS_GETUID, "getuid", (syscall_handler_t)sys_getuid);
    syscall_register(SYS_TIME, "time", (syscall_handler_t)sys_time);
    syscall_register(SYS_GETTIMEOFDAY, "gettimeofday", (syscall_handler_t)sys_gettimeofday);
    syscall_register(SYS_UNAME, "uname", (syscall_handler_t)sys_uname);
    syscall_register(SYS_SYSINFO, "sysinfo", (syscall_handler_t)sys_sysinfo);
    syscall_register(SYS_OPEN, "open", (syscall_handler_t)sys_open);
    syscall_register(SYS_CLOSE, "close", (syscall_handler_t)sys_close);
    syscall_register(SYS_READ, "read", (syscall_handler_t)sys_read);
    syscall_register(SYS_WRITE, "write", (syscall_handler_t)sys_write);
    
    syscall_initialized = true;
    return 0;
}

int syscall_shutdown(void) {
    printf("[SYSCALL] Shutting down system call interface\n");
    return 0;
}

#else

int syscall_init(void) {
    if (syscall_initialized) return 0;
    memset(syscall_table, 0, sizeof(syscall_table));
    syscall_initialized = true;
    return 0;
}

int syscall_shutdown(void) {
    return 0;
}

#endif

/* ============================================================
 * System Call Registration
 * ============================================================ */

int syscall_register(int num, const char* name, syscall_handler_t handler) {
    if (num < 0 || num >= NR_SYSCALLS) return -1;
    if (!name || !handler) return -1;
    
    syscall_table[num].name = name;
    syscall_table[num].handler = handler;
    
    return 0;
}

const char* syscall_get_name(int num) {
    if (num < 0 || num >= NR_SYSCALLS) return NULL;
    return syscall_table[num].name;
}

int syscall_get_number(const char* name) {
    if (!name) return -1;
    
    for (int i = 0; i < NR_SYSCALLS; i++) {
        if (syscall_table[i].name && strcmp(syscall_table[i].name, name) == 0) {
            return i;
        }
    }
    
    return -1;
}

/* ============================================================
 * System Call Invocation
 * ============================================================ */

int syscall(int num, uint32_t arg0, uint32_t arg1, uint32_t arg2,
             uint32_t arg3, uint32_t arg4, uint32_t arg5) {
    if (num < 0 || num >= NR_SYSCALLS) return -1;
    if (!syscall_table[num].handler) return -1;
    
    syscall_args_t args = {
        .arg0 = arg0,
        .arg1 = arg1,
        .arg2 = arg2,
        .arg3 = arg3,
        .arg4 = arg4,
        .arg5 = arg5
    };
    
    return syscall_table[num].handler(&args);
}

/* ============================================================
 * Process Management Syscalls
 * ============================================================ */

int sys_exit(int exit_code) {
#ifdef TEST_MODE
    printf("[SYSCALL] Exit called with code %d\n", exit_code);
    exit(exit_code);
#else
    process_exit(exit_code);
#endif
    return 0;
}

int sys_fork(void) {
#ifdef TEST_MODE
    return fork();
#else
    return 0;  /* Would implement actual fork */
#endif
}

int sys_getpid(void) {
#ifdef TEST_MODE
    return getpid();
#else
    process_t* proc = process_get_current();
    return proc ? proc->pid : 0;
#endif
}

int sys_getppid(void) {
#ifdef TEST_MODE
    return getppid();
#else
    process_t* proc = process_get_current();
    return proc ? proc->ppid : 0;
#endif
}

int sys_kill(int pid, int sig) {
#ifdef TEST_MODE
    return kill(pid, sig);
#else
    (void)pid; (void)sig;
    return 0;
#endif
}

/* ============================================================
 * File Syscalls
 * ============================================================ */

int sys_open(const char* pathname, int flags, int mode) {
#ifdef TEST_MODE
    return open(pathname, flags, mode);
#else
    (void)pathname; (void)flags; (void)mode;
    return -1;
#endif
}

int sys_close(int fd) {
#ifdef TEST_MODE
    return close(fd);
#else
    (void)fd;
    return 0;
#endif
}

int sys_read(int fd, void* buf, size_t count) {
#ifdef TEST_MODE
    return read(fd, buf, count);
#else
    return device_read(fd, buf, count);
#endif
}

int sys_write(int fd, const void* buf, size_t count) {
#ifdef TEST_MODE
    return write(fd, buf, count);
#else
    return device_write(fd, buf, count);
#endif
}

/* ============================================================
 * Memory Syscalls
 * ============================================================ */

void* sys_brk(void* addr) {
#ifdef TEST_MODE
    return (void*)-1;  /* Not really applicable in userspace */
#else
    (void)addr;
    return NULL;
#endif
}

/* ============================================================
 * Time Syscalls
 * ============================================================ */

time_t sys_time(time_t* tloc) {
#ifdef TEST_MODE
    time_t t = time(tloc);
    return t;
#else
    (void)tloc;
    return system_get_uptime();
#endif
}

int sys_gettimeofday(struct timeval* tv, struct timezone* tz) {
#ifdef TEST_MODE
    return gettimeofday(tv, tz);
#else
    if (tv) {
        tv->tv_sec = system_get_uptime();
        tv->tv_usec = 0;
    }
    (void)tz;
    return 0;
#endif
}

unsigned int sys_alarm(unsigned int seconds) {
#ifdef TEST_MODE
    return alarm(seconds);
#else
    (void)seconds;
    return 0;
#endif
}

/* ============================================================
 * Info Syscalls
 * ============================================================ */

int sys_getuid(void) {
    return 0;  /* Root for now */
}

int sys_uname(struct utsname* buf) {
#ifdef TEST_MODE
    return uname(buf);
#else
    if (!buf) return -1;
    
    strcpy(buf->sysname, "AlphaOS");
    strcpy(buf->nodename, "alphaos");
    strcpy(buf->release, "2.0.0");
    strcpy(buf->version, "2026");
    strcpy(buf->machine, "x86");
    strcpy(buf->domainname, "(none)");
    
    return 0;
#endif
}

int sys_sysinfo(struct sysinfo* info) {
#ifdef TEST_MODE
    return sysinfo(info);
#else
    if (!info) return -1;
    
    memset(info, 0, sizeof(struct sysinfo));
    info->uptime = system_get_uptime();
    
    memory_stats_t mem;
    memory_get_extended_stats(&mem);
    info->totalram = mem.total_pages * 4096;
    info->freeram = mem.free_pages * 4096;
    info->procs = 1;
    info->mem_unit = 1;
    
    return 0;
#endif
}

/* ============================================================
 * Signal Syscalls
 * ============================================================ */

int sys_sigaction(int signum, const struct sigaction* act, struct sigaction* oldact) {
#ifdef TEST_MODE
    struct sigaction sa;
    return sigaction(signum, act ? act : &sa, oldact);
#else
    (void)signum; (void)act; (void)oldact;
    return 0;
#endif
}

int sys_sigprocmask(int how, const sigset_t* set, sigset_t* oldset) {
#ifdef TEST_MODE
    return sigprocmask(how, set, oldset);
#else
    (void)how; (void)set; (void)oldset;
    return 0;
#endif
}
