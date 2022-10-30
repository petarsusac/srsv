#include "controller.h"
#include "time_utils.h"
#include <signal.h>

static void sigint_handler(int signum)
{
    controller_stop();
}

int main()
{
    signal(SIGINT, sigint_handler);

    input_t *inputs[] = {
        input_new(1, 500, 100),
        input_new(2, 100, 50),
    };
    int num_inputs = 2;

    time_utils_init();
    controller_init(inputs, num_inputs);

    controller_run();

    return 0;
}