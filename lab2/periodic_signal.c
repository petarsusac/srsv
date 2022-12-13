#include "periodic_signal.h"

#include <time.h>
#include <signal.h>

#include "time_utils.h"
#include "call.h"

#define CLOCK CLOCK_MONOTONIC

timer_t timer;

void periodic_signal_create(int signum, unsigned long period_ms)
{
    struct sigevent event;
    struct itimerspec period;

    event.sigev_notify = SIGEV_SIGNAL;
    event.sigev_signo = signum;

    CALL(STOP, timer_create, CLOCK, &event, &timer);

    time_utils_ms_to_timespec(period_ms, &period.it_value);
    time_utils_ms_to_timespec(period_ms, &period.it_interval);

    CALL(STOP, timer_settime, timer, 0, &period, NULL);
}

void periodic_signal_disable()
{
    struct itimerspec period;
    period.it_value.tv_sec = 0;
    period.it_value.tv_nsec = 0;

    CALL(STOP, timer_settime, timer, 0, &period, NULL);
}
