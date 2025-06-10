; boot.asm - Multiboot compliant entry point for standard compilation

MBALIGN     equ  1<<0                   ; align loaded modules on page boundaries
MEMINFO     equ  1<<1                   ; provide memory map
MBFLAGS     equ  MBALIGN | MEMINFO      ; this is the Multiboot 'flag' field
MAGIC       equ  0x1BADB002             ; 'magic number' lets bootloader find the header
CHECKSUM    equ -(MAGIC + MBFLAGS)      ; checksum of above, to prove we are multiboot

; Multiboot header
section .multiboot
align 4
    dd MAGIC
    dd MBFLAGS
    dd CHECKSUM

; Stack space
section .bss
align 16
stack_bottom:
    resb 16384 ; 16 KiB stack
stack_top:

; Entry point
section .text
global _start:function (_start.end - _start)
_start:
    ; Set up the stack
    mov esp, stack_top

    ; Call the kernel main function
    extern kernel_main
    call kernel_main

    ; If kernel_main returns, enter an infinite loop
    cli
.hang:
    hlt
    jmp .hang
.end:
