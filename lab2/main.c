#include <stdio.h>
#include <signal.h>

#include "time_utils.h"
#include "controller.h"

static void sigint_handler(int signum)
{
    controller_stop();
    time_utils_print_timestamp("Simulacija je zaustavljena, cekam da sve dretve zavrse");
}


int main(void)
{
    // Signal za prekidanje simulacije (i ispis statistike po prekidu)
    signal(SIGINT, sigint_handler);

    // 3 zadatka tipa A (T=1s), 10 zadataka tipa B (T=5s) i 7 zadataka tipa C (T=10s)
    // Podjela 1 sekunde na intervale od 100 ms 
    // A se mora obraditi 3 puta u 1 s, B 2 puta i C jednom s tim da ce se izvoditi prazan C u 3 od 10 s
    // Raspored:
    // A B - A B - A C - -

    input_t *tasks[] = {
        // id, period, prva pojava

        // Zadaci tipa A
        input_new(1, 1000, 0),
        input_new(2, 1000, 200),
        input_new(3, 1000, 400),

        // Zadaci tipa B
        input_new(4, 5000, 0),
        input_new(5, 5000, 300),
        input_new(6, 5000, 1000),
        input_new(7, 5000, 1300),
        input_new(8, 5000, 2000),
        input_new(9, 5000, 2300),
        input_new(10, 5000, 3000),
        input_new(11, 5000, 3300),
        input_new(12, 5000, 4000),
        input_new(13, 5000, 4300),

        // Zadaci tipa C
        input_new(14, 10000, 0),
        input_new(15, 10000, 1000),
        input_new(16, 10000, 2000),
        input_new(17, 10000, 3000),
        input_new(18, 10000, 4000),
        input_new(19, 10000, 5000),
        input_new(20, 10000, 6000),
        NULL,
        NULL,
        NULL,
    };

    int num_tasks_by_type[] = {3, 10, 10};

    time_utils_init();
    controller_init(tasks, 23, 3, num_tasks_by_type);

    controller_start();

    for (int i = 0; i < 23; i++)
    {
        input_delete(tasks[i]);
    }

    return 0;
}