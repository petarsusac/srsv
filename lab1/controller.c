#include "controller.h"

#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "simulator.h"
#include "time_utils.h"

#define K (2U)
#define LOG_MESSAGE_LENGTH (100U)

static struct _config
{
    input_t **inputs;
    int num_inputs;
    bool simulation_running;
} config;

static int generate_response(int state)
{
    // Vrati samo kopiju stanja (zasad)
    return state;
}

static void simulate_input_processing()
{
    // Generiraj nasumican broj izmedu 0 i 100
    int rand_num = rand() % 100;
    
    // Distribucija vjerojatnosti razlicitih vremena obrade
    int num_values = 4;
    int processing_times_ms[] = {30, 50, 80, 120};
    int probabilities_percent[] = {20, 50, 20, 10};

    // Izracunaj u koji od intervala je upala slucajna vrijednost
    for (int i = 0; i < num_values; i++)
    {
        int interval = 0;
        for (int j = 0; j <= i; j++)
        {
            interval += probabilities_percent[j];
        }

        if (rand_num < interval)
        {
            // Odgodi izvrsavanje dretve
            time_utils_delay_for(processing_times_ms[i]);
            break;
        }
    }
}

void controller_init(input_t **inputs, int num_inputs)
{
    config.inputs = inputs;
    config.num_inputs = num_inputs;
    config.simulation_running = true;

    srand(time(NULL));
}

void controller_run()
{
    // Stvori dretve koje postavljaju ulaze
    pthread_t tid[config.num_inputs];
    simulator_init(config.num_inputs, K);
    for (int i = 0; i < config.num_inputs; i++)
    {
        if (pthread_create(&tid[i], NULL, &simulator_run, (void *) config.inputs[i]))
        {
            perror("Error: pthread_create");
        }
    }

    // Treba spremati zadnje stanje svakog ulaza kako bi znali je li se promijenilo
    int last_state[config.num_inputs];
    for (int i = 0; i < config.num_inputs; i++)
    {
        last_state[i] = 0;
    }

    char msg[LOG_MESSAGE_LENGTH];

    time_utils_print_timestamp("Upravljac zapoceo");

    // Upravljacka petlja
    while (config.simulation_running)
    {
        for (int i = 0; i < config.num_inputs; i++)
        {
            input_state_t input_state = input_get_state(config.inputs[i]);
            if (input_state.state != last_state[i])
            {
                sprintf(msg, "Upravljac: ulaz %d: promjena (%d -> %d), obradujem",
                        input_get_id(config.inputs[i]), last_state[i], input_state.state);
                time_utils_print_timestamp(msg);

                simulate_input_processing();

                int response = generate_response(input_state.state);

                sprintf(msg, "Upravljac: ulaz %d: kraj obrade, postavljam odgovor %d",
                        input_get_id(config.inputs[i]), response);
                time_utils_print_timestamp(msg);

                input_set_response(config.inputs[i], response);

                last_state[i] = input_state.state;
            }
        }
    }

    // Zaustavi dretve simulatore
    simulator_stop_all();
    for (int i = 0; i < config.num_inputs; i++)
    {
        pthread_join(tid[i], NULL);
    }
}

void controller_stop()
{
    config.simulation_running = false;
}