
#include "ch.h"
#include "hal.h"
#include "usb_related.h"
#include "dw1000_hal.h"
#include "dw1000_lld.h"
#include "dw1000_reg.h"
#include "dw1000.h"
#include "exti.h"
#include "dw1000_ranging.h"
#include "timer.h"
#include "eeprom.h"
#include "fulhacket.h"


#include "chprintf.h"
#include "debug_print.h"
extern BaseSequentialStream debug_print;
extern SerialUSBDriver SDU1;

#define printf(...) chprintf(&debug_print, __VA_ARGS__)

/**
 * @brief Placeholder for error messages.
 */
volatile assert_errors kfly_assert_errors;

extern dw1000_hal_t default_dw1000_hal;


dw1000_driver_t dw;
uint16_t counter[12];
dw1000_euid_t devid;

void get_euid(void){
    eeprom_init();
    uint8_t uid[8] = {0,0,0,0,0,0,0,0};
    eeprom_read(EEPROM_UID_ADDRESS, (uint8_t *)uid, 8);
    devid.u8[0] = uid[0];
    devid.u8[1] = uid[1];
    devid.u8[2] = uid[2];
    devid.u8[3] = uid[3];
    devid.u8[4] = uid[4];
    devid.u8[5] = uid[5];
    devid.u8[6] = uid[6];
    devid.u8[7] = uid[7];
}

void HardFault_Handler(unsigned long *hardfault_args);

int main(void)
{
    /*
     * System initializations.
     * - HAL initialization, this also initializes the configured
     *   device drivers and performs the board-specific initializations.
     * - Kernel initialization, the main() function becomes a thread
     *   and the RTOS is active.
     */
    halInit();
    chSysInit();

    // enable usb
    usbstartup();

    get_euid();

    //enable debug print thread
    vInitDebugPrint((BaseSequentialStream *) &SDU1);

    palClearPad(GPIOC, GPIOC_DW_RST);
    chThdSleepMilliseconds(20);
    palSetPad(GPIOC, GPIOC_DW_RST);

    //enable interrupt handler
    start_interrupt_handler();

    bool sender = false;
    //if(palReadPad(GPIOC, GPIOC_PIN5) == PAL_HIGH){
    if(true){
        sender = true;
    }

    dw1000_conf_t config;
    rangerRole role = get_role(devid);
    switch(role){
        case ANCHOR0:
            config.shortaddr.u16 = 0;
            break;
        case ANCHOR1:
            config.shortaddr.u16 = 1;
            break;
        case ANCHOR2:
            config.shortaddr.u16 = 2;
            break;
        case ANCHOR3:
            config.shortaddr.u16 = 3;
            break;
        case NODE1:
            config.shortaddr.u16 = 4;
            break;
        case NODE2:
            config.shortaddr.u16 = 5;
            break;
    }


    dw.state = UNINITIALIZED;

    dw1000_generate_recommended_conf(
            &default_dw1000_hal,
            DW1000_DATARATE_850,
            DW1000_CHANNEL_2,
            devid,
            &config);

    dw.config = &config;
    dw1000_init(&dw);

 // dw1000_set_antenna_delay(&default_dw1000_hal, 0);

    /* Fulhacket */
    start_chain_range_thd();

    dw1000_receive(&dw);

    if(role == ANCHOR0){
    //    set_ranging_callback(calibration_cb);
    }
    else{
    }

    int per_loop =0;
    uint16_t sleep =1000;
    while(1)
    {
        //palTogglePad(GPIOC, GPIOC_PIN8);
        chThdSleepMilliseconds(sleep);
        dw1000_shortaddr_t dst;
        printf("euid: 0x%2x%2x%2x%2x%2x%2x%2x%2x \n\r",
                devid.u8[0],
                devid.u8[1],
                devid.u8[2],
                devid.u8[3],
                devid.u8[4],
                devid.u8[5],
                devid.u8[6],
                devid.u8[7]
              );


        if(role == ANCHOR0){
            dst.u16 = 0xFFFF;
            request_ranging(&dw, dst);
        }

        dw1000_get_event_counters(&default_dw1000_hal, counter);
        if (per_loop == 100 && false){   // never run
            per_loop = 0;
             printf("    PHR_ERRORS:    %u \n\r",
                     counter[PHR_ERRORS]);
             printf("    RSD_ERRORS:    %u \n\r",
                     counter[RSD_ERRORS]);
             printf("    FCS_GOOD:      %u \n\r",
                     counter[FCS_GOOD]);
             printf("    FCS_ERRORS:    %u \n\r",
                     counter[FCS_ERRORS]);
             printf("    FILTER_REJ:    %u \n\r",
                     counter[FILTER_REJECTIONS]);
             printf("    RX_OVERRUNS:   %u \n\r",
                     counter[RX_OVERRUNS]);
             printf("    SFD_TO:        %u \n\r",
                     counter[SFD_TIMEOUTS]);
             printf("    PREAMBLE_TO:   %u \n\r",
                     counter[PREAMBLE_TIMEOUTS]);
             printf("    RX_TIMEOUTS:   %u \n\r",
                     counter[RX_TIMEOUTS]);
             printf("    TX_SENT:       %u \n\r",
                     counter[TX_SENT]);
             printf("    HPWARN:        %u \n\r",
                     counter[HALF_PERIOD_WARNINGS]);
             printf("    TX_PWRUP_WARN: %u \n\r",
                     counter[TX_PWRUP_WARNINGS]);
        }
        per_loop++;


    }

}
void HardFault_Handler(unsigned long *hardfault_args){
   /**
 * HardFaultHandler_C:
 * This is called from the HardFault_HandlerAsm with a pointer the Fault stack
 * as the parameter. We can then read the values from the stack and place them
 * into local variables for ease of reading.
 * We then read the various Fault Status and Address Registers to help decode
 * cause of the fault.
 * The function ends with a BKPT instruction to force control back into the debugger
 */
  volatile unsigned long stacked_r0 ;
  volatile unsigned long stacked_r1 ;
  volatile unsigned long stacked_r2 ;
  volatile unsigned long stacked_r3 ;
  volatile unsigned long stacked_r12 ;
  volatile unsigned long stacked_lr ;
  volatile unsigned long stacked_pc ;
  volatile unsigned long stacked_psr ;
  volatile unsigned long _CFSR ;
  volatile unsigned long _HFSR ;
  volatile unsigned long _DFSR ;
  volatile unsigned long _AFSR ;
  volatile unsigned long _BFAR ;
  volatile unsigned long _MMAR ;

  stacked_r0 = ((unsigned long)hardfault_args[0]) ;
  stacked_r1 = ((unsigned long)hardfault_args[1]) ;
  stacked_r2 = ((unsigned long)hardfault_args[2]) ;
  stacked_r3 = ((unsigned long)hardfault_args[3]) ;
  stacked_r12 = ((unsigned long)hardfault_args[4]) ;
  stacked_lr = ((unsigned long)hardfault_args[5]) ;
  stacked_pc = ((unsigned long)hardfault_args[6]) ;
  stacked_psr = ((unsigned long)hardfault_args[7]) ;

  // Configurable Fault Status Register
  // Consists of MMSR, BFSR and UFSR
  _CFSR = (*((volatile unsigned long *)(0xE000ED28))) ;

  // Hard Fault Status Register
  _HFSR = (*((volatile unsigned long *)(0xE000ED2C))) ;

  // Debug Fault Status Register
  _DFSR = (*((volatile unsigned long *)(0xE000ED30))) ;

  // Auxiliary Fault Status Register
  _AFSR = (*((volatile unsigned long *)(0xE000ED3C))) ;

  // Read the Fault Address Registers. These may not contain valid values.
  // Check BFARVALID/MMARVALID to see if they are valid values
  // MemManage Fault Address Register
  _MMAR = (*((volatile unsigned long *)(0xE000ED34))) ;
  // Bus Fault Address Register
  _BFAR = (*((volatile unsigned long *)(0xE000ED38))) ;

  __asm("BKPT #0\n") ; // Break into the debugger
    while(1){
   palTogglePad(GPIOC, GPIOC_LED1);
   palTogglePad(GPIOC, GPIOC_LED2);
   palTogglePad(GPIOC, GPIOC_LED3);

   }
}
