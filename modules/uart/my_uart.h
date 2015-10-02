/**
 * @file my_uart.h
 * @brief uart communication headers
 * @author Kristoffer Ödmark
 * @version 0.1
 * @date 2015-09-25
 */

#ifndef MY_UART_H_
#define MY_UART_H_

#include "ch.h"
#include "hal.h"
#include "fulhacket.h"

#define AUX1_SERIAL_DRIVER SD3

void start_serial(void);
void serial_write_round_result(dw1000_round_results_t res);

#endif
