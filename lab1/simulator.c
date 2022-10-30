#include "simulator.h"

#include <stdbool.h>
#include "time_utils.h"
#include <stdio.h>

#define SHORT_SLEEP_INTERVAL_MS (5U)

typedef struct _stats_t
{
    unsigned int num_state_changes;
    double average_response_time;
    unsigned int max_response_time;
    unsigned int num_problems;
    unsigned int sum_response_times;
} stats_t;

static stats_t total_stats;

static struct _config
{
    int num_inputs;
    bool simulation_running;
    unsigned int K; // konstanta koja odreduje koliko cesto dolaze promjene stanja
} config;

void simulator_init(int num_inputs, int K)
{
    config.num_inputs = num_inputs;
    config.simulation_running = true;

    total_stats.num_state_changes = 0;
    total_stats.average_response_time = 0;
    total_stats.max_response_time = 0;
    total_stats.num_problems = 0;
    total_stats.sum_response_times = 0;
}

void simulator_run(input_t *input)
{
    if (NULL != input)
    {
        stats_t stats = {
            .num_state_changes = 0,
            .average_response_time = 0,
            .max_response_time = 0,
            .num_problems = 0,
            .sum_response_times = 0,
        };

        // Pricekaj prvu pojavu
        time_utils_delay_for(input_get_first_occurence(input));

        // Pamtiti zadnji odgovor kako bi se znalo je li dosao novi odgovor
        int last_response = 0;

        while(config.simulation_running)
        {
            input_set_state(input, generate_state());
            stats.num_state_changes += 1;

            input_response_t input_response = input_get_response(input);
            while (input_response.response == last_response)
            {
                // Odgovor jos nije stigao
                time_utils_delay_for(SHORT_SLEEP_INTERVAL_MS);
                input_response = input_get_response(input);
            }

            // Odgovor je stigao
            // Izracunaj vrijeme od promjene stanja do odgovora
            unsigned long state_change_timestamp_ms = input_get_state(input).timestamp;
            unsigned long response_timestamp_ms = input_get_response(input).timestamp;
            unsigned long response_time_ms = response_timestamp_ms - state_change_timestamp_ms;

            // Azuriraj statistiku
            if (response_time_ms > input_get_period(input))
            {
                stats.num_problems += 1;
            }

            stats.sum_response_times += response_time_ms;

            if (response_time_ms > stats.max_response_time)
            {
                stats.max_response_time = response_time_ms;
            }

            // Izracunaj vrijeme odgode
            unsigned long delay_until_ms = input_get_state(input).timestamp + input_get_period(input);

            // Ako nije bilo kasnjenja, dodaj dodatnu odgodu
            if (response_time_ms < input_get_period(input))
            {
                delay_until_ms += generate_added_delay(input_get_period(input), config.K);
            }

            time_utils_delay_until(delay_until_ms);
        }
    }
}