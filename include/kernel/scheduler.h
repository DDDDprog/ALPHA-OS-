#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../kernel/types.h"
#include "../kernel/memory.h"

// Process states
typedef enum {
    PROCESS_STATE_NEW = 0,
    PROCESS_STATE_READY = 1,
    PROCESS_STATE_RUNNING = 2,
    PROCESS_STATE_BLOCKED = 3,
    PROCESS_STATE_SLEEPING = 4,
    PROCESS_STATE_TERMINATED = 5,
    PROCESS_STATE_WAITING = 6
} process_state_t;

// Process flags
#define PROCESS_FLAG_KERNEL    0x01
#define PROCESS_FLAG_USER       0x02
#define PROCESS_FLAG_DAEMON     0x04
#define PROCESS_FLAG_BG         0x08

// Maximum processes
#define MAX_PROCESSES 64
#define PROCESS_NAME_LEN 32
#define MAX_ARGS 16

// Process context (saved registers)
typedef struct {
    uint32_t eax, ebx, ecx, edx;
    uint32_t esi, edi, ebp, esp;
    uint32_t eip, eflags;
    uint32_t cs, ds, es, fs, gs, ss;
    uint32_t cr3;
} process_context_t;

// Memory region for process
typedef struct {
    uint32_t start;
    uint32_t end;
    uint32_t flags;
} process_vm_t;

// Process Control Block
typedef struct process {
    uint32_t pid;
    uint32_t ppid;
    char name[PROCESS_NAME_LEN];
    process_state_t state;
    uint32_t flags;
    uint32_t priority;
    
    // Memory
    page_directory_t* page_directory;
    process_vm_t vmareas[8];
    int vmarea_count;
    
    // Context
    process_context_t context;
    uint32_t kernel_stack;
    uint32_t user_stack;
    
    // Scheduling
    uint32_t time_slice;
    uint32_t cpu_time_used;
    uint32_t wake_time;
    
    // Files
    int stdin_fd;
    int stdout_fd;
    int stderr_fd;
    int file_descriptors[16];
    
    // Parent/Children
    struct process* parent;
    struct process* next;
    struct process* prev;
    
    // Return code
    int exit_code;
    
    // Statistics
    uint64_t creation_time;
    uint64_t user_time;
    uint64_t kernel_time;
} process_t;

// Scheduler statistics
typedef struct {
    uint32_t total_processes;
    uint32_t running_processes;
    uint32_t blocked_processes;
    uint32_t sleeping_processes;
    uint64_t context_switches;
    uint64_t total_cpu_time;
    uint32_t idle_time_ms;
} scheduler_stats_t;

// Scheduler initialization
void scheduler_init(void);

// Process management
process_t* process_create(const char* name, uint32_t flags);
int process_destroy(process_t* process);
process_t* process_get_current(void);
process_t* process_get_by_pid(uint32_t pid);

// Process lifecycle
int process_start(process_t* process, void (*entry)(void*), void* arg);
void process_exit(int code);
int process_wait(process_t* process);
void process_sleep(uint32_t ms);
void process_wake(process_t* process);

// Process control
int process_kill(process_t* process);
int process_signal(process_t* process, int signum);
int process_set_priority(process_t* process, uint32_t priority);
uint32_t process_get_priority(process_t* process);

// Scheduler
void scheduler_run(void);
void scheduler_yield(void);
void scheduler_schedule(void);
void scheduler_tick(void);
void scheduler_block(process_t* process);
void scheduler_unblock(process_t* process);

// Statistics
void scheduler_get_stats(scheduler_stats_t* stats);

// Context switching
void context_switch(process_context_t* old_ctx, process_context_t* new_ctx);

// Idle process
void idle_process(void* arg);

#endif // SCHEDULER_H
