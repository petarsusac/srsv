#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "input.h"

void controller_init(input_t **inputs, int num_inputs, unsigned long time_limit_ms);
void controller_run();
void controller_stop();

#endif