#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "input.h"

void controller_init(input_t **inputs, int num_inputs, int num_input_types, int *num_inputs_by_type);
void controller_start();
void controller_stop();

#endif