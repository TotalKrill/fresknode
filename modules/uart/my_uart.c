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
  38400,
  0,
  USART_CR2_STOP1_BITS,
  0
};

THD_WORKING_AREA(waAux1DataPumpTask, 128);
static uint32_t ranges[4] = {0,0,0,0};
struct range_info
{
    uint16_t node;
    uint32_t range;
    uint32_t rssi;
};

static struct range_info range_data;

typedef enum {
    SERIAL_NONE,
    SERIAL_ROUND_RESULTS,
    SERIAL_RANGE_RESULT,
} serial_type_t;
static serial_type_t serial_data = SERIAL_NONE;

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
        switch(serial_data)
        {
            case SERIAL_ROUND_RESULTS:
                chprintf((BaseSequentialStream *)&AUX1_SERIAL_DRIVER, "s%5u,%5u,%5u,%5ue",
                    ranges[0],
                    ranges[1],
                    ranges[2],
                    ranges[3]);
                serial_data = SERIAL_NONE;
                break;
            case SERIAL_RANGE_RESULT:
                chprintf((BaseSequentialStream *)&AUX1_SERIAL_DRIVER, "s%6u,%6u,%6ue\n",
                        range_data.node,
                        range_data.range,
                        range_data.rssi);
                serial_data = SERIAL_NONE;
            case SERIAL_NONE:
            default:
                break;

        }

        /* We will only get here is a request to send data has been received */
    }
}

void serial_write_range(uint16_t node, uint32_t range, uint32_t rssi)
{
   range_data.node = node;
   range_data.range = range;
   range_data.rssi = rssi;
   serial_data = SERIAL_RANGE_RESULT;
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
