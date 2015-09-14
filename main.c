
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
    eeprom_init();
    uint8_t uid[6] = {0,0,0,0,0,0};

    eeprom_read(EEPROM_UID_ADDRESS, (uint8_t *)uid, 6);

    //enable debug print thread
    vInitDebugPrint((BaseSequentialStream *) &SDU1);

    palClearPad(GPIOC, GPIOC_DW_RST);
    chThdSleepMilliseconds(20000);
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

    uint8_t rxbuf[4];
    dw1000_conf_t config;
    if(sender){
        config.shortaddr.u16 = 0xFA77;
    }
    else{
        config.shortaddr.u16 = 0xBEEF;
        //config.shortaddr.u16 = 0xBEFF;
    }


    dw.state = UNINITIALIZED;
    dw1000_euid_t euid;
    euid.u64 = 1;

    dw1000_generate_recommended_conf(
            &default_dw1000_hal,
            DW1000_DATARATE_850,
            DW1000_CHANNEL_2,
            euid,
            &config);

    dw.config = &config;

    dw1000_init(&dw);

    uint32_t zero = \
                    DW1000_EVENT_TXFRS | \
                    DW1000_EVENT_RXDFR | \
                    DW1000_EVENT_AFFREJ | \
                    DW1000_EVENT_RXFCE | \
                    DW1000_EVENT_RXPHE | \
                    DW1000_EVENT_LDEERR | \
                    DW1000_EVENT_RXRFSL | \
                    DW1000_EVENT_RXSFDTO | \
                    DW1000_EVENT_HPDWARN | \
                    DW1000_EVENT_TXBERR \
                    ;
    dw1000_set_interrupts(&default_dw1000_hal,zero);

    dw1000_receive(&dw);

    if(sender){
        set_ranging_callback(calibration_cb);
        //palSetPad(GPIOC, GPIOC_PIN8);
    }
    else{
        //palClearPad(GPIOC, GPIOC_PIN8);
    }

    int per_loop =0;
    uint8_t sleep =2000;
    while(1)
    {
        //palTogglePad(GPIOC, GPIOC_PIN8);
        chThdSleepMilliseconds(sleep);
        dw1000_shortaddr_t dst;
        printf("uid: 0x%2x,0x%2x, 0x%2x, 0x%2x, 0x%2x, 0x%2x \n\r",
                uid[0],
                uid[1],
                uid[2],
                uid[3],
                uid[4],
                uid[5]);

        if(sender){
            dst.u16 = 0xBEEF;
            request_ranging(&dw, dst);
            chThdSleepMilliseconds(sleep);
            dst.u16 =0xBEFF;
            request_ranging(&dw, dst);
        }

        dw1000_get_event_counters(&default_dw1000_hal, counter);
        if (per_loop == 100){
            per_loop = 0;
           // printf("    PHR_ERRORS:    %u \n\r", counter[PHR_ERRORS]);
           // printf("    RSD_ERRORS:    %u \n\r", counter[RSD_ERRORS]);
           // printf("    FCS_GOOD:      %u \n\r", counter[FCS_GOOD]);
           // printf("    FCS_ERRORS:    %u \n\r", counter[FCS_ERRORS]);
           // printf("    FILTER_REJ:    %u \n\r", counter[FILTER_REJECTIONS]);
           // printf("    RX_OVERRUNS:   %u \n\r", counter[RX_OVERRUNS]);
           // printf("    SFD_TO:        %u \n\r", counter[SFD_TIMEOUTS]);
           // printf("    PREAMBLE_TO:   %u \n\r", counter[PREAMBLE_TIMEOUTS]);
           // printf("    RX_TIMEOUTS:   %u \n\r", counter[RX_TIMEOUTS]);
           // printf("    TX_SENT:       %u \n\r", counter[TX_SENT]);
           // printf("    HPWARN:        %u \n\r", counter[HALF_PERIOD_WARNINGS]);
           // printf("    TX_PWRUP_WARN: %u \n\r", counter[TX_PWRUP_WARNINGS]);
        }
        per_loop++;


    }

}
