#include "input.h"

#include <stdlib.h>
#include "time_utils.h"

struct _input_t 
{
    // staticka svojstva (ne mijenjaju se)
    unsigned int id;
    unsigned int period;
    unsigned int first_occurence;

    // dinamicka svojstva (dretve ih mogu mijenjati)
    input_state_t input_state;
    input_response_t input_response;
    input_stats_t stats;
};

input_t *input_new(unsigned int id, unsigned int period, unsigned int first_occurence)
{
    input_t *p_input = (input_t *) malloc(sizeof(struct _input_t));

    if (NULL != p_input)
    {
        p_input->id = id;
        p_input->period = period;
        p_input->first_occurence = first_occurence;

        p_input->input_state.state = 0;
        p_input->input_state.timestamp = 0;

        p_input->input_response.response = 0;
        p_input->input_response.timestamp = 0;

        p_input->stats.average_response_time = 0.0;
        p_input->stats.max_response_time = 0;
        p_input->stats.num_problems = 0;
        p_input->stats.num_state_changes = 0;
        p_input->stats.sum_response_times = 0;
    }

    return p_input;
}

void input_set_state(input_t *this, int state)
{
    if (NULL != this)
    {
        this->input_state.state = state;
        this->input_state.timestamp = time_utils_get_time_ms();
    }
}

input_state_t input_get_state(input_t *this)
{
    return this->input_state;
}

void input_set_response(input_t *this, int response)
{
    if (NULL != this)
    {
        this->input_response.response = response;
        this->input_response.timestamp = time_utils_get_time_ms();
    }
}

input_response_t input_get_response(input_t *this)
{
    return this->input_response;
}

unsigned long input_get_first_occurence(input_t *this)
{
    if (NULL != this)
    {
        return this->first_occurence;
    }

    return 0;
}

unsigned long input_get_period(input_t *this)
{
    if (NULL != this)
    {
        return this->period;
    }

    return 0;
}

unsigned int input_get_id(input_t *this)
{
    if (NULL != this)
    {
        return this->id;
    }

    return 0;
}

input_stats_t *input_get_stats(input_t *this)
{
    if (NULL != this)
    {
        return &this->stats;
    }

    return NULL;
}
