#include <signal.h>
#include <stdio.h>

#include "controller.h"
#include "time_utils.h"

static void sigint_handler(int signum)
{
    controller_stop();
    time_utils_print_timestamp("Simulacija je zaustavljena, cekam da sve dretve zavrse");
}

static void print_stats(input_t **inputs, int num_inputs)
{
    input_stats_t total_stats = {
        .average_response_time = 0.0,
        .max_response_time = 0,
        .num_problems = 0,
        .num_state_changes = 0,
        .sum_response_times = 0,
    };
    
    for (int i = 0; i < num_inputs; i++)
    {
        printf("\n----------- STATISTIKA ULAZ %d -----------\n", input_get_id(inputs[i]));

        // Ispis pojedinacne statistike za ulaz
        input_stats_t *stats = input_get_stats(inputs[i]);

        printf("Broj promjena stanja ulaza: %d\n", stats->num_state_changes);
        printf("Prosjecno vrijeme odgovora: %.2lf ms\n", stats->average_response_time);
        printf("Maksimalno vrijeme odgovora: %d ms\n", stats->max_response_time);
        printf("Broj zakasnjelih odgovora: %d (%.2lf%%)\n", stats->num_problems, ((double) stats->num_problems / stats->num_state_changes) * 100);
    
        // Dodavanje pojedinacne statistike ukupnoj statistici
        total_stats.num_state_changes += stats->num_state_changes;
        total_stats.sum_response_times += stats->sum_response_times;
        total_stats.num_problems += stats->num_problems;

        if (stats->max_response_time > total_stats.max_response_time)
        {
            total_stats.max_response_time = stats->max_response_time;
        }
    }

    // Ispis ukupne statistike
    total_stats.average_response_time = (double) total_stats.sum_response_times / total_stats.num_state_changes;

    printf("\n----------- UKUPNA STATISTIKA -----------\n");
    printf("Broj promjena stanja ulaza: %d\n", total_stats.num_state_changes);
    printf("Prosjecno vrijeme odgovora: %.2lf ms\n", total_stats.average_response_time);
    printf("Maksimalno vrijeme odgovora: %d ms\n", total_stats.max_response_time);
    printf("Broj zakasnjelih odgovora: %d (%.2lf%%)\n", total_stats.num_problems, ((double) total_stats.num_problems / total_stats.num_state_changes) * 100);
}

int main()
{
    signal(SIGINT, sigint_handler);

    input_t *inputs[] = {
        // id, period, prva pojava
        input_new(1, 500, 100),
        input_new(2, 500, 200),
        input_new(3, 500, 300),
        input_new(4, 500, 400),
        input_new(5, 500, 500),
        input_new(6, 500, 600),
        input_new(7, 500, 700),
        input_new(8, 1000, 800),
        input_new(9, 1000, 900),
        input_new(10, 1000, 1000),
        input_new(11, 1000, 1100),
        input_new(12, 1000, 1200),
        input_new(13, 1000, 1300),
        input_new(14, 2000, 1400),
        input_new(15, 2000, 1500),
        input_new(16, 2000, 1600),
        input_new(17, 2000, 1700),
        input_new(18, 5000, 1800),
        input_new(19, 5000, 1900),
        input_new(20, 5000, 2000),
    };
    int num_inputs = 20;

    time_utils_init();
    controller_init(inputs, num_inputs);

    controller_run();

    print_stats(inputs, num_inputs);

    for (int i = 0; i < num_inputs; i++)
    {
        input_delete(inputs[i]);
    }

    return 0;
}