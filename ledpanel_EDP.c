#include <stdio.h>
#include <stdlib.h>

#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "ssd1306_fontReversed.h"
#include "ws2812.pio.h"

#define IS_RGBW false
#define NUM_PIXELS 256

#ifdef PICO_DEFAULT_WS2812_PIN
#define WS2812_PIN PICO_DEFAULT_WS2812_PIN
#else

#define WS2812_PIN 1
#endif

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

void printChar(char chr) {
    uint8_t chrNum = chr - 48;
    uint8_t charData[8];

    static uint8_t r = 0x04;
    static uint8_t g = 0x04;
    static uint8_t b = 0x00;

    if (chrNum <= 9) {
        // 0 ~ 9 の読み込み処理
        // 対応関係：'0' = 48 = index:(1+26)*8-1
        for (int i = 0; i < 8; ++i) {
            charData[i] = fontReversed[(chrNum + 1 + 26) * 8 - 1 + i];
        }
    } else if (chrNum) {
        // A ～ Z の読み込み処理
        // 対応関係： 'A' = 65 = index:(1+0)*8-1
        for (int i = 0; i < 8; ++i) {
            charData[i] =
                fontReversed[(chrNum - (65 - 48) + 1 + 0) * 8 - 1 + i];
        }
    }

    // 点灯処理
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (i % 2 == 0) {
                if (((charData[i] >> j) & 1) == 0) {
                    put_pixel(urgb_u32(0, 0, 0));
                } else if (((charData[i] >> j) & 1) == 1) {
                    put_pixel(urgb_u32(r, g, b));
                }
            } else if (i % 2 == 1) {
                if (((charData[i] << j) & 0b10000000) == 0) {
                    put_pixel(urgb_u32(0, 0, 0));
                } else if (((charData[i] << j) & 0b10000000) == 0b10000000) {
                    put_pixel(urgb_u32(r, g, b));
                }
            }
        }
    }
}

int main() {
    stdio_init_all();

    // todo get free sm
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);

    while (1) {
        printChar('T');
        printChar('E');
        printChar('S');
        printChar('T');
        sleep_ms(1500);
        printChar('0');
        printChar('1');
        printChar('2');
        printChar('3');
    }
}