/**
 * Alpha OS - System Call Implementation
 * Minimal stub implementation
 */

#include "../include/kernel/console.h"
#include "../include/kernel/memory.h"
#include "../include/kernel/system.h"
#include "../include/kernel/types.h"
#include "../include/libc/string.h"

#ifdef TEST_MODE
#include <stdio.h>
#endif

/* Process handlers */
int sys_exit(int exit_code) {
    (void)exit_code;
    return 0;
}

int sys_fork(void) { return -1; }

int sys_getpid(void) { return 1; }

int sys_getppid(void) { return 0; }

int sys_kill(int pid, int sig) {
    (void)pid; (void)sig;
    return 0;
}

/* File handlers - stubs */
int sys_open(const char* pathname, int flags, int mode) {
    (void)pathname; (void)flags; (void)mode;
    return -1;
}

int sys_close(int fd) {
    (void)fd;
    return 0;
}

long sys_read(int fd, void* buf, size_t count) {
    (void)fd; (void)buf; (void)count;
    return -1;
}

long sys_write(int fd, const void* buf, size_t count) {
    (void)fd; (void)buf; (void)count;
    return -1;
}

long sys_lseek(int fd, long offset, int whence) {
    (void)fd; (void)offset; (void)whence;
    return -1;
}

int sys_mkdir(const char* pathname, int mode) {
    (void)pathname; (void)mode;
    return -1;
}

int sys_rmdir(const char* pathname) {
    (void)pathname;
    return -1;
}

int sys_unlink(const char* pathname) {
    (void)pathname;
    return -1;
}

/* Memory handlers */
void* sys_brk(void* addr) {
    (void)addr;
    return NULL;
}

/* Time handlers */
unsigned long sys_time(unsigned long* tloc) {
    unsigned long t = 0;
    if (tloc) *tloc = t;
    return t;
}

/* Info handlers */
int sys_getuid(void) { return 0; }

int syscall_init(void) {
    return 0;
}
