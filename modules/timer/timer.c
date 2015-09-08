/**
 * @file timer.c
 *
 * @brief  Timer module
 * @author Kristoffer Ödmark
 * @version 0.1
 * @date 2015-08-27
 */
#include    "timer.h"

#undef DEBUG
#define DEBUG 1

#undef printf
#if DEBUG
// include necesary files for debugging output
#include "hal.h"
#include "chprintf.h"
#include "debug_print.h"
extern BaseSequentialStream debug_print;
#define printf(...) chprintf(&debug_print, __VA_ARGS__)

#else

#endif

static my_timer_t timex;

void init_timex(){
    init_timer(&timex);
}

void start_timex(){
    start_timer(&timex);
}

uint32_t get_timex_us(){
    return get_timer_us(&timex);
}

uint32_t stop_timex(){
    return stop_timer(&timex);
}

void print_timex(){
    uint32_t i = 0;

    printf("Timer values: \n\r");
    printf("nr    : total time : difference \n \r");
    printf("================================================\n\r");
    for(i = 0; i < timex.counter; i++){
        if(i == 0){
            printf("%5u : %10u : %10u \n\r", i, timex.timevector[i],
                    timex.timevector[i]);
        }
        else{
            printf("%5u : %10u : %10u \n\r", i, timex.timevector[i],
                    timex.timevector[i] - timex.timevector[i-1]);
        }
    }
    printf("================================================\n\r");

}

void init_timer(my_timer_t *timer){

    chTMObjectInit(&timer->timer);
    /* clear timevector */
    uint32_t i;
    for(i = 0; i < sizeof(timer->timevector)/sizeof(uint32_t); i++){
        timer->timevector[i] = 0;
    }

    timer->accumulated_time = 0;
    timer->counter = 0;
}

void start_timer(my_timer_t *timer){
    chTMStartMeasurementX(&timer->timer);
    timer->accumulated_time = 0;
    timer->timevector[0] = 0;
    timer->counter =1;
    timer->running = true;
}

uint32_t get_timer_us(my_timer_t *timer){
    if(timer->running == false)
        return 0;

    chTMStopMeasurementX(&timer->timer);
    /* Some macro to extract the time in us*/
    uint32_t dt = RTC2US(STM32_SYSCLK, timer->timer.last);
    /* append time to the accumulated value */
    chTMStartMeasurementX(&timer->timer);
    timer->accumulated_time += dt;
    timer->timevector[timer->counter] = timer->accumulated_time;
    timer->counter++;
    if(timer->counter >= sizeof(timer->timevector)/sizeof(uint32_t)){
        timer->counter = 0;
    }

    return timer->accumulated_time;
}

uint32_t stop_timer(my_timer_t *timer){
    chTMStopMeasurementX(&timer->timer);
    /* Some macro to extract the time */
    uint32_t dt = RTC2US(STM32_SYSCLK, timer->timer.last);
    timer->accumulated_time += dt;
    timer->timevector[timer->counter] = timer->accumulated_time;
    timer->counter++;
    timer->running = false;
    uint32_t returnval = timer->accumulated_time;
    //timer->accumulated_time = 0;
    return returnval;
}
