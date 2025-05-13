#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "hardware/pio.h"

#define NUM_PIXELS 25  // ou o número que você está usando

#ifdef __cplusplus
extern "C" {
#endif

uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b);
void put_pixel(uint32_t pixel_grb);
void set_one_led(uint8_t r, uint8_t g, uint8_t b, bool led_buffer[]);
void atualizarLeds(bool led_buffer[], bool modo);

#ifdef __cplusplus
}
#endif
