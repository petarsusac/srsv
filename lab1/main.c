#include "input.h"

#include <stdio.h>

int main()
{
    input_t *inputs[] = {
        input_new(1, 500, 100),
        input_new(2, 300, 10),
    };

    input_set_state(inputs[0], 10);
    printf("%d\n", input_get_state(inputs[0]).state);

    return 0;
}