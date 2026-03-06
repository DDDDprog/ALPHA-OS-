#include "../include/kernel/scheduler.h"
#include "../include/kernel/cpu.h"
#include "../include/kernel/memory.h"
#include "../include/kernel/system.h"
#include "../include/libc/stdio.h"
#include "../include/libc/string.h"

#ifndef TEST_MODE

// Process table
static process_t* process_table[MAX_PROCESSES];
static process_t* current_process = NULL;
static process_t* ready_queue = NULL;
static process_t* sleeping_queue = NULL;
static uint32_t next_pid = 1;

// Scheduler statistics
static scheduler_stats_t sched_stats = {0};

// Current process ID
static uint32_t current_pid = 0;

void scheduler_init(void) {
    memset(process_table, 0, sizeof(process_table));
    ready_queue = NULL;
    sleeping_queue = NULL;
    current_process = NULL;
    next_pid = 1;
    current_pid = 0;
    
    // Create idle process
    process_t* idle = process_create("idle", PROCESS_FLAG_KERNEL);
    if (idle) {
        idle->priority = 0;
        idle->state = PROCESS_STATE_READY;
    }
    
    printf("Scheduler initialized\n");
}

process_t* process_create(const char* name, uint32_t flags) {
    if (!name) return NULL;
    
    process_t* process = malloc(sizeof(process_t));
    if (!process) return NULL;
    
    memset(process, 0, sizeof(process_t));
    
    // Allocate PID
    process->pid = next_pid++;
    
    // Copy name
    strncpy(process->name, name, PROCESS_NAME_LEN - 1);
    process->name[PROCESS_NAME_LEN - 1] = '\0';
    
    // Set flags
    process->flags = flags;
    process->priority = 50;  // Default priority
    
    // Initial state
    process->state = PROCESS_STATE_NEW;
    
    // Set default file descriptors (0,1,2 for stdin/stdout/stderr)
    process->stdin_fd = 0;
    process->stdout_fd = 1;
    process->stderr_fd = 2;
    
    // Time slice
    process->time_slice = 10;  // 10 ticks
    process->cpu_time_used = 0;
    
    // Creation time
    process->creation_time = timer_get_ticks();
    process->user_time = 0;
    process->kernel_time = 0;
    
    // Add to process table
    if (process->pid < MAX_PROCESSES) {
        process_table[process->pid] = process;
        sched_stats.total_processes++;
    }
    
    return process;
}

int process_destroy(process_t* process) {
    if (!process) return -1;
    
    process->state = PROCESS_STATE_TERMINATED;
    sched_stats.total_processes--;
    
    // Free resources would go here
    
    free(process);
    return 0;
}

process_t* process_get_current(void) {
    return current_process;
}

process_t* process_get_by_pid(uint32_t pid) {
    if (pid < MAX_PROCESSES) {
        return process_table[pid];
    }
    return NULL;
}

int process_start(process_t* process, void (*entry)(void*), void* arg) {
    if (!process || !entry) return -1;
    
    // Set up initial context
    process->context.eip = (uint32_t)entry;
    process->context.esp = process->user_stack;
    process->context.ebp = process->user_stack;
    
    // Set up stack for function call
    uint32_t* stack = (uint32_t*)process->user_stack;
    stack[-1] = (uint32_t)arg;
    stack[-2] = 0;  // Return address (will cause crash if returned)
    
    process->state = PROCESS_STATE_READY;
    
    // Add to ready queue
    process->next = ready_queue;
    process->prev = NULL;
    if (ready_queue) {
        ready_queue->prev = process;
    }
    ready_queue = process;
    
    return 0;
}

void process_exit(int code) {
    if (!current_process) return;
    
    current_process->exit_code = code;
    current_process->state = PROCESS_STATE_TERMINATED;
    
    printf("Process '%s' (PID %u) exited with code %d\n",
           current_process->name, current_process->pid, code);
    
    scheduler_yield();
}

int process_wait(process_t* process) {
    if (!process) return -1;
    
    while (process->state != PROCESS_STATE_TERMINATED) {
        process_sleep(10);
    }
    
    return process->exit_code;
}

void process_sleep(uint32_t ms) {
    if (!current_process) return;
    
    current_process->state = PROCESS_STATE_SLEEPING;
    current_process->wake_time = timer_get_ticks() + (ms * 100) / 10;
    
    // Move to sleeping queue
    current_process->next = sleeping_queue;
    current_process->prev = NULL;
    if (sleeping_queue) {
        sleeping_queue->prev = current_process;
    }
    sleeping_queue = current_process;
    
    scheduler_yield();
}

void process_wake(process_t* process) {
    if (!process) return;
    
    process->state = PROCESS_STATE_READY;
    
    // Move to ready queue
    process->next = ready_queue;
    process->prev = NULL;
    if (ready_queue) {
        ready_queue->prev = process;
    }
    ready_queue = process;
}

int process_kill(process_t* process) {
    if (!process) return -1;
    
    printf("Killing process '%s' (PID %u)\n", process->name, process->pid);
    
    process->exit_code = -1;
    process->state = PROCESS_STATE_TERMINATED;
    
    return 0;
}

int process_signal(process_t* process, int signum) {
    (void)process;
    (void)signum;
    return 0;
}

int process_set_priority(process_t* process, uint32_t priority) {
    if (!process) return -1;
    process->priority = priority > 100 ? 100 : priority;
    return 0;
}

uint32_t process_get_priority(process_t* process) {
    if (!process) return 0;
    return process->priority;
}

void scheduler_run(void) {
    scheduler_init();
    cpu_enable_interrupts();
    
    while (1) {
        scheduler_tick();
    }
}

void scheduler_yield(void) {
    // Simply call schedule
    scheduler_schedule();
}

void scheduler_schedule(void) {
    // Simple round-robin scheduling
    if (!ready_queue) {
        // Run idle
        return;
    }
    
    // Get next process
    process_t* next = ready_queue;
    
    // Move to end of ready queue
    ready_queue = ready_queue->next;
    if (ready_queue) {
        ready_queue->prev = NULL;
    }
    
    // Add current process to end of queue if it's still ready
    if (current_process && current_process->state == PROCESS_STATE_RUNNING) {
        current_process->state = PROCESS_STATE_READY;
        current_process->next = ready_queue;
        current_process->prev = NULL;
        if (ready_queue) {
            ready_queue->prev = current_process;
        }
        ready_queue = current_process;
    }
    
    // Switch to next process
    process_t* prev = current_process;
    current_process = next;
    current_process->state = PROCESS_STATE_RUNNING;
    current_process->cpu_time_used = 0;
    
    // Context switch
    if (prev != current_process) {
        sched_stats.context_switches++;
        
        if (prev) {
            // Save previous context would go here
        }
        
        // Load new context
        // In a real kernel, this would switch page tables and stacks
    }
}

void scheduler_tick(void) {
    if (!current_process) return;
    
    current_process->cpu_time_used++;
    
    if (current_process->cpu_time_used >= current_process->time_slice) {
        // Time slice exhausted
        scheduler_schedule();
    }
    
    // Check sleeping processes
    uint64_t now = timer_get_ticks();
    process_t* sleep = sleeping_queue;
    while (sleep) {
        process_t* next = sleep->next;
        if (now >= sleep->wake_time) {
            process_wake(sleep);
        }
        sleep = next;
    }
}

void scheduler_block(process_t* process) {
    if (!process) return;
    process->state = PROCESS_STATE_BLOCKED;
    sched_stats.blocked_processes++;
}

void scheduler_unblock(process_t* process) {
    if (!process) return;
    process_wake(process);
    sched_stats.blocked_processes--;
}

void scheduler_get_stats(scheduler_stats_t* stats) {
    if (stats) {
        memcpy(stats, &sched_stats, sizeof(scheduler_stats_t));
    }
}

void context_switch(process_context_t* old_ctx, process_context_t* new_ctx) {
    (void)old_ctx;
    (void)new_ctx;
    // In a real implementation, this would save and restore all registers
    __asm__ volatile ("pushad");
    __asm__ volatile ("popad");
}

void idle_process(void* arg) {
    (void)arg;
    while (1) {
        cpu_idle();
    }
}

#else

// Test mode implementations
#include <stdlib.h>
#include <unistd.h>

void scheduler_init(void) {
    printf("[TEST] Scheduler initialized\n");
}

process_t* process_create(const char* name, uint32_t flags) {
    (void)name;
    (void)flags;
    process_t* p = malloc(sizeof(process_t));
    if (p) {
        memset(p, 0, sizeof(process_t));
        p->pid = 1;
    }
    return p;
}

int process_destroy(process_t* process) {
    free(process);
    return 0;
}

process_t* process_get_current(void) { return NULL; }
process_t* process_get_by_pid(uint32_t pid) { (void)pid; return NULL; }
int process_start(process_t* process, void (*entry)(void*), void* arg) {
    (void)process; (void)entry; (void)arg;
    return 0;
}
void process_exit(int code) { exit(code); }
int process_wait(process_t* process) { (void)process; return 0; }
void process_sleep(uint32_t ms) { usleep(ms * 1000); }
void process_wake(process_t* process) { (void)process; }
int process_kill(process_t* process) { (void)process; return 0; }
int process_signal(process_t* process, int signum) { (void)process; (void)signum; return 0; }
int process_set_priority(process_t* process, uint32_t priority) {
    (void)process; (void)priority; return 0;
}
uint32_t process_get_priority(process_t* process) { (void)process; return 50; }
void scheduler_run(void) {}
void scheduler_yield(void) {}
void scheduler_schedule(void) {}
void scheduler_tick(void) {}
void scheduler_block(process_t* process) { (void)process; }
void scheduler_unblock(process_t* process) { (void)process; }
void scheduler_get_stats(scheduler_stats_t* stats) {
    if (stats) memset(stats, 0, sizeof(scheduler_stats_t));
}
void context_switch(process_context_t* old_ctx, process_context_t* new_ctx) {
    (void)old_ctx; (void)new_ctx;
}
void idle_process(void* arg) { (void)arg; while(1) sleep(1); }

#endif
