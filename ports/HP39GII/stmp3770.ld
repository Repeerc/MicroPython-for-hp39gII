/* GNU linker script for STMP3770 */

ENTRY(boot)


MEMORY
{
    RAM (xrw)       : ORIGIN = 0x00000000, LENGTH = 0x080000 /* 512 KiB */
}



SECTIONS
{


    VRAM_ADDR = 0x0100;
    /*
        0x0100 ~ 0x8800 VRAM
    */

    .usb_ram 0x9000:
	{
		*(.usb_ram)
	} >RAM



    .text 0xB000:
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
        _sidata = _etext;
    } >RAM

    .data : AT ( _sidata )
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(4);
        _edata = .;
    } >RAM

   .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) }
   .ARM.exidx   :
    {
      PROVIDE_HIDDEN (__exidx_start = .);
      *(.ARM.exidx* .gnu.linkonce.armexidx.*)
      PROVIDE_HIDDEN (__exidx_end = .);
    }

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >RAM

    .heap :
    {
        . = ALIGN(8);
        _heap_start = .;
    } >RAM


    /*_estack = ORIGIN(RAM) + LENGTH(RAM);*/


    . = ORIGIN(RAM) + LENGTH(RAM);
	ABT_STACK_ADDR = .;

	. = . - 0x300;
	UND_STACK_ADDR = .;
	
	. = . - 0x300;
	FIQ_STACK_ADDR = .;
	
	. = . - 0x300;
	IRQ_STACK_ADDR = .;	
	
	. = . - 0x300;
	SVC_STACK_ADDR = .;		
	
	. = . - 0x300;
	SYS_STACK_ADDR = .;	

    . = . - 0x300;

    _heap_stop = .;
}
