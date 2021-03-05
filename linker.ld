ENTRY(_boot)
OUTPUT_FORMAT("elf64-littleriscv", "elf64-little", "elf64-big")
OUTPUT_ARCH("riscv")

SECTIONS
{
    . = 0x80000000;
    .text : {
        *(.text)
        *(.text.*)
        . = ALIGN(0x1000); /* one page */
    }

    .rodata : {
        . = ALIGN(16);
        *(.srodata)
        *(.srodata.*)
        . = ALIGN(16);
        *(.rodata)
        *(.rodata.*)
    }

    .data : {
        . = ALIGN(16);
        *(.sdata)
        *(.sdata.*)
        . = ALIGN(16);
        *(.data)
        *(.data.*)
    }

      .bss : {
        . = ALIGN(16);
        *(.sbss)
        *(.sbss.*)
        . = ALIGN(16);
        *(.bss)
        *(.bss.*)
    }
}
