
##############################################################################
# Start of GDB section
#

OCD_TARGET = stm32f4x
OCD_INTERFFACE = stlink-v2.cfg
GDB = arm-none-eabi-gdb
OPENOCD = openocd
RTOS_AUTO_FULE = gdbrules/rtos_auto.cfg
GDB_FLAGS = -ex "target remote | $(OPENOCD) -c \"gdb_port pipe\" -f interface/$(OCD_INTERFFACE) -f target/$(OCD_TARGET).cfg -f $(RTOS_AUTO_FULE)"

GDB_FLAGS += -ex "load"
GDB_FLAGS += -ex "monitor reset halt"

LAST_SHELL=$(shell ls /dev/pts/ -1c --sort=time | head -n1)
NEW_SHELL=$(shell (xterm &); sleep 1;ls /dev/pts/ -1c --sort=time | head -n1)

DASHBOARD += -ex "dashboard -output /dev/pts/$(LAST_SHELL)"
gdb: build/$(PROJECT).elf
	$(GDB) build/$(PROJECT).elf $(GDB_FLAGS)

rtos: build/$(PROJECT).elf
	$(GDB) build/$(PROJECT).elf $(RTOS_FLAGS)

flash_all:
	$(GDB) build/$(PROJECT).elf -x gdbrules/flash_all.gdb

rpi1:
	$(GDB) build/$(PROJECT).elf -x gdbrules/rpi1.gdb
rpi2:
	$(GDB) build/$(PROJECT).elf -x gdbrules/rpi2.gdb
rpi3:
	$(GDB) build/$(PROJECT).elf -x gdbrules/rpi3.gdb
