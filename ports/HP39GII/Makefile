# Include the core environment definitions; this will set $(TOP).
include ../../py/mkenv.mk

# Include py core make definitions.
include $(TOP)/py/py.mk

# Set makefile-level MicroPython feature configurations.
MICROPY_ROM_TEXT_COMPRESSION ?= 1

# Define toolchain and other tools.
CROSS_COMPILE ?= arm-none-eabi-
ELF2SB ?= ./elftosb.exe

# Set CFLAGS.
CFLAGS += -I. -I$(TOP) -I$(BUILD)
CFLAGS += -I./stmp3770
CFLAGS += -I./stmp3770/registers
CFLAGS += -I./stmp3770/hardware
CFLAGS += -I./TinyUSB
CFLAGS += -I./TinyUSB/class/msc

#CFLAGS += -I../../lib/littlefs
CFLAGS += -I./dhara
CFLAGS += -I../../extmod/

CFLAGS += -I./FreeRTOS/include
CFLAGS += -I./modules

CFLAGS += -Wall -Werror -std=gnu2x -nostdlib
CFLAGS += -mtune=arm926ej-s -mcpu=arm926ej-s -msoft-float -mfloat-abi=soft
CSUPEROPT = -Os # save some code space for performance-critical code

# Select debugging or optimisation build.
#ifeq ($(DEBUG), 1)
#CFLAGS += -Og
#else
CFLAGS += -Os -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
#endif

# Set linker flags.
LDFLAGS += -nostdlib -T stmp3770.ld --gc-sections 
# Define the required source files.
SRC_C += lib.c main.c system.c
SRC_C += ../../lib/libc/string0.c

SRC_C += usb_task.c usb_descriptors.c usb_msc.c usb_cdc.c
SRC_C += ./TinyUSB/class/cdc/cdc_device.c ./TinyUSB/class/msc/msc_device.c
SRC_C += ./flash.c

SRC_C += ./mphalport.c

#SRC_C += $(wildcard ./*.c )

SRC_C +=    lib/mp-readline/readline.c \
			lib/utils/gchelper_generic.c \
			lib/utils/pyexec.c \
			lib/utils/stdout_helpers.c \


CFLAGS += -DFFCONF_H=\"ports/HP39GII/ffconf.h\"

SRC_C += ./fatfs_port.c
SRC_C += $(addprefix lib/,\
        oofatfs/ff.c \
        oofatfs/ffunicode.c \
		timeutils/timeutils.c \
        )
		
SRC_C += $(addprefix extmod/,\
        vfs.c \
        vfs_fat.c \
        vfs_fat_file.c \
        vfs_fat_diskio.c \
		vfs_reader.c \
        )		

SRC_MODS += $(addprefix ./modules/,\
        machine.c \
		machine_flash.c \
		moduos.c \
        )
		
SRC_EXTMOD += $(addprefix extmod/,\
        moduhashlib.c \
		moduheapq.c \
		moductypes.c \
		moduasyncio.c \
		modurandom.c \
        )



SRC_QSTR += $(SRC_MODS) $(SRC_EXTMOD)

#SRC_C +=    $(wildcard ../../lib/littlefs/*.c)
SRC_C += $(wildcard ./dhara/*.c)

SRC_C += $(wildcard ./stmp3770/hardware/*.c)
SRC_C += $(wildcard ./TinyUSB/*.c)
SRC_C += $(wildcard ./TinyUSB/common/*.c)
SRC_C += $(wildcard ./TinyUSB/device/*.c)

SRC_C += $(wildcard ./FreeRTOS/*.c )
SRC_C += $(wildcard ./FreeRTOS/porting/*.c )

SRC_C += $(wildcard ./oofatfs/*.c )

SRC_C += $(addprefix lib/,\
	libm/math.c \
	libm/fmodf.c \
	libm/nearbyintf.c \
	libm/ef_sqrt.c \
	libm/kf_rem_pio2.c \
	libm/kf_sin.c \
	libm/kf_cos.c \
	libm/kf_tan.c \
	libm/ef_rem_pio2.c \
	libm/sf_sin.c \
	libm/sf_cos.c \
	libm/sf_tan.c \
	libm/sf_frexp.c \
	libm/sf_modf.c \
	libm/sf_ldexp.c \
	libm/asinfacosf.c \
	libm/atanf.c \
	libm/atan2f.c \
	libm/roundf.c \
	)

# Define the required object files.

OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_EXTMOD:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_MODS:.c=.o))


OBJ += $(PY_CORE_O)


LIBS += ./lib/libc/libc
LIBS += ./lib/libc/libgcc
# Define the top-level target, the main firmware.
all: $(BUILD)/firmware.sb

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	#$(CC) -o $@ $(LDFLAGS) $(OBJ) $(LIBS)
	$(Q)$(SIZE) $@

# $(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	# $(ECHO) "Create $@"
	# $(Q)$(OBJCOPY) -O binary -j .isr_vector -j .text -j .data $^ $@

$(BUILD)/firmware.sb: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(ELF2SB) -z -c ./build_fw.bd -o $(BUILD)/firmware.sb $(BUILD)/firmware.elf
#$(Q)$(PYTHON) $(DFU) -b 0x08000000:$^ $@

update: $(BUILD)/firmware.sb
	$(Q)$(PYTHON) $(PYDFU) -u $^

# Include remaining core make rules.
include $(TOP)/py/mkrules.mk
