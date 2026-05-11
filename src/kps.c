#include "cfg.h"
#include "kps.h"

#include "pico/rand.h"
#include <string.h>

static float noise_table[TABLE_SIZE];

void delay_init_noise_src()
{
    for (int i = 0; i < TABLE_SIZE; i++) {
        noise_table[i] = (int32_t)get_rand_32() / (float)0x80000000U;
    }
}

void delay_set_freq(struct delay *dly, float freq)
{
    assert(freq >= FREQ_MIN && freq <= SAMPLE_RATE / 2.0f);
    dly->idx = 0;
    dly->size = SAMPLE_RATE / freq;
}

void delay_excite(struct delay *dly)
{
    memcpy(dly->buf, noise_table, dly->size);
}

float delay_get_next_sample(struct delay *dly)
{
    float sample = dly->buf[dly->idx];
    for (int i = 1; i < dly->filter_size; i++) {
        sample += dly->buf[(dly->idx + i) % dly->size];
    }
    sample *= 1.0f / dly->filter_size * dly->decay;
    dly->buf[dly->idx] = sample;
    dly->idx = (dly->idx + 1) % dly->size;
    return sample;
}
