# Drivers directory location from Makefile
DRIVERS_DIR = ./drivers

# Imported source files and paths from drivers
include $(DRIVERS_DIR)/eicu/eicu.mk
include $(DRIVERS_DIR)/dw1000/dw1000.mk

# List of all the related files.
ADRIVERSSRC = $(EICUSRC) $(DW1000SRC)

# Required include directories
ADRIVERSINC = $(EICUINC) $(DW1000INC)
