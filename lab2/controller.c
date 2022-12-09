#include "controller.h"

#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "periodic_signal.h"
#include "call.h"
#include "time_utils.h"
#include "simulator.h"

#define LOG_MESSAGE_LENGTH (100U)
#define K (1U)
#define INTERRUPT_PERIOD_MS 1000

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

static void calculate_processing_time()
{
    // Generiraj nasumican broj izmedu 0 i 100
    int rand_num = rand() % 100;
    
    // Distribucija vjerojatnosti razlicitih vremena obrade
    int num_values = 4;
    int processing_times_ms[] = {30, 50, 80, 120};
    int probabilities_percent[] = {20, 50, 25, 5};

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
            // Vrati trajanje obrade
            return processing_times_ms[i];
        }
    }
}

static void create_periodic_interrupt(void (*interrupt_cb)(int))
{
    signal(SIGINT, interrupt_cb);
    periodic_signal_create(SIGINT, INTERRUPT_PERIOD_MS);
}

// GLAVNA FUNKCIJA ZA OBRADU
// Poziva se na periodicki prekid (odnosno signal koji generira timer)
static void process_task(int signum)
{
    (void) signum;

    time_utils_print_timestamp("Periodicki prekid zapoceo");
}

void controller_init(input_t **inputs, int num_inputs)
{
    config.inputs = inputs;
    config.num_inputs = num_inputs;
    config.simulation_running = true;

    srand(time(NULL));
}

void controller_start()
{
    // Pokreni periodicki prekid
    create_periodic_interrupt(process_task);

    // Stvori dretve koje postavljaju ulaze
    pthread_t tid[config.num_inputs];
    simulator_init(config.num_inputs, K);
    for (int i = 0; i < config.num_inputs; i++)
    {
        CALL(STOP, pthread_create, &tid[i], NULL, &simulator_run, (void *) config.inputs[i]);
    }

    time_utils_print_timestamp("Upravljac zapoceo");

    // Cekaj kraj simulacije
    while (config.simulation_running) ;

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