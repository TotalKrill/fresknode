# Modules directory location from Makefile
MODULE_DIR = ./modules

# Imported source files and paths from modules
include $(MODULE_DIR)/usb/usb.mk

include $(MODULE_DIR)/dbgprint/debugprint.mk

include $(MODULE_DIR)/timer/timer.mk
include $(MODULE_DIR)/uart/uart.mk

# include $(MODULE_DIR)/version_information/version_information.mk

# List of all the module related files.
MODULES_SRC = 	$(USB_SRCS) \
			  	$(TIMER_SRCS) \
			  	$(UARTSRC) \
              	$(VERSIONINFO_SRCS) \
				$(MODULE_DIR)/debug_print/debug_print.c

# Required include directories
MODULES_INC = 	$(USB_INC) \
			  	$(TIMER_INC) \
			  	$(UARTINC) \
              	$(VERSIONINFO_INC) \
				$(MODULE_DIR)/debug_print


