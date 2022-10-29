#include "input.h"

#include <stdlib.h>

struct _input_t 
{
    // staticka svojstva (ne mijenjaju se)
    unsigned int id;
    unsigned int period;
    unsigned int first_occurence;

    // dinamicka svojstva (dretve ih mogu mijenjati)
    input_state_t input_state;
    input_response_t input_response;
} _input_t;

input_t *input_new(unsigned int id, unsigned int period, unsigned int first_occurence)
{
    input_t *p_input = (input_t *) malloc(sizeof(struct _input_t));

    if (NULL != p_input)
    {
        p_input->id = id;
        p_input->period = period;
        p_input->first_occurence = first_occurence;
    }

    return p_input;
}

void input_set_state(input_t *this, int state)
{
    if (NULL != this)
    {
        this->input_state.state = state;
        this->input_state.timestamp = 0;
    }
}

input_state_t input_get_state(input_t *this)
{
    return this->input_state;
}