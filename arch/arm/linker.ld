/* Linker script for ARM Alpha OS */

/* Entry point */
ENTRY(_start)

/* Base address for ARM */
BASE_ADDRESS = 0x8000;

/* Stack sizes */
STACK_SIZE = 4096;

SECTIONS
{
    /* Text section at base address */
    . = BASE_ADDRESS;
    
    _kernel_start = .;
    
    .text : {
        *(.text.boot)
        *(.text)
        *(.rodata)
    }
    
    .data : {
        *(.data)
    }
    
    /* BSS section */
    .bss : {
        __bss_start__ = .;
        *(.bss)
        . = ALIGN(4);
        __bss_end__ = .;
    }
    
    /* Stack top locations */
    . = . + STACK_SIZE;
    . = . + STACK_SIZE;
    . = . + STACK_SIZE;
    . = . + STACK_SIZE;
    . = . + STACK_SIZE;
    
    _kernel_end = .;
    
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
    }
}
