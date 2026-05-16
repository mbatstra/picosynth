#ifndef CFG_H
#define CFG_H

#define SAMPLE_RATE 48000.0f
#define SAMPLES_PER_BUFFER 256

#define FREQ_MIN 55.0f
#define TABLE_SIZE 2048
#define MAX_DELAY_SIZE 873 // sample rate over min freq

#define PICO_AUDIO_I2S_DATA_PIN 20
#define PICO_AUDIO_I2S_CLOCK_PIN_BASE 18
#define PICO_AUDIO_I2S_CLOCK_PINS_SWAPPED 1

#define CORE1_STACK_SIZE 1024

#define BTN_A_PIN 6
#define BTN_B_PIN 7
#define BTN_C_PIN 8
#define BTN_D_PIN 9

#endif // !CFG_H
