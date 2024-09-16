// CH32V003F4P6 - NP2403 LED Control
//
// September 16, 2024
// Tetsu Nishimura
//
//
// x 基板との接続はGND, 5V, SWDIOの3本
// x スイッチは押しっぱなしにするか，用意してあるスルーホールをジャンパーでつなぐ

#include <stdlib.h>
#include <stdbool.h>
#include "debug.h"
#include "peripheral.h"

//#define NUM_LEDS    1
#define NUM_LEDS    4
//#define NUM_LEDS    8

// ADC value:
// Button pressed:     519~
// Button not pressed: 942~
#define BUTTON_PRESS_THRESHOLD   ((519 + 942) / 2)


typedef enum {
  FADEIN,
  FADEOUT
} fadeMode_t;


void fadein(int num)
{
    for (int n = 0; n < 256; n++) {
        setPwm(num, n);
        Delay_Ms(6);
    }
}

void fadeout(int num)
{
    for (int n = 255; n >= 0; n--) {
        setPwm(num, n);
        Delay_Ms(12);
    }
}

void setRandomSeed(void)
{
    unsigned long seed = 0;

    for (int n = 0; n < 4; n++) {
        seed <<= 8;
        seed |= Get_ADC_Val(ADC_Channel_0) & 0xff;
        // Wait a while to get a different value from previous one
        Delay_Ms(10);
    }

    srand(seed);
}

void selectRandom(fadeMode_t mode)
{
    int state = 0;

    for (;;) {
        int num = rand() % NUM_LEDS;
        int bitMask = 1 << num;

        if ((state & bitMask) == 0) {
            if (mode == FADEIN) {
                fadein(num);
            }
            else {
                fadeout(num);
            }
            state |= bitMask;

            // If all bits are set
            if (state >= (1 << NUM_LEDS) - 1) {
                break;
            }
        }
        else {
            Delay_Ms(500);
        }
    }
}


int main(void)
{
    systemInit();
    powerEnbale(true);
    Delay_Ms(1000);

    setRandomSeed();

    uint8_t buttonPressed;
    if (Get_ADC_Val(ADC_Channel_0) < BUTTON_PRESS_THRESHOLD){
        buttonPressed = 1;
    }
    else {
        buttonPressed = 0;
    }

    selectRandom(FADEIN);

    if (buttonPressed) {
        Delay_Ms(60000);
    }
    else {
        Delay_Ms(2000);
    }

    selectRandom(FADEOUT);

    Delay_Ms(1000);
    powerEnbale(false);

    while(1) {
    }
}
