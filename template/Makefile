# Основная цель.
TARGET    = main

# Объектные файлы.
OBJECTS   = main.o

# Собственные библиотеки в исходниках.
SRC_LIBS  = 

# Макросы.
DEFINES  += 

# Стандартные драйвера периферии.
# STD_PERIPH_DRIVERS += misc
# STD_PERIPH_DRIVERS += stm32f10x_adc
# STD_PERIPH_DRIVERS += stm32f10x_bkp
# STD_PERIPH_DRIVERS += stm32f10x_can
# STD_PERIPH_DRIVERS += stm32f10x_cec
# STD_PERIPH_DRIVERS += stm32f10x_crc
# STD_PERIPH_DRIVERS += stm32f10x_dac
# STD_PERIPH_DRIVERS += stm32f10x_dbgmcu
# STD_PERIPH_DRIVERS += stm32f10x_dma
# STD_PERIPH_DRIVERS += stm32f10x_exti
# STD_PERIPH_DRIVERS += stm32f10x_flash
# STD_PERIPH_DRIVERS += stm32f10x_fsmc
# STD_PERIPH_DRIVERS += stm32f10x_gpio
# STD_PERIPH_DRIVERS += stm32f10x_i2c
# STD_PERIPH_DRIVERS += stm32f10x_iwdg
# STD_PERIPH_DRIVERS += stm32f10x_pwr
# STD_PERIPH_DRIVERS += stm32f10x_rcc
# STD_PERIPH_DRIVERS += stm32f10x_rtc
# STD_PERIPH_DRIVERS += stm32f10x_sdio
# STD_PERIPH_DRIVERS += stm32f10x_spi
# STD_PERIPH_DRIVERS += stm32f10x_tim
# STD_PERIPH_DRIVERS += stm32f10x_usart
# STD_PERIPH_DRIVERS += stm32f10x_wwdg

# Библиотеки.
LIBS      = c

# Оптимизация, вторая часть флага компилятора -O.
OPTIMIZE  = s

# Флаги отладки.
DEBUG    += 

# МК.
# Имя МК.
MCU_NAME   = stm32f103c8
# Семейство МК.
MCU_FAMILY = md
# Конфигурация МК.
# Адрес флеш-памяти.
MCU_FLASH_ORIGIN = 0x8000000
# Размер флеш-памяти.
MCU_FLASH_SIZE   = 0x10000
# Адрес ОЗУ.
MCU_RAM_ORIGIN   = 0x20000000
# Размер ОЗУ.
MCU_RAM_SIZE     = 0x5000
# Семейство МК в верхнем регистре.
MCU_FAMILY_UPPER = $(shell echo $(MCU_FAMILY) | tr '[:lower:]' '[:upper:]')

# Макросы МК.
# Макрос семейства МК.
DEFINES   += STM32F10X_$(MCU_FAMILY_UPPER)
# Использование стандартной библиотеки периферии.
DEFINES   += USE_STDPERIPH_DRIVER
# Использование набора инстркций Thumb-2
DEFINES   += __thumb2__=1

# Программа st-flash.
STFLASH    = "/usr/bin/st-flash"
STFLASH_WIN = "C:\Program Files\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility\ST-LINK_CLI.exe"
# Устройство stlink.
STLINK_DEV = /dev/stlinkv2_4
# OpenOCD
OPENOCD      = openocd
# Аргументы OpenOCD
OPENOCD_ARGS = -f "interface/stlink-v2.cfg" -f "target/stm32f1x.cfg"
# Команда OpenOCD
OPENOCD_CMD  = $(OPENOCD) $(OPENOCD_ARGS)

# Путь к библиотекам от производителя (SPL, CMSIS).
STM32_LIBS      = ../stm32

# Путь к стандартной библиотеке периферии.
SPL_PATH        = $(STM32_LIBS)/StdPeriphLib/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries
# Путь к стандартной библиотеке драйверов периферии.
STD_PERIPH_PATH = $(SPL_PATH)/STM32F10x_StdPeriph_Driver
# Путь к исходникам драйверов периферии.
STD_PERIPH_SRC  = $(STD_PERIPH_PATH)/src
# Путь к заголовочным файлам драйверов периферии.
STD_PERIPH_INC  = $(STD_PERIPH_PATH)/inc

# Путь к CMSIS
CMSIS_PATH                = $(STM32_LIBS)/CMSIS
# Путь к библиотеке ядра.
CMSIS_CORE_SUPPORT_PATH   = $(CMSIS_PATH)/CMSIS
# Заголовки библиотеки ядра.
CMSIS_CORE_SUPPORT_INC    = $(CMSIS_CORE_SUPPORT_PATH)/Include
# Исходники библиотеки ядра.
CMSIS_CORE_SUPPORT_SRC    = 
# Путь к библиотекам ядра.
CMSIS_CORE_SUPPORT_LIBS_PATH = $(CMSIS_CORE_SUPPORT_PATH)/Lib/GCC
# Библиотека математики.
CMSIS_MATH_LIB            = arm_cortexM3l_math
# Библиотеки ядра.
CMSIS_CORE_SUPPORT_LIBS  += $(CMSIS_MATH_LIB)

# Путь к библиотеке CMSIS в составе SPL.
SPL_CMSIS_PATH            = $(SPL_PATH)/CMSIS/CM3
# Путь к библиотеке устройства.
CMSIS_DEVICE_SUPPORT_PATH = $(SPL_CMSIS_PATH)/DeviceSupport/ST/STM32F10x
# Путь к заголовкам библиотеки устройства.
CMSIS_DEVICE_SUPPORT_INC  = $(CMSIS_DEVICE_SUPPORT_PATH)
# Путь к исходникам библиотеки устройства.
CMSIS_DEVICE_SUPPORT_SRC  = $(CMSIS_DEVICE_SUPPORT_PATH)
# Путь к стартовым файлам.
CMSIS_STARTUP_PATH        = $(CMSIS_DEVICE_SUPPORT_PATH)/startup/gcc_ride7
# Стартовый файл CMSIS.
CMSIS_STARTUP             = startup_stm32f10x_$(MCU_FAMILY).s

# Путь к собственным библиотекам в исходниках.
SRC_LIBS_PATH     += ../lib
# Пути ко всем собственным библиотекам в исходниках.
SRC_LIBS_ALL_PATH += $(wildcard $(addsuffix /*, $(SRC_LIBS_PATH)))

# Пути библиотек.
# Путь к библиотекам ядра.
LIBS_PATH += $(CMSIS_CORE_SUPPORT_LIBS_PATH)

# Скрипты линкера.
# Путь к скриптам линкера.
LDSCRIPTS_PATH += ../stm32/ld-scripts
# Имя файла скрипта.
LDSCRIPT   += $(LDSCRIPTS_PATH)/$(MCU_NAME).ld
# Путь поиска скриптов.
LIBS_PATH  += $(LDSCRIPTS_PATH)

# Флаги МК.
# Ядро.
MCUFLAGS  += -mcpu=cortex-m3 -mtune=cortex-m3
# Инструкции.
MCUFLAGS  += -mthumb

# Пути заголовочных файлов.
INCS     += .
INCS     += $(CMSIS_CORE_SUPPORT_INC)
INCS     += $(CMSIS_DEVICE_SUPPORT_INC)
INCS     += $(STD_PERIPH_INC)
INCS     += $(SRC_LIBS_PATH)

# Библиотеки.
# Библиотеки CMSIS.
LIBS     += $(CMSIS_CORE_SUPPORT_LIBS)

# Пути с исходниками.
VPATH   += .
VPATH   += $(CMSIS_CORE_SUPPORT_SRC)
VPATH   += $(CMSIS_DEVICE_SUPPORT_SRC)
VPATH   += $(STD_PERIPH_SRC)
VPATH   += $(CMSIS_STARTUP_PATH)
VPATH   += $(SRC_LIBS_ALL_PATH)

# Объектные файлы.
# CMSIS поддержка ядра.
OBJECTS += $(notdir $(patsubst %.c, %.o, $(wildcard $(addsuffix /*.c, $(CMSIS_CORE_SUPPORT_SRC)))))
# CMSIS поддержка устройства.
OBJECTS += $(notdir $(patsubst %.c, %.o, $(wildcard $(addsuffix /*.c, $(CMSIS_DEVICE_SUPPORT_SRC)))))
# Драйвера периферии.
OBJECTS += $(addsuffix .o, $(STD_PERIPH_DRIVERS))
# Стартовые файлы.
OBJECTS += $(patsubst %.s, %.o, $(CMSIS_STARTUP))
# Собственные библиотеки в исходниках.
OBJECTS += $(addsuffix .o, $(SRC_LIBS))

# Ассемблер листинги.
ASM     += $(patsubst %.o, %.s, $(OBJECTS))

# Флаги компилятора С.
# Использование каналов.
CFLAGS    += -pipe
# Флаги МК.
CFLAGS    += $(MCUFLAGS)
# Флаги оптимизации.
CFLAGS    += -O$(OPTIMIZE)
# Флаги отладки.
CFLAGS    += $(DEBUG)
# Выводить все предупреждения.
CFLAGS    += -Wall
# Без встроенных функций. (включено в -ffreestanding)
# CFLAGS    += -fno-builtin
# Код будет выполнятся без ОС.
CFLAGS    += -ffreestanding
# Помещать функции в отдельные секции.
CFLAGS    += -ffunction-sections
# Помещать данные в отдельные секции.
CFLAGS    += -fdata-sections
# Пути поиска заголовочных файлов.
CFLAGS    += $(addprefix -I, $(INCS))
# Макросы.
CFLAGS    += $(addprefix -D, $(DEFINES))

# Флаги компоновщика.
# Флаги МК.
LDFLAGS   += $(MCUFLAGS)
# Конфигурация библиотек.
LDFLAGS   += -specs=nano.specs
# Удаление ненужных секций.
LDFLAGS   += -Wl,--gc-sections
# Без стандартных стартовых файлов.
LDFLAGS   += -nostartfiles
# Генерация карты исполнимого файла.
LDFLAGS   += -Wl,-Map=$(TARGET_MAP),--cref
# Пути поиска библиотек.
LDFLAGS   += $(addprefix -L, $(LIBS_PATH))
# Скрипт линкера.
LDFLAGS   += -T$(LDSCRIPT)
# Библиотеки.
LDLIBS   += $(addprefix -l, $(LIBS))

# Флаги ассемблера.
# Флаги МК.
ASFLAGS   += $(MCUFLAGS)
ASFLAGS   += 

# Тулкит.
TOOLKIT_PREFIX=arm-none-eabi-
AS      = $(TOOLKIT_PREFIX)gcc
CC      = $(TOOLKIT_PREFIX)gcc
LD      = $(TOOLKIT_PREFIX)gcc
OBJCOPY = $(TOOLKIT_PREFIX)objcopy
STRIP   = $(TOOLKIT_PREFIX)strip
SIZE    = $(TOOLKIT_PREFIX)size

# Прочие утилиты.
RM      = rm -f

# Побочные цели.
# Файл прошивки.
TARGET_HEX = $(TARGET).hex
# Файл карты бинарного файла.
TARGET_MAP = $(TARGET).map
# Бинарный файл прошивки.
TARGET_BIN = $(TARGET).bin


all: $(TARGET) size bin hex

strip: $(TARGET)
	$(STRIP) $(TARGET)

size: $(TARGET)
	$(SIZE) -B $(TARGET)

hex: $(TARGET)
	$(OBJCOPY) -O ihex $(TARGET) $(TARGET_HEX)

bin: $(TARGET)
	$(OBJCOPY) -O binary $(TARGET) $(TARGET_BIN)

asm: $(ASM)

#$(TARGET_BIN): all

$(TARGET): $(OBJECTS)
	$(LD) -o $@ $(LDFLAGS) $^ $(LDLIBS)

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<

%.o: %.s
	$(AS) -c -o $@ $(ASFLAGS) $<

%.s: %.c
	$(CC) -S -o $@ $(CFLAGS) $<

clean:
	$(RM) $(OBJECTS)
	$(RM) $(ASM)

clean_all: clean
	$(RM) $(TARGET)
	$(RM) $(TARGET_BIN)
	$(RM) $(TARGET_HEX)
	$(RM) $(TARGET_MAP)

burn_stflash: $(TARGET_BIN)
	$(STFLASH) erase
	$(STFLASH) write $(TARGET_BIN) $(MCU_FLASH_ORIGIN)

burn_win: $(TARGET_BIN)
	$(STFLASH_WIN) -Q -ME
	$(STFLASH_WIN) -Q -P $(TARGET_BIN) $(MCU_FLASH_ORIGIN)
	$(STFLASH_WIN) -Q -Rst

burn_openocd: $(TARGET_BIN)
	$(OPENOCD_CMD) -c "init" -c "reset halt" -c "flash write_image erase $(TARGET_BIN) $(MCU_FLASH_ORIGIN) bin" -c "reset run" -c "exit"

erase_openocd:
	$(OPENOCD_CMD) -c "init" -c "reset halt" -c "flash erase_sector 0 0 last" -c "exit"

burn: burn_openocd

erase: erase_openocd

