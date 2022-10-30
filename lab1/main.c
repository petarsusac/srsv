#include "input.h"

#include <stdio.h>
#include "time_utils.h"

int main()
{
    input_t *inputs[] = {
        input_new(1, 500, 100),
        input_new(2, 300, 10),
    };

    input_set_state(inputs[0], 10);
    printf("%d\n", input_get_state(inputs[0]).state);

    time_utils_delay_for(1000);

    printf("%d\n", input_get_state(inputs[1]).state);

    return 0;
}