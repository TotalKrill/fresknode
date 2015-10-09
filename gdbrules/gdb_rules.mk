
##############################################################################
# Start of GDB section
#

OCD_TARGET = stm32f4x
OCD_INTERFFACE = stlink-v2.cfg
GDB = arm-none-eabi-gdb
OPENOCD = openocd
GDB_FLAGS = -ex "target remote | $(OPENOCD) -c \"gdb_port pipe\" -f interface/$(OCD_INTERFFACE) -f target/$(OCD_TARGET).cfg" -ex "load" -ex "monitor reset halt"

RTOS_FLAGS = -ex "target remote | $(OPENOCD) -c \"gdb_port pipe;" -f interface/$(OCD_INTERFFACE) -f target/$(OCD_TARGET).cfg" -ex "load" -ex "monitor reset halt""

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
