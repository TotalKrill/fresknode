/**
 * @file timer.h
 * @brief  Header for timer functions
 * @author Kristoffer Ödmark
 * @version 0.1
 * @date 2015-08-27
 */
#ifndef TIMER_H_
#define TIMER_H_
#include    "ch.h"
#include    "hal.h"
#include    "stdint.h"

typedef struct{
        time_measurement_t timer;
        uint32_t accumulated_time;
        uint32_t timevector[128];
        uint8_t counter;
        bool running;
}my_timer_t;

/**
 * @brief  Inititate the timer module.
 *
 * @param timer
 */
void init_timer(my_timer_t *timer);

/**
 * @brief  Start timer.
 *
 * @param timer
 */
void start_timer(my_timer_t *timer);


/**
 * @brief  Get time from when timer was started.
 * This will not stop the timer. Just get a time in us from when it stared.
 *
 * @param timer
 *
 * @returns time from start in us.
 */
uint32_t get_timer_us(my_timer_t *timer);


/**
 * @brief Stops the timer and returns the time.
 *
 * @param timer
 *
 * @returns time from start in us.
 */
uint32_t stop_timer(my_timer_t *timer);

/* static version of functions. */
void init_timex(void);
void start_timex(void);
uint32_t get_timex_us(void);
uint32_t stop_timex(void);
void print_timex(void);
#endif
