ENTRY(_start)
OUTPUT_FORMAT(elf64-x86-64)

SECTIONS
{
    . = 1M;

    .text ALIGN(0x1000): {
        *(.text)
        *(.text.*)
        *(.gnu.linkonce.t*)
    }

    .rodata ALIGN(0x1000): {
        *(.rodata)
        *(.rodata.*)
        *(.gnu.linkonce.r*)
    }

    .data ALIGN(0x1000): {
        *(.data)
        *(.data.*)
        *(.gnu.linkonce.d*)
    }

    .bss ALIGN(0x1000): {
        *(COMMON)
        *(.bss)
        *(.bss.*)
        *(.gnu.linkonce.b*)
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame) /* C++ Exception Handler Frames are not required for kernel */
    }

}
