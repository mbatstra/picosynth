#ifndef KPS_H
#define KPS_H

#include "cfg.h"
#include <stddef.h>
#include <stdint.h>

struct delay {
  float buf[MAX_DELAY_SIZE];
  size_t size;
  size_t idx;
  size_t filter_size;
  float decay;
};

void delay_init_noise_src();
void delay_set_freq(struct delay *delay, float freq);
void delay_excite(struct delay *delay);
float delay_get_next_sample(struct delay *delay);

#endif // !KPS_H
