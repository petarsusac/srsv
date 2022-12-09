#include <stdio.h>
#include <signal.h>
#include "periodic_signal.h"

#define INTERRUPT_PERIOD_MS 1000

static void signal_handler(int signum)
{
    printf("SIGINT\n");
}

static void create_periodic_interrupt(void (*interrupt_cb)(int))
{
    signal(SIGINT, interrupt_cb);
    periodic_signal_create(SIGINT, INTERRUPT_PERIOD_MS);
}

int main(void)
{
    time_utils_init();
    create_periodic_interrupt(signal_handler);  

    while(1);
}