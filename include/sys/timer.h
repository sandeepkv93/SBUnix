#ifndef _TIMER_H
#define _TIMER_H

void timer_setup();
void timer_isr();
void timer_sleep(uint32_t);

typedef struct
{
    uint32_t seconds;
    uint32_t milliseconds;
    uint32_t counter;
} s_time;
#endif
