#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "input.h"

void controller_init(input_t **inputs, int num_inputs);
void controller_run(unsigned int K);
void controller_stop();

#endif