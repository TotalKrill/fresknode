#ifndef USB_RELATED_H__
#define USB_RELATED_H__


//#define printf(...) chprintf((BaseSequentialStream *)&SDU1, __VA_ARGS__)

#include "hal.h"
#include "dw1000.h"


#undef DEBUG

#define DEBUG 1
#undef printf
#if DEBUG== 1
// include necesary files for debugging output
#include "chprintf.h"
#include "debug_print.h"
extern BaseSequentialStream debug_print;
extern SerialUSBDriver SDU1;
#define printf(...) chprintf(&debug_print, __VA_ARGS__)

#else

#define printf(...)

#endif


void usbstartup(dw1000_driver_t *driver);


#endif
