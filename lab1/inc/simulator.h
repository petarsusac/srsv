#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

void simulator_init(int num_inputs, int K);
extern void *simulator_run(void *input);
void simulator_stop_all();

#endif
