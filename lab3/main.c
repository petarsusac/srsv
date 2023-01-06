#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "time_utils.h"
#include "controller.h"

#define LAB3RT true


int main(int argc, char **argv)
{

    input_t *inputs[] = {
        // id, period, prva pojava
        input_new(1, 1000, 500),
        input_new(2, 2000, 1000),
    };
    int num_inputs = 2;

    time_utils_init();
    controller_init(inputs, num_inputs, 10000);

    controller_run(LAB3RT);

    return 0;
}