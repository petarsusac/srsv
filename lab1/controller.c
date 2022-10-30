#include "controller.h"

#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>

static struct _config
{
    input_t **inputs;
    int num_inputs;
    bool simulation_running;
} config;

void controller_init(input_t **inputs, int num_inputs)
{
    config.inputs = inputs;
    config.num_inputs = num_inputs;
    config.simulation_running = true;
}

void controller_run()
{
    // Stvori dretve koje postavljaju ulaze
    pthread_t tid[config.num_inputs];
    for (int i = 0; i < config.num_inputs; i++)
    {
        if (pthread_create(tid[i], NULL, posao_dretve, arg))
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

    // Upravljacka petlja
    while (config.simulation_running)
    {
        for (int i = 0; i < config.num_inputs; i++)
        {
            input_state_t input_state = input_get_state(config.inputs[i]);
            if (input_state.state != last_state[i])
            {
                simulate_input_processing();
                input_set_response(config.inputs[i], generate_response());
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