#include "simulator.h"

#include <stdbool.h>
#include "time_utils.h"
#include "input.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define SHORT_SLEEP_INTERVAL_MS (5U)
#define STATE_MIN (100U)
#define STATE_MAX (999U)
#define LOG_MESSAGE_LENGTH (100U)

static struct _config
{
    int num_inputs;
    bool simulation_running;
    unsigned int K; // konstanta koja odreduje koliko cesto dolaze promjene stanja
} config;

static int generate_state()
{
    return rand() % (STATE_MAX - STATE_MIN + 1) + STATE_MIN;
}

static unsigned int generate_added_delay(unsigned int period, unsigned int K)
{
    int rand_int = rand() % (K - 1);
    return rand_int * period;
}

void simulator_init(int num_inputs, int K)
{
    config.num_inputs = num_inputs;
    config.simulation_running = true;
    config.K = K;
}

void *simulator_run(void *input)
{
    if (NULL != input)
    {
        input_t *p_input = (input_t *) input;

        input_stats_t *stats = input_get_stats(p_input);

        // Pricekaj prvu pojavu
        time_utils_delay_for(input_get_first_occurence(p_input));

        // Pamtiti zadnji odgovor kako bi se znalo je li dosao novi odgovor
        int last_response = 0;

        char msg[LOG_MESSAGE_LENGTH];
        
        while(config.simulation_running)
        {
            int new_state = generate_state();

            sprintf(msg, "Dretva %d: promjena stanja ulaza (nova vrijednost %d)",
                    input_get_id(p_input), new_state);
            time_utils_print_timestamp(msg);

            input_set_state(p_input, new_state);
            stats->num_state_changes += 1;

            input_response_t input_response = input_get_response(p_input);
            while (input_response.response == last_response)
            {
                // Odgovor jos nije stigao
                time_utils_delay_for(SHORT_SLEEP_INTERVAL_MS);
                input_response = input_get_response(p_input);
            }

            // Odgovor je stigao
            last_response = input_response.response;

            // Izracunaj vrijeme od promjene stanja do odgovora
            unsigned long state_change_timestamp_ms = input_get_state(p_input).timestamp;
            unsigned long response_timestamp_ms = input_get_response(p_input).timestamp;
            unsigned long response_time_ms = response_timestamp_ms - state_change_timestamp_ms;

            sprintf(msg, "Dretva %d: odgovoreno, %lu od promjene", 
                    input_get_id(p_input), response_time_ms);
            time_utils_print_timestamp(msg);

            // Azuriraj statistiku
            if (response_time_ms > input_get_period(p_input))
            {
                stats->num_problems += 1;

                sprintf(msg, "Dretva %d: odgovor kasni, azuriram statistiku", input_get_id(p_input));
                time_utils_print_timestamp(msg);
            }

            stats->sum_response_times += response_time_ms;

            if (response_time_ms > stats->max_response_time)
            {
                stats->max_response_time = response_time_ms;
            }

            stats->average_response_time = (double) stats->sum_response_times / stats->num_state_changes;

            // Izracunaj vrijeme odgode
            unsigned long delay_until_ms = input_get_state(p_input).timestamp + input_get_period(p_input);
            
            // Ako nije bilo kasnjenja, dodaj dodatnu odgodu
            if (response_time_ms < input_get_period(p_input))
            {
                delay_until_ms += generate_added_delay(input_get_period(p_input), config.K);
            }

            sprintf(msg, "Dretva %d: spavam do %lu", input_get_id(p_input), delay_until_ms);
            time_utils_print_timestamp(msg);

            time_utils_delay_until(delay_until_ms);
        }
    }
}

void simulator_stop_all()
{
    config.simulation_running = false;
}