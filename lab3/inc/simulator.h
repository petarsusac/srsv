#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

void simulator_init(int num_inputs, unsigned time_limit_ms);
extern void *simulator_run(void *input);
void simulator_stop_all();

#endif
