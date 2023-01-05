#include "time_utils.h"

#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>

#define CLOCK CLOCK_MONOTONIC

static pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
static struct timespec start_time;

static unsigned long num_iterations_10_ms_delay;

static void timespec_add(struct timespec *A, struct timespec *B)
{
    A->tv_sec += B->tv_sec;
    A->tv_nsec += B->tv_nsec;
    if ( A->tv_nsec >= 1000000000 )
    {
        A->tv_sec++;
        A->tv_nsec -= 1000000000;
    }
}

static void timespec_sub(struct timespec *A, struct timespec *B)
{
    A->tv_sec -= B->tv_sec;
    A->tv_nsec -= B->tv_nsec;
    if ( A->tv_nsec < 0 ) {
        A->tv_sec--;
        A->tv_nsec += 1000000000;
    }
}

static unsigned long timespec_to_ms(struct timespec *t)
{
    return t->tv_sec * 1000 + t->tv_nsec / 1000000;
}

static void ms_to_timespec(unsigned long ms, struct timespec *t)
{
    t->tv_sec = ms / 1000;
    t->tv_nsec = (ms % 1000) * 1000000;
}

static void run_for_10_ms()
{
    for (unsigned long i = 0; i < num_iterations_10_ms_delay; i++)
    {
        asm volatile("" ::: "memory");
    }
}

static void calc_num_iterations_10_ms_delay()
{
    num_iterations_10_ms_delay = 1000000;
    bool run = true;
    unsigned long t0;
    unsigned long t1;

    while (run)
    {
        t0 = time_utils_get_time_ms();
        run_for_10_ms();
        t1 = time_utils_get_time_ms();
        if (t1 - t0 >= 10)
        {
            run = false;
        }
        else
        {
            num_iterations_10_ms_delay *= 10;
        }

    }
    
    num_iterations_10_ms_delay = num_iterations_10_ms_delay * (10. / (t1 - t0));
}

void time_utils_init()
{
    clock_gettime(CLOCK, &start_time);
    calc_num_iterations_10_ms_delay();
    
    // Ponovno postavi pocetno vrijeme jer je racunanje iteracija za
    // kasnjenje 10 ms trajalo neko vrijeme, pa da ne bude pomaka
    // na pocetku simulacije
    clock_gettime(CLOCK, &start_time);
}

void time_utils_simulate_ms(int ms)
{
    for (unsigned long i = 0; i < ms / 10; i++)
    {
        run_for_10_ms();
    }
}

void time_utils_print_timestamp(const char *msg)
{
    struct timespec t;
    pthread_mutex_lock(&print_mutex);
    clock_gettime(CLOCK, &t);
    timespec_sub(&t, &start_time);
    unsigned long ms = timespec_to_ms(&t);
    printf("[%lu] %s\n", ms, msg);
    pthread_mutex_unlock(&print_mutex);
}

unsigned long time_utils_get_time_ms()
{
    struct timespec t;
    clock_gettime(CLOCK, &t);
    timespec_sub(&t, &start_time);
    return timespec_to_ms(&t);
}

void time_utils_delay_until(unsigned long ms)
{
    struct timespec t;
    ms_to_timespec(ms, &t);
    timespec_add(&t, &start_time);
    clock_nanosleep(CLOCK, TIMER_ABSTIME, &t, NULL);
}

void time_utils_delay_for(unsigned long ms)
{
    struct timespec t;
    ms_to_timespec(ms, &t);
    clock_nanosleep(CLOCK, 0, &t, NULL);
}
