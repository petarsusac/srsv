#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "input.h"

void controller_init(input_t **inputs, int num_inputs);
void controller_start();
void controller_stop();

#endif