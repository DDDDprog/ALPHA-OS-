/* Linker script for RISC-V Alpha OS */

ENTRY(_start)

BASE_ADDRESS = 0x80200000;

SECTIONS
{
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
    
    .bss : {
        __bss_start = .;
        *(.bss)
        . = ALIGN(8);
        __bss_end = .;
    }
    
    _kernel_end = .;
    
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
    }
}
