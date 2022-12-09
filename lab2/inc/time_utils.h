#ifndef _TIME_UTILS_H_
#define _TIME_UTILS_H_

#include <time.h>

void time_utils_init();
void time_utils_ms_to_timespec(unsigned long ms, struct timespec *t);
void time_utils_print_timestamp(const char *msg);
unsigned long time_utils_get_time_ms();
void time_utils_delay_until(unsigned long ms);
void time_utils_delay_for(unsigned long ms);

#endif