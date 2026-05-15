#include "cfg.h"
#include "kps.h"

#include "common.h"

#include "pico/audio_i2s.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "pico/stdlib.h"
#include "pico/types.h"

#include <stdint.h>
#include <stdio.h>

static int16_t noise_table[TABLE_SIZE];

struct audio_buffer_pool *init_audio()
{
    static audio_format_t audio_format = {
        .format = AUDIO_BUFFER_FORMAT_PCM_S16,
        .sample_freq = SAMPLE_RATE,
        .channel_count = 1,
    };

    static struct audio_buffer_format producer_format = {
        .format = &audio_format,
        .sample_stride = 2
    };

    struct audio_buffer_pool *producer_pool = audio_new_producer_pool(&producer_format, 3, SAMPLES_PER_BUFFER);

    const struct audio_format *output_format;

    struct audio_i2s_config config = {
        .data_pin = PICO_AUDIO_I2S_DATA_PIN,
        .clock_pin_base = PICO_AUDIO_I2S_CLOCK_PIN_BASE,
        .dma_channel = 0,
        .pio_sm = 0,
    };

    output_format = audio_i2s_setup(&audio_format, &config);
    if (!output_format) {
        panic("PicoAudio: Unable to open audio device.\n");
    }

    bool ok = audio_i2s_connect(producer_pool);
    assert(ok);
    audio_i2s_set_enabled(true);
    return producer_pool;
}

int main()
{
    stdio_init_all();

    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);

    hx711_multi_config_t hxmcfg;
    hx711_multi_get_default_config(&hxmcfg);
    hxmcfg.clock_pin = 26;
    hxmcfg.data_pin_base = 6;
    hxmcfg.chips_len = 4;
    hxmcfg.pio = pio1;

    hx711_multi_t hxm;
    hx711_multi_init(&hxm, &hxmcfg);
    hx711_multi_power_up(&hxm, hx711_gain_128);
    hx711_wait_settle(hx711_rate_10);

    delay_init_noise_src();
    struct delay delay = {
        .decay = 0.992f,
        .filter_size = 5,
    };
    delay_set_freq(&delay, 220.0f);

    const float freqs[] = { 110.0f, 220.0f, 330.0f, 440.0f, 550.0f };
    int i = 0;
    absolute_time_t excite_at = make_timeout_time_ms(1500);
    const float amp = 0.075f;
    struct audio_buffer_pool *ap = init_audio();
    bool led = true;

    // int32_t arr[hxmcfg.chips_len];
    while (true) {
        // if (get_absolute_time() > excite_at) {
        //     led = !led;
        //     gpio_put(25, led);

        //     delay_set_freq(&delay, freqs[i]);
        //     i = (i + 1) % 5;

        //     excite_at = make_timeout_time_ms(500);
        //     delay_excite(&delay);
        // }

        // struct audio_buffer *buffer = take_audio_buffer(ap, true);
        // int16_t *samples = (int16_t *) buffer->buffer->bytes;
        // for (uint i = 0; i < buffer->max_sample_count; i++) {
        //     samples[i] = delay_get_next_sample(&delay) * amp * INT16_MAX;
        // }

        // buffer->sample_count = buffer->max_sample_count;
        // give_audio_buffer(ap, buffer);

        // hx711_multi_get_values(&hxm, arr);
        // for (int i = 0; i < hxmcfg.chips_len; i++) { printf("%d ", arr[i]); }
        // printf("\n");
        sleep_ms(500);
        led = !led;
        gpio_put(25, led);
    }
    return 0;
}
