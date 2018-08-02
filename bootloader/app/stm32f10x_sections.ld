/*
 * Скрипт компоновщика для stm32.
 * Часть задания секций.
 */

/* Секции. */
SECTIONS
{
    /* Вектора прерываний. */
    .isr_vector : ALIGN(4)
    {
        _sisr = .;
        
        KEEP(*(.isr_vector))
        
        . = ALIGN(4);
        _eisr = .;
    } > FLASH
    
    /* Код и данные только для чтения. */
    .text : ALIGN(4)
    {
        _stext = .;
        
        *(.text)
        *(.text.*)
        *(.rodata)
        *(.rodata.*)
        *(.glue_7)
        *(.glue_7t)
        
        . = ALIGN(4);
        _etext = .;
    } > FLASH
    
    /* Массив предварительной инициализации. */
    .preinit : ALIGN(4)
    {
        _spreinit_array = .;
        
        KEEP(*(SORT(.preinit_array.*)))
        KEEP(*(.preinit_array))
        
        . = ALIGN(4);
        _epreinit_array = .;
    } > FLASH
    
    /* Массив инициализации. */
    .init : ALIGN(4)
    {
        _sinit_array = .;
        
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        
        . = ALIGN(4);
        _einit_array = .;
    } > FLASH
    
    /* Массив деинициализации. */
    .fini : ALIGN(4)
    {
        _sfini_array = .;
        
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        
        . = ALIGN(4);
        _efini_array = .;
    } > FLASH
    
    /* ARM magic sections */
    .ARM.extab : ALIGN(4)
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH
    
    .ARM.exidx : ALIGN(4)
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > FLASH
    
    . = ALIGN(4);
    /* Адрес инициализированных данных. */
    _sidata = .;
    
    /* Инициализированные данные. */
    .data : AT(_sidata) ALIGN(4)
    {
        _sdata = .;
        
        *(.data)
        *(.data.*)
        
        . = ALIGN(4);
        _edata = .;
    } > RAM
    
    /* Неинициализированные данные. */
    .bss : ALIGN(4)
    {
        _sbss = .;
        
        *(.bss)
        *(.bss.*)
        *(COMMON)
        
        . = ALIGN(4);
        _ebss = .;
    } > RAM
    
    /* Стек минимального размера. */
    .min_stack : ALIGN(4)
    {
        _smin_stack = .;
        
        . = . + _min_stack_size;
        
        . = ALIGN(4);
        _emin_stack = .;
    } > RAM
    
    /* Отладочная информация стандартных библиотек. */
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }
}
