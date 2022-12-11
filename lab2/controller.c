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
#define INTERRUPT_PERIOD_MS (100U)
#define SHORT_PROCESSING_DELAY_MS (5U)

static struct _config
{
    input_t **inputs;
    int num_inputs;
    int num_input_types;
    int *num_inputs_by_type;
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

static input_t *get_input_by_id(int id)
{
    for (int i = 0; i < config.num_inputs; i++)
    {
        if (NULL == config.inputs[i])
        {
            break;
        }
        else if (input_get_id(config.inputs[i]) == id)
        {
            return config.inputs[i];
        }
    }

    return NULL;
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

static int choose_task_to_process()
{
    // Raspored:
    // A B - A B - A C - -

    // A = 0, B = 1, C = 2, nista = -1
    int sequence[] = {0, 1, -1, 0, 1, -1, 0, 2, -1, -1};

    // Indeks koji broji koji tip zadatka je na redu
    static int type_index = 0;

    // Indeksi koji broje koji konkretno zadatak svakog tipa je na redu
    static int task_indexes[] = {0, 0, 0};

    int task_to_process_id;
    int task_type = sequence[type_index];
    if (-1 != task_type)
    {
        int task_to_process_index;
        if (0 == task_type)
        {
            task_to_process_index = task_indexes[task_type];
        }
        else if (1 == task_type)
        {
            task_to_process_index = config.num_inputs_by_type[0] + task_indexes[task_type];
        }
        else if (2 == task_type)
        {
            task_to_process_index = config.num_inputs_by_type[0] + config.num_inputs_by_type[1] + task_indexes[task_type];
        }

        if (config.inputs[task_to_process_index])
        {
            task_to_process_id = input_get_id(config.inputs[task_to_process_index]);
        }
        else
        {
            task_to_process_id = 0;
        }

        task_indexes[task_type] = (task_indexes[task_type] + 1) % config.num_inputs_by_type[task_type];
    }
    else
    {
        task_to_process_id = 0;
    }

    type_index = (type_index + 1) % 10; // TODO: izvuci ovaj 10 u neki define

    return task_to_process_id;
}

// GLAVNA FUNKCIJA ZA OBRADU
// Poziva se na periodicki prekid (odnosno signal koji generira timer)
static void process_task(int signum)
{
    (void) signum;

    time_utils_print_timestamp("Upravljac: Periodicki prekid zapoceo");

    input_stats_t *stats = input_get_stats(get_input_by_id(state.current_task));

    char msg[100];

    if (0 != state.current_task)
    {
        // Neki zadatak je u obradi. Provjeri hocemo li mu dopustiti
        // jos jednu periodu ili ne.

        if (1 == state.current_period && state.periods_without_overflow <= 10)
        {
            // Zabiljezi da je dopustena druga perioda
            stats->two_periods_used_counter++;

            sprintf(msg, "Upravljac: Zadatku %d je dopustena druga perioda", state.current_task);
            time_utils_print_timestamp(msg);

            state.current_period = 2;
            state.periods_without_overflow = 0;
            return;
        }
        else
        {
            // Prekini trenutni zadatak i zapocni obradu novog kodom ispod
            // Zabiljezi da je zadatak prekinut
            sprintf(msg, "Upravljac: Zadatak %d je prekinut", state.current_task);
            time_utils_print_timestamp(msg);
        }
    }

    // Lokalne varijable
    int my_task = 0;
    int my_job = 0;
    int processing_time = 0;
    bool take_next = true;

    while (true == take_next)
    {
        take_next = false;

        state.current_task = choose_task_to_process();

        if (0 == state.current_task)
        {
            // Nema zadataka koji cekaju obradu
            time_utils_print_timestamp("Upravljac: Nema zadataka koji cekaju na obradu. Vrtim prazan zadatak ovaj period.");
            return;
        }

        state.current_job = time_utils_get_time_ms(); // neki nasumicni broj, npr. vrijeme
        my_task = state.current_task;
        my_job = state.current_job;

        state.current_period = 1;

        // Zabiljezi pocetak obrade
        unsigned long processing_start = time_utils_get_time_ms();

        sprintf(msg, "Upravljac: Zapoceta obrada zadatka %d", my_task);
        time_utils_print_timestamp(msg);

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
            // Zabiljezi kraj obrade
            unsigned long processing_end = time_utils_get_time_ms();
            sprintf(msg, "Upravljac: Obrada zadatka %d je zavrsena, trajanje: %lu ms", my_task, processing_end - processing_start);
            time_utils_print_timestamp(msg);

            // Upisi odgovor za dretvu simulator
            input_set_response(get_input_by_id(my_task), generate_response(input_get_state(get_input_by_id(my_task)).state));

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
            // Prije izlaska zapisi neki odgovor tako da dretva simulator ne ceka
            input_set_response(get_input_by_id(my_task), generate_response(input_get_state(get_input_by_id(my_task)).state));
        }
    }

    // TODO: omoguci prekidanje?
}

void controller_init(input_t **inputs, int num_inputs, int num_input_types, int *num_inputs_by_type)
{
    config.inputs = inputs;
    config.num_inputs = num_inputs;
    config.num_input_types = num_input_types;
    config.num_inputs_by_type = num_inputs_by_type;
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