/**
 * @file main.c
 * @brief program entry point
 * @author Kristoffer Ödmark
 * @version 0.9
 * @date 2015-11-02
 */
#include "ch.h"
#include "hal.h"
#include "usb_related.h"
#include "dw1000_hal.h"
#include "dw1000_lld.h"
#include "dw1000_reg.h"
#include "dw1000.h"
#include "exti.h"
#include "dw1000_twowayranging.h"
#include "dw1000_multitwowayranging.h"
#include "timer.h"
#include "eeprom.h"
#include "my_uart.h"
#include "ieee_types.h"

#include "dw1000_peertopeer.h"

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

ieee_euid_t devid;

typedef enum {
    ANCHOR0,
    ANCHOR1,
    ANCHOR2,
    ANCHOR3,
    NODE1,
    NODE2,
    NODE3
}rangerRole;

rangerRole get_role(ieee_euid_t id){
    if(id.u64 == 0xffff0c0412000ba3){
        return ANCHOR0;
    }
    if(id.u64 == 0xffff34fc11000ba3){
        return ANCHOR1;
    }
    if(id.u64 == 0xffff4cee11000ba3){
        return ANCHOR2;
    }
    if(id.u64 == 0xffff5ffc11000ba3){
        return ANCHOR3;
    }
    if(id.u64 == 0xffff120b12000ba3){
        return NODE1;
    }
    if(id.u64 == 0){
        return NODE2;
    }
    if(id.u64 == 0xffff110812000ba3){
        //ok node for test
        return NODE2;
    }

    return NODE3;

}

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
    usbstartup(&dw);

    get_euid();

    //enable debug print thread
    vInitDebugPrint((BaseSequentialStream *) &SDU1);

    chThdSleepMilliseconds(4000);
    printf("dw1000 git descriptor: %s\n\r", DW_GIT);
    palClearPad(GPIOC, GPIOC_DW_RST);
    chThdSleepMilliseconds(200);
    palSetPad(GPIOC, GPIOC_DW_RST);

    //enable interrupt handler
    start_interrupt_handler();

    // start uart
    start_serial();

    ieee_shortaddr_t ieeshortaddr;
    volatile rangerRole role = get_role(devid);
    switch(role){
        case ANCHOR0:
            ieeshortaddr.u16 = 0;
            break;
        case ANCHOR1:
            ieeshortaddr.u16 = 1;
            break;
        case ANCHOR2:
            ieeshortaddr.u16 = 2;
            break;
        case ANCHOR3:
            ieeshortaddr.u16 = 3;
            break;
        case NODE1:
            ieeshortaddr.u16 = 4;
            break;
        case NODE2:
            ieeshortaddr.u16 = 5;
            break;
        default:
            ieeshortaddr.u16 = 5;
            break;

    }

    dw.state = DW1000_STATE_UNINITIALIZED;

    dw1000_conf_t config;

    config.shortaddr = ieeshortaddr;

    dw1000_generate_recommended_conf(
            DW1000_DATARATE_110,
            DW1000_CHANNEL_2,
            devid,
            &config);

    dw.config = &config;

    dw.hal = &default_dw1000_hal;

    dw.ranging_module  = &twowayranging_module;

    dw.packet_module = &peertopeer_module;

    dw1000_init(&dw);

    dw1000_print_config(&dw);

    mranging_targets_payload_t targ = {
        .ranging_id = 1,
        .nr_of_targets = 3,
        .target = {0,1,2,3,4,5,6,7},
    };

    uint8_t data[8] = {'a','b','c','d','e','f','g','h'};

    dw1000_receive(&dw,0 ,0);

    if(role == ANCHOR0){
    //    set_ranging_callback(calibration_cb);
    }
    else if( role == NODE1)
    {
    }
    else if( role == NODE3){
        dw1000_set_antenna_delay(&default_dw1000_hal, 0);
    }

    int per_loop =1;
    uint16_t sleep =100;
    int range_delay = 50;

    ieee_shortaddr_t dst;
    dw1000_sensors_t sensors;
    while(1)
    {
        sensors = dw1000_get_sensors(&dw);
        printf("Sensors: temp = %f, vbat = %f\n\r",
                sensors.temp,
                sensors.vbat);
        (void)targ;
        palTogglePad(GPIOC, GPIOC_LED1);

        if( role == ANCHOR0  ) {
            //dst.u16 = 4;
            chThdSleepMilliseconds(sleep);
        }
        if( role == ANCHOR1  ) {
            //dst.u16 = 4;
            chThdSleepMilliseconds(sleep*2);
        }
        if( role == ANCHOR2  ) {
            //dst.u16 = 4;
            chThdSleepMilliseconds(sleep*3);
        }
        if( role == ANCHOR3  ) {
            //dst.u16 = 4;
            chThdSleepMilliseconds(sleep*4);
        }

        if(role == NODE1 ||
           role == NODE2 ||
           role == NODE3
           ){
            dst.u16 = 0;
            twowayranging_request(&dw, dst);
            chThdSleepMilliseconds(range_delay);
            dst.u16 = 1;
            twowayranging_request(&dw, dst);
            chThdSleepMilliseconds(range_delay);
            dst.u16 = 2;
            twowayranging_request(&dw, dst);
            chThdSleepMilliseconds(range_delay);
            dst.u16 = 3;
            twowayranging_request(&dw, dst);
            chThdSleepMilliseconds(range_delay);

            //peertopeer_send(&dw, dst, &data, 8);
            //peertopeer_controlled_send(&dw, dst, 5,(uint8_t *)&data, 8);
        }
        else
        {
            chThdSleepMilliseconds(100);
            //dw1000_receive(&dw);
        }

        if (per_loop % 100 == 0)
        {
            dw1000_trx_off(&default_dw1000_hal);
            dw1000_softreset_rx(&default_dw1000_hal);
            dw1000_receive(&dw,0,0);
            //dw1000_get_event_counters(&default_dw1000_hal);
            per_loop = 0;

        }
        if (per_loop % 5 == 0 && false)
        {
            dw1000_print_config(&dw);
            chThdSleepMilliseconds(10);
            dw1000_sleep(&dw);
            chThdSleepMilliseconds(5000);
            dw1000_wakeup(&dw);
            chThdSleepMilliseconds(10);
            dw1000_print_config(&dw);
            chThdSleepMilliseconds(10);
            dw1000_receive(&dw,0,0);
            chThdSleepMilliseconds(10);
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
    (void) stacked_r0;
    (void) stacked_r1;
    (void) stacked_r2;
    (void) stacked_r3;
    (void) stacked_r12;
    (void) stacked_lr;
    (void) stacked_pc;
    (void) stacked_psr;
    (void) _CFSR;
    (void) _HFSR;
    (void) _DFSR;
    (void) _AFSR;
    (void) _MMAR;
    (void) _BFAR;
}
