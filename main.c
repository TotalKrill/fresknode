
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

const EXTConfig extcfg = {
  {
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOC, extcb1},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL},
    {EXT_CH_MODE_DISABLED, NULL}
  }
};

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

void HardFault_Handler(void){
    while(1){
   palTogglePad(GPIOC, GPIOC_LED1);
   palTogglePad(GPIOC, GPIOC_LED2);
   palTogglePad(GPIOC, GPIOC_LED3);

   }
}

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
    //
    //enable interrupt
    extStart(&EXTD1, &extcfg);

    // start thread to be activated by interrupt.
    start_thd();

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

    /* Fulhacket */
    start_chain_range_thd();

    dw1000_receive(&dw);

    if(sender){
        set_ranging_callback(calibration_cb);
    }
    else{
    }

    int per_loop =0;
    uint16_t sleep =2000;
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
