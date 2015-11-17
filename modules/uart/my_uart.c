#include "my_uart.h"

#undef printf
#if DEBUG
// include necesary files for debugging output
#include "hal.h"
#include "chprintf.h"
#include "debug_print.h"
extern BaseSequentialStream debug_print;
#define printf(...) chprintf(&debug_print, "twoway: "); \
                    chprintf(&debug_print, __VA_ARGS__)
#else


#define printf(...)

#endif


/* Temporary settings until serial is settable through the USB */
static const SerialConfig aux1_config =
{
  57600,
  0,
  USART_CR2_STOP1_BITS,
  0
};

THD_WORKING_AREA(waAux1DataPumpTask, 128);
static uint32_t ranges[4] = {0,0,0,0};
static bool serial_empty = true;

/**
 * @brief           Transmits the content of the Aux1 circular buffer over Aux1.
 *
* @param[in] arg   Input argument (unused).
 */
static THD_FUNCTION(Aux1DataPumpTask, arg)
{
    (void)arg;

    /* Name for debug */
    chRegSetThreadName("Aux1 Data Pump");

    /* Buffer for transmitting serial Aux1 commands */

    while(1)
    {
        /* Wait for a start transmission event */
        chThdSleepMilliseconds(1);
        if(serial_empty == false)
        {
            chprintf((BaseSequentialStream *)&AUX1_SERIAL_DRIVER, "s%5u,%5u,%5u,%5ue",
                    ranges[0],
                    ranges[1],
                    ranges[2],
                    ranges[3]);

            serial_empty = true;
        }

        /* We will only get here is a request to send data has been received */
    }
}
void serial_write_round_result(dw1000_round_results_t res)
{
    ranges[0] = res.anchor[0].range_mm;
    ranges[1] = res.anchor[1].range_mm;
    ranges[2] = res.anchor[2].range_mm;
    ranges[3] = res.anchor[3].range_mm;
    serial_empty = false;

}
void start_serial(void)
{
    sdStart(&AUX1_SERIAL_DRIVER, &aux1_config );

    chThdCreateStatic(waAux1DataPumpTask,
            sizeof(waAux1DataPumpTask),
            NORMALPRIO,
            Aux1DataPumpTask,
            NULL);
}
