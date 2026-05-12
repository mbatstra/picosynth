#include "cfg.h"
#include "kps.h"

#include "pico/audio_i2s.h"
#include "hardware/gpio.h"
#include "pico/time.h"
#include "pico/stdlib.h"
#include "pico/util/queue.h"
#include "pico/types.h"

#include <stdint.h>
#include <stdio.h>

typedef enum {
    BTN_A,
    BTN_B,
    BTN_C,
    BTN_D,
} button_id;

queue_t btn_queue;

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

void on_button_press(uint pin)
{
    button_id bid;

    switch (pin) {
        case BTN_A_PIN:
            bid = BTN_A;
            queue_try_add(&btn_queue, &bid);
            break;
        case BTN_B_PIN:
            bid = BTN_B;
            queue_try_add(&btn_queue, &bid);
            break;
        case BTN_C_PIN:
            bid = BTN_C;
            queue_try_add(&btn_queue, &bid);
            break;
        case BTN_D_PIN:
            bid = BTN_D;
            queue_try_add(&btn_queue, &bid);
            break;
    }
}

void btn_a_isr() { on_button_press(BTN_A_PIN); }
void btn_b_isr() { on_button_press(BTN_B_PIN); }
void btn_c_isr() { on_button_press(BTN_C_PIN); }
void btn_d_isr() { on_button_press(BTN_D_PIN); }

int main()
{
    stdio_init_all();

    queue_init(&btn_queue, sizeof(button_id), 4);
    gpio_set_irq_enabled_with_callback(BTN_A_PIN, GPIO_IRQ_EDGE_FALL, true, btn_a_isr);
    gpio_set_irq_enabled_with_callback(BTN_B_PIN, GPIO_IRQ_EDGE_FALL, true, btn_b_isr);
    gpio_set_irq_enabled_with_callback(BTN_C_PIN, GPIO_IRQ_EDGE_FALL, true, btn_c_isr);
    gpio_set_irq_enabled_with_callback(BTN_D_PIN, GPIO_IRQ_EDGE_FALL, true, btn_d_isr);

    delay_init_noise_src();
    struct delay delay = {
        .decay = 0.999f,
        .filter_size = 5,
    };
    delay_set_freq(&delay, 440.0f);
    absolute_time_t excite_at = make_timeout_time_ms(1500);
    const float amp = 0.6f;

    struct audio_buffer_pool *ap = init_audio();
    button_id btn;

    while (true) {
        if (queue_try_remove(&btn_queue, &btn)) {
            switch (btn) {
                case BTN_A:
                    delay_excite(&delay);
                    printf("button press\n");
                    break;
                default:
                    break;
            }
        }
        if (get_absolute_time() > excite_at) {
            excite_at = make_timeout_time_ms(1500);
        }

        struct audio_buffer *buffer = take_audio_buffer(ap, true);
        int16_t *samples = (int16_t *) buffer->buffer->bytes;
        for (uint i = 0; i < buffer->max_sample_count; i++) {
            samples[i] = delay_get_next_sample(&delay) * amp * INT16_MAX;
        }

        buffer->sample_count = buffer->max_sample_count;
        give_audio_buffer(ap, buffer);
    }
    return 0;
}
