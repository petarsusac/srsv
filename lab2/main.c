#include <stdio.h>

#include "time_utils.h"
#include "controller.h"

int main(void)
{

    input_t *inputs[] = {
        // id, period, prva pojava
        input_new(1, 500, 0),
        input_new(2, 1000, 100),
    };

    time_utils_init();
    controller_init(inputs, 2);  

    controller_start();

    return 0;
}