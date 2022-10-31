#include "controller.h"
#include "time_utils.h"
#include <signal.h>

static void sigint_handler(int signum)
{
    controller_stop();
}

static void print_stats(input_t **inputs, int num_inputs)
{
    for (int i = 0; i < num_inputs; i++)
    {
        printf("\n----------- STATISTIKA ULAZ %d -----------\n", input_get_id(inputs[i]));

        input_stats_t *stats = input_get_stats(inputs[i]);

        printf("Broj promjena stanja ulaza: %d\n", stats->num_state_changes);
        printf("Prosjecno vrijeme odgovora: %.2lf ms\n", stats->average_response_time);
        printf("Maksimalno vrijeme odgovora: %d ms\n", stats->max_response_time);
        printf("Broj zakasnjelih odgovora: %d (%.2lf%%)\n", stats->num_problems, ((double) stats->num_problems / stats->num_state_changes) * 100);
    }
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

    print_stats(inputs, num_inputs);

    return 0;
}