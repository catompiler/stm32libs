# Основная цель.
TARGET    = main

# Объектные файлы.
OBJECTS   = main.o

# Собственные библиотеки в исходниках.
SRC_LIBS  = circular_buffer usart_buf newlib_stubs

# Макросы.
DEFINES  += 

# Библиотеки.
LIBS      = c

# Оптимизация, вторая часть флага компилятора -O.
OPTIMIZE  = s

# Флаги отладки.
DEBUG    += 

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
 STD_PERIPH_DRIVERS += stm32f10x_gpio
# STD_PERIPH_DRIVERS += stm32f10x_i2c
# STD_PERIPH_DRIVERS += stm32f10x_iwdg
# STD_PERIPH_DRIVERS += stm32f10x_pwr
 STD_PERIPH_DRIVERS += stm32f10x_rcc
# STD_PERIPH_DRIVERS += stm32f10x_rtc
# STD_PERIPH_DRIVERS += stm32f10x_sdio
# STD_PERIPH_DRIVERS += stm32f10x_spi
# STD_PERIPH_DRIVERS += stm32f10x_tim
 STD_PERIPH_DRIVERS += stm32f10x_usart
# STD_PERIPH_DRIVERS += stm32f10x_wwdg

# FreeRTOS.
# Исходники FreeRTOS.
FREERTOS_SRC += 
# Реализация менеджера памяти.
FREERTOS_HEAP = heap_1

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

# Каталог сборки.
BUILD_DIR       = ./build

# Путь к библиотекам от производителя (SPL, CMSIS).
STM32_LIBS      = ../stm32

# Путь к стандартной библиотеке периферии.
SPL_PATH        = StdPeriphLib/STM32F10x_StdPeriph_Lib_V3.5.0/Libraries
# Путь к стандартной библиотеке драйверов периферии.
STD_PERIPH_PATH = $(SPL_PATH)/STM32F10x_StdPeriph_Driver
# Путь к исходникам драйверов периферии.
STD_PERIPH_SRC  = $(STD_PERIPH_PATH)/src
# Путь к заголовочным файлам драйверов периферии.
STD_PERIPH_INC  = $(STM32_LIBS)/$(STD_PERIPH_PATH)/inc
# Объектные файлы SPL.
STD_PERIPH_OBJS = $(addprefix $(STD_PERIPH_SRC)/,$(addsuffix .o, $(STD_PERIPH_DRIVERS)))

# Путь к CMSIS.
CMSIS_PATH                = CMSIS
# Путь к библиотеке ядра.
CMSIS_CORE_SUPPORT_PATH   = $(CMSIS_PATH)/CMSIS
# Заголовки библиотеки ядра.
CMSIS_CORE_SUPPORT_INC    = $(STM32_LIBS)/$(CMSIS_CORE_SUPPORT_PATH)/Include
# Исходники библиотеки ядра.
CMSIS_CORE_SUPPORT_SRC    = 
# Путь к библиотекам ядра.
CMSIS_CORE_SUPPORT_LIBS_PATH = $(STM32_LIBS)/$(CMSIS_CORE_SUPPORT_PATH)/Lib/GCC
# Библиотека математики.
CMSIS_MATH_LIB            = arm_cortexM3l_math
# Библиотеки ядра.
CMSIS_CORE_SUPPORT_LIBS  += $(CMSIS_MATH_LIB)
# Файлы исходного кода библиотеки ядра.
CMSIS_CORE_SRC           += 
# Объектные файлы библеотеки ядра.
CMSIS_CORE_SUPPORT_OBJS  += $(addprefix $(CMSIS_CORE_SUPPORT_SRC)/, $(patsubst %.c, %.o, $(CMSIS_CORE_SRC)))

# Путь к библиотеке CMSIS в составе SPL.
SPL_CMSIS_PATH            = $(SPL_PATH)/CMSIS/CM3
# Путь к библиотеке устройства.
CMSIS_DEVICE_SUPPORT_PATH = $(SPL_CMSIS_PATH)/DeviceSupport/ST/STM32F10x
# Путь к заголовкам библиотеки устройства.
CMSIS_DEVICE_SUPPORT_INC  = $(STM32_LIBS)/$(CMSIS_DEVICE_SUPPORT_PATH)
# Путь к исходникам библиотеки устройства.
CMSIS_DEVICE_SUPPORT_SRC  = $(CMSIS_DEVICE_SUPPORT_PATH)
# Путь к стартовым файлам.
CMSIS_STARTUP_PATH        = $(CMSIS_DEVICE_SUPPORT_PATH)/startup/gcc_ride7
# Стартовый файл CMSIS.
CMSIS_STARTUP_SRC         = startup_stm32f10x_$(MCU_FAMILY).s
# Системный файл CMSIS.
CMSIS_SYSTEM_SRC          = system_stm32f10x.c
# Объектные файлы библиотеки устройства.
CMSIS_DEVICE_SUPPORT_OBJS += $(addprefix $(CMSIS_STARTUP_PATH)/, $(patsubst %.s, %.o, $(CMSIS_STARTUP_SRC)))
CMSIS_DEVICE_SUPPORT_OBJS += $(addprefix $(CMSIS_DEVICE_SUPPORT_SRC)/, $(patsubst %.c, %.o, $(CMSIS_SYSTEM_SRC)))

# FreeRTOS.
# Путь к FreeRTOS.
FREERTOS_PATH             = FreeRTOS
# Путь к общему исходному коду.
FREERTOS_SRC_PATH         = $(FREERTOS_PATH)/Source
# Путь к заголовочным файлам.
FREERTOS_INC_PATH         = $(STM32_LIBS)/$(FREERTOS_SRC_PATH)/include
# Путь к порту FreeRTOS.
FREERTOS_PORT_PATH        = $(FREERTOS_SRC_PATH)/portable/GCC/ARM_CM3
# Путь к исходникам порта FreeRTOS.
FREERTOS_PORT_SRC_PATH    = $(FREERTOS_PORT_PATH)
# Путь к заголовкам порта FreeRTOS.
FREERTOS_PORT_INC_PATH    = $(STM32_LIBS)/$(FREERTOS_PORT_PATH)
# Путь к исходникам реализаций выделения памяти.
FREERTOS_MEMMANG_SRC_PATH = $(FREERTOS_SRC_PATH)/portable/MemMang
# Базовые элементы ядра FreeRTOS.
FREERTOS_SRC             += tasks queue list
# Полный путь к базовым элементам ядра FreeRTOS.
FREERTOS_SRC_FULL         = $(addprefix $(FREERTOS_SRC_PATH)/, $(FREERTOS_SRC))
# Исходный код порта FreeRTOS.
FREERTOS_PORT_SRC         = port
# Полный путь к порту FreeRTOS.
FREERTOS_PORT_SRC_FULL    = $(addprefix $(FREERTOS_PORT_SRC_PATH)/, $(FREERTOS_PORT_SRC))
# Полный путь к менеджеру памяти FreeRTOS.
FREERTOS_MEMMANG_SRC_FULL = $(addprefix $(FREERTOS_MEMMANG_SRC_PATH)/, $(FREERTOS_HEAP))
# Полный исходный код FreeRTOS.
FREERTOS_FULL_SRC        += $(FREERTOS_SRC_FULL)
FREERTOS_FULL_SRC        += $(FREERTOS_PORT_SRC_FULL)
FREERTOS_FULL_SRC        += $(FREERTOS_MEMMANG_SRC_FULL)
# Объектные файлы FreeRTOS.
FREERTOS_OBJS             = $(addsuffix .o, $(FREERTOS_FULL_SRC))

# Путь к собственным библиотекам в исходниках.
SRC_LIBS_PATH     += ../lib
# Пути ко всем собственным библиотекам в исходниках.
SRC_LIBS_ALL_PATH += $(dir $(wildcard $(addsuffix /*/., $(SRC_LIBS_PATH))))
# Объектные файлы собственных библиотек.
SRC_LIBS_OBJS      = $(addsuffix .o, $(SRC_LIBS))

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
INCS     += $(FREERTOS_INC_PATH)
INCS     += $(FREERTOS_PORT_INC_PATH)

# Библиотеки.
# Библиотеки CMSIS.
LIBS     += $(CMSIS_CORE_SUPPORT_LIBS)

# Пути с исходниками.
VPATH   += .
VPATH   += $(STM32_LIBS)
VPATH   += $(SRC_LIBS_ALL_PATH)

# Объектные файлы.
# CMSIS поддержка ядра.
OBJECTS += $(CMSIS_CORE_SUPPORT_OBJS)
# CMSIS поддержка устройства.
OBJECTS += $(CMSIS_DEVICE_SUPPORT_OBJS)
# Драйвера периферии.
OBJECTS += $(STD_PERIPH_OBJS)
# Собственные библиотеки в исходниках.
OBJECTS += $(SRC_LIBS_OBJS)
# Исходный код FreeRTOS.
OBJECTS  += $(FREERTOS_OBJS)

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
LDFLAGS   += -Wl,-Map=$(BUILD_TARGET_MAP),--cref
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
MKDIR   = mkdir -p
RMDIR   = rmdir --ignore-fail-on-non-empty
RMDIR_P = rmdir -p --ignore-fail-on-non-empty
RM_DIR  = rm -fr

# Переменные сборки.
# Цель сборки.
BUILD_TARGET = $(BUILD_DIR)/$(TARGET)
# Файл прошивки.
BUILD_TARGET_HEX = $(BUILD_TARGET).hex
# Файл карты бинарного файла.
BUILD_TARGET_MAP = $(BUILD_TARGET).map
# Бинарный файл прошивки.
BUILD_TARGET_BIN = $(BUILD_TARGET).bin
# Каталоги объектных файлов.
OBJECTS_DIRS     = $(sort $(filter-out ./, $(dir $(OBJECTS))))
# Подкаталоги объектных файлов.
OBJECTS_SUBDIRS  = $(sort $(foreach DIR, $(OBJECTS_DIRS), $(shell expr $(DIR) : "\\([^/]*\\)")))
# Каталоги сборки объектных файлов.
BUILD_OBJECTS_DIRS = $(addprefix $(BUILD_DIR)/, $(OBJECTS_DIRS))
# Подкаталоги сборки.
BUILD_OBJECTS_SUBDIRS = $(addprefix $(BUILD_DIR)/, $(OBJECTS_SUBDIRS))
# Объектные файлы сборки.
BUILD_OBJECTS      = $(addprefix $(BUILD_DIR)/, $(OBJECTS))
# Ассемблерные файлы сборки.
BUILD_ASM          = $(addprefix $(BUILD_DIR)/, $(ASM))



.PHONY: all strip size hex bin asm dirs clean clean_all\
	burn_stflash burn_win burn_openocd erase_openocd burn erase



all: dirs $(BUILD_TARGET) size bin hex

strip: dirs $(BUILD_TARGET)
	$(STRIP) $(TARGET)

size: dirs $(BUILD_TARGET)
	$(SIZE) -B $(BUILD_TARGET)

hex: dirs $(BUILD_TARGET_HEX)

bin: dirs $(BUILD_TARGET_BIN)

asm: dirs $(BUILD_ASM)

dirs: $(BUILD_DIR)

$(BUILD_DIR): $(BUILD_OBJECTS_DIRS)
	$(MKDIR) $@

$(BUILD_OBJECTS_DIRS):
	$(MKDIR) $@

$(BUILD_TARGET): $(BUILD_OBJECTS)
	$(LD) -o $@ $(LDFLAGS) $^ $(LDLIBS)

$(BUILD_TARGET_HEX): $(BUILD_TARGET)
	$(OBJCOPY) -O ihex $^ $@

$(BUILD_TARGET_BIN): $(BUILD_TARGET)
	$(OBJCOPY) -O binary $^ $@

$(BUILD_DIR)/%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<

$(BUILD_DIR)/%.o: %.s
	$(AS) -c -o $@ $(ASFLAGS) $<

$(BUILD_DIR)/%.s: %.c
	$(CC) -S -o $@ $(CFLAGS) $<

clean:
	$(RM) $(BUILD_OBJECTS)

clean_asm:
	$(RM) $(BUILD_ASM)

clean_dirs:
	$(RM_DIR) $(BUILD_OBJECTS_SUBDIRS)

clean_all: clean clean_asm clean_dirs
	$(RM) $(BUILD_TARGET)
	$(RM) $(BUILD_TARGET_BIN)
	$(RM) $(BUILD_TARGET_HEX)
	$(RM) $(BUILD_TARGET_MAP)
	$(RM_DIR) $(BUILD_DIR)

burn_stflash: $(TARGET_BIN)
	$(STFLASH) erase
	$(STFLASH) write $(BUILD_TARGET_BIN) $(MCU_FLASH_ORIGIN)

burn_win: $(TARGET_BIN)
	$(STFLASH_WIN) -Q -ME
	$(STFLASH_WIN) -Q -P $(BUILD_TARGET_BIN) $(MCU_FLASH_ORIGIN)
	$(STFLASH_WIN) -Q -Rst

burn_openocd: $(TARGET_BIN)
	$(OPENOCD_CMD) -c "init" -c "reset halt" -c "flash write_image erase $(BUILD_TARGET_BIN) $(MCU_FLASH_ORIGIN) bin" -c "reset run" -c "exit"

erase_openocd:
	$(OPENOCD_CMD) -c "init" -c "reset halt" -c "flash erase_sector 0 0 last" -c "exit"

burn: burn_openocd

erase: erase_openocd

