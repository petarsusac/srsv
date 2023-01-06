#include "stats.h"

#include <stdio.h>

void print_stats(input_t **inputs, int num_inputs)
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