#include "scheduling.h"


int scheduling_choose_task_to_process(input_t **inputs, int *num_inputs_by_type)
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
            task_to_process_index = num_inputs_by_type[0] + task_indexes[task_type];
        }
        else if (2 == task_type)
        {
            task_to_process_index = num_inputs_by_type[0] + num_inputs_by_type[1] + task_indexes[task_type];
        }

        if (inputs[task_to_process_index])
        {
            task_to_process_id = input_get_id(inputs[task_to_process_index]);
        }
        else
        {
            task_to_process_id = 0;
        }

        task_indexes[task_type] = (task_indexes[task_type] + 1) % num_inputs_by_type[task_type];
    }
    else
    {
        task_to_process_id = 0;
    }

    type_index = (type_index + 1) % 10; // TODO: izvuci ovaj 10 u neki define

    return task_to_process_id;
}