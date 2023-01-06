#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "time_utils.h"
#include "controller.h"
#include "stats.h"

#define LAB3RT (true)
#define TIME_LIMIT (60000U)

static void sigint_handler(int signum)
{
    controller_stop();
    time_utils_print_timestamp("Simulacija je zaustavljena, cekam da sve dretve zavrse");
}

int main(int argc, char **argv)
{
    signal(SIGINT, sigint_handler);

    input_t *inputs[] = {
        // id, period, prva pojava
        input_new(1, 1000, 500),
        input_new(2, 1000, 1000),
        input_new(3, 1000, 500),
        input_new(4, 1000, 1000),
        input_new(5, 2000, 1500),
        input_new(6, 2000, 2000),
    };
    int num_inputs = 6;

    time_utils_init();
    controller_init(inputs, num_inputs, TIME_LIMIT);

    controller_run(LAB3RT);

    print_stats(inputs, num_inputs);

    return 0;
}