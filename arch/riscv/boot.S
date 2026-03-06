; RISC-V Bootloader for Alpha OS
; RV64ACIMU (64-bit with atomics, compressed, integer, multiply, user mode)

.section .text.boot

.global _start

_start:
    ; Set up stack pointer
    la sp, stack_top
    
    ; Clear BSS section
    la a0, __bss_start
    la a1, __bss_end
    bge a0, a1, clear_bss_done
    
clear_bss:
    sd zero, 0(a0)
    addi a0, a0, 8
    blt a0, a1, clear_bss
    
clear_bss_done:
    
    ; Initialize trap vector
    la t0, trap_vector
    csrw mtvec, t0
    
    ; Enable M-mode interrupts
    csrr t0, mstatus
    ori t0, t0, 0x8   # Set MIE bit
    csrw mstatus, t0
    
    ; Enable timer and external interrupts
    csrr t0, mie
    ori t0, t0, 0x708  # Enable MTIE, MSIE, MEIE
    csrw mie, t0
    
    ; Jump to kernel main
    call kernel_main

hang:
    wfi
    j hang

.align 4
trap_vector:
    ; Simple trap handler
    csrr a0, mcause
    csrr a1, mepc
    # In a real OS, we'd handle this properly
    j hang

.section .data
.align 4

stack_top:
    .space 4096

.end
