#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "time_utils.h"


int main(int argc, char **argv)
{
    time_utils_init();

    time_utils_print_timestamp("Pocetak");
    time_utils_simulate_ms(100);
    time_utils_print_timestamp("Kraj");

    return 0;
}