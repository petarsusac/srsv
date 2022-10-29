#include "time_utils.h"

#include <time.h>
#include <pthread.h>
#include <stdio.h>

#define CLOCK CLOCK_MONOTONIC

static pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;
static struct timespec start_time;

void time_utils_init()
{
    clock_gettime(CLOCK, &start_time);
}

void time_utils_print_timestamp(const char *msg)
{
    struct timespec t;
    pthread_mutex_lock(&print_mutex);
    clock_gettime(CLOCK, &t);
    timespec_sub(&t, &start_time);
    unsigned long ms = timespec_to_ms(&t);
    printf("[%lu] %s", ms, msg);
}

unsigned long time_utils_get_time_ms()
{
    struct timespec t;
    clock_gettime(CLOCK, &t);
    timespec_sub(&t, &start_time);
    return timespec_to_ms(&t);
}

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