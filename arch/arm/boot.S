; ARM Bootloader for Alpha OS
; This code runs in ARM ARMv7-A mode (Cortex-A)

.section .text.boot

.global _start

_start:
    ; Set up stack pointer
    ldr sp, =stack_top
    
    ; Disable interrupts
    cpsid if
    
    ; Enable SWI/ABT/IRQ/FIQ modes and set stack pointers
    mov r0, #0x12        ; IRQ mode
    msr cpsr, r0
    ldr sp, =irq_stack_top
    
    mov r0, #0x13        ; SVC mode
    msr cpsr, r0
    ldr sp, =svc_stack_top
    
    mov r0, #0x17        ; ABT mode
    msr cpsr, r0
    ldr sp, =abt_stack_top
    
    mov r0, #0x1B        ; UND mode
    msr cpsr, r0
    ldr sp, =und_stack_top
    
    ; Return to SVC mode
    mov r0, #0x13
    msr cpsr, r0
    
    ; Clear BSS section
    ldr r0, =__bss_start__
    ldr r1, =__bss_end__
    mov r2, #0
    
clear_bss:
    cmp r0, r1
    strne r2, [r0], #4
    bne clear_bss
    
    ; Enable MMU and caches (if needed)
    ; For now, keep it simple
    
    ; Jump to kernel main
    bl kernel_main
    
    ; If kernel returns, halt
hang:
    wfi
    b hang

.section .data
.align 12

; Stack areas
.align 12
stack_top:
    .space 4096
irq_stack_top:
    .space 4096
svc_stack_top:
    .space 4096
abt_stack_top:
    .space 4096
und_stack_top:
    .space 4096

.end
