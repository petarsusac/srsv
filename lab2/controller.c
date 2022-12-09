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
#define INTERRUPT_PERIOD_MS (1000U)
#define SHORT_PROCESSING_DELAY_MS (5U)

static struct _config
{
    input_t **inputs;
    int num_inputs;
    bool simulation_running;
} config;

static struct _state
{
    volatile int current_task;
    volatile int current_job;
    volatile int current_period;
    volatile int periods_without_overflow;
} state;

static int generate_response(int state)
{
    // Vrati samo kopiju stanja (zasad)
    return state;
}

static int calculate_processing_time()
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

    if (0 != state.current_task)
    {
        // Neki zadatak je u obradi. Provjeri hocemo li mu dopustiti
        // jos jednu periodu ili ne.

        if (1 == state.current_period && state.periods_without_overflow <= 10)
        {
            // TODO: zabiljezi da je dopustena druga perioda
            state.current_period = 2;
            state.periods_without_overflow = 0;
            return;
        }
        else
        {
            // Prekini trenutni zadatak i zapocni obradu novog kodom ispod
            // TODO: Zabiljezi da je zadatak prekinut
        }
    }

    // Lokalne varijable
    int my_task = 0;
    int my_job = 0;
    int processing_time = 0;
    bool take_next = false;

    while (true == take_next)
    {
        take_next = false;

        state.current_task = choose_task_to_process(); // TODO: napisati ovu funkciju
        state.current_job = time_utils_get_time_ms(); // neki nasumicni broj, npr. vrijeme
        my_task = state.current_task;
        my_job = state.current_job;

        state.current_period = 1;
        // TODO: zabiljezi pocetak obrade

        processing_time = calculate_processing_time();

        // TODO: omoguci prekidanje?

        while (my_job == state.current_job && processing_time > 0)
        {
            time_utils_delay_for(SHORT_PROCESSING_DELAY_MS);
            processing_time -= SHORT_PROCESSING_DELAY_MS;
        }

        // TODO: zabrani prekidanje?

        // Po izlazu iz petlje obrada je ili zavrsena ili prekinuta
        if (my_job == state.current_job && processing_time <= 0)
        {
            // Obrada je zavrsena
            // TODO: zabiljezi kraj obrade
            state.current_task = 0;
            state.current_job = 0;
            if (1 == state.current_period)
            {
                state.periods_without_overflow++;
            }
            else
            {
                // Zadatak je potrosio i dio druge periode.
                // Nemoj cekati iduci prekid nego odmah kreni s iducim zadatkom.
                take_next = true;
            }
        }
        else
        {
            // Zadatak je prekinut, izadi iz funkcije
        }
    }

    // TODO: omoguci prekidanje?
}

void controller_init(input_t **inputs, int num_inputs)
{
    config.inputs = inputs;
    config.num_inputs = num_inputs;
    config.simulation_running = true;

    state.current_task = 0;
    state.current_job = 0;
    state.current_period = 0;
    state.periods_without_overflow = 0;

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