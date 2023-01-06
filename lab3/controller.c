#include "controller.h"

#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "simulator.h"
#include "time_utils.h"
#include "call.h"

#define LOG_MESSAGE_LENGTH (100U)
#define SIMULATOR_THREAD_PRIORITY (15U)

static struct _config
{
    input_t **inputs;
    int num_inputs;
    bool simulation_running;
    unsigned long time_limit_ms;
} config;

static int generate_response(int state)
{
    // Vrati samo kopiju stanja (zasad)
    return state;
}

static unsigned calc_input_processing_length(unsigned period)
{
    // Generiraj nasumican broj izmedu 0 i 100
    int rand_num = rand() % 100;
    
    // Distribucija vjerojatnosti razlicitih vremena obrade
    int num_values = 4;
    unsigned processing_times_period_percent[] = {10, 20, 40, 70};
    int probabilities_percent[] = {50, 30, 15, 5};

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
            return (processing_times_period_percent[i] / 100.) * period;
        }
    }
}

static void *controller_thread(void *input)
{
    input_t *p_input = (input_t *) input;

    char msg[50];
    sprintf(msg, "Upravljac %d zapoceo", input_get_id(p_input));
    time_utils_print_timestamp(msg);

    // Pricekaj prvu pojavu ulaza
    unsigned long abs_time_to_wait = input_get_first_occurence(p_input);
    time_utils_delay_until(abs_time_to_wait);

    while(config.simulation_running && time_utils_get_time_ms() < config.time_limit_ms)
    {
        // Simuliraj obradu ulaza
        sprintf(msg, "Upravljac %d: zapoceta obrada", input_get_id(p_input));
        time_utils_print_timestamp(msg);

        time_utils_simulate_ms(calc_input_processing_length(input_get_period(p_input)));

        // Postavi odgovor
        input_set_response(p_input, generate_response(input_get_state(p_input).state));

        sprintf(msg, "Upravljac %d: odgovor postavljen", input_get_id(p_input));
        time_utils_print_timestamp(msg);

        // Cekaj iducu periodu
        abs_time_to_wait += input_get_period(p_input);
        time_utils_delay_until(abs_time_to_wait);
    }
}

void controller_init(input_t **inputs, int num_inputs, unsigned long time_limit_ms)
{
    config.inputs = inputs;
    config.num_inputs = num_inputs;
    config.simulation_running = true;
    config.time_limit_ms = time_limit_ms;

    srand(time(NULL));
}

void controller_run(bool rt_sched)
{
    // Stvori dretve koje postavljaju ulaze
    pthread_t tid_sim[config.num_inputs];
    simulator_init(config.num_inputs, config.time_limit_ms);

    if (!rt_sched)
    {
        for (int i = 0; i < config.num_inputs; i++)
        {
            if (pthread_create(&tid_sim[i], NULL, &simulator_run, (void *) config.inputs[i]))
            {
                perror("Error: pthread_create");
            }
        }
    }
    else
    {
        pthread_attr_t attr;
        CALL(STOP, pthread_attr_init, &attr);
        CALL(STOP, pthread_attr_setinheritsched, &attr, PTHREAD_EXPLICIT_SCHED);
        CALL(STOP, pthread_attr_setschedpolicy, &attr, SCHED_FIFO);
        struct sched_param param;
        param.sched_priority = SIMULATOR_THREAD_PRIORITY;
        CALL(STOP, pthread_attr_setschedparam, &attr, &param);
        
        for (int i = 0; i < config.num_inputs; i++)
        {
            pthread_create(&tid_sim[i], &attr, &simulator_run, (void *) config.inputs[i]);
        }
    }

    // Stvori upravljacke dretve
    pthread_t tid_controller[config.num_inputs];
    if (!rt_sched)
    {
        for (int i = 0; i < config.num_inputs; i++)
        {
            if (pthread_create(&tid_controller[i], NULL, &controller_thread, (void *) config.inputs[i]))
            {
                perror("Error: pthread_create");
            }
        }
    }
    else
    {
        pthread_attr_t attr;
        CALL(STOP, pthread_attr_init, &attr);
        CALL(STOP, pthread_attr_setinheritsched, &attr, PTHREAD_EXPLICIT_SCHED);
        CALL(STOP, pthread_attr_setschedpolicy, &attr, SCHED_FIFO);
        
        for (int i = 0; i < config.num_inputs; i++)
        {
            struct sched_param param;
            param.sched_priority = config.num_inputs - input_get_id(config.inputs[i]) + 1;
            CALL(STOP, pthread_attr_setschedparam, &attr, &param);
            CALL(STOP, pthread_create, &tid_controller[i], &attr, &controller_thread, (void *) config.inputs[i]);
        }
    }

    // Cekaj da zavrse upravljacke dretve
    for (int i = 0; i < config.num_inputs; i++)
    {
        pthread_join(tid_controller[i], NULL);
    }

    // Zaustavi dretve simulatore
    simulator_stop_all();
    for (int i = 0; i < config.num_inputs; i++)
    {
        pthread_join(tid_sim[i], NULL);
    }
}

void controller_stop()
{
    config.simulation_running = false;
}