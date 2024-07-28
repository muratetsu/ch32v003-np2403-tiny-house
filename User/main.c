// CH32V003F4P6 - NP2403 LED Control
//
// July 26, 2024
// Tetsu Nishimura
//
//
// x 基板との接続はGND, 5V, SWDIOの3本
//   GNDと5Vは電池端子のところに繋いでもよい
//
// x スイッチは押しっぱなしにするか，用意してあるスルーホールをジャンパーでつなぐ

#include "debug.h"

#define NUM_LEDS    8

void GPIO_Toggle_INIT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

// Pin Name Timer
// 19  PD2  T1CH1
//  5  PA1  T1CH2
// 13  PC3  T1CH3
// 14  PC4  T1CH4
//  1  PD4  T2CH1ETR
// 20  PD3  T2CH2
// 10  PC0  T2CH3
//  4  PD7  T2CH4, デフォルトの設定はNRSTになっているので使用するにはUser Option Bytesの変更が必要
void TIM_PWMOut_Init(u16 arr, u16 psc, u16 ccp)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    TIM_OCInitTypeDef TIM_OCInitStructure={0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure={0};

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);

    // PA1/T1CH2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // PC0/T2CH3, PC3/T1CH3, PC4/T1CH4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // PD2/T1CH1, PD3/T2CH2, PD4/T2CH1ETR, PD7/T2CH4
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // Initialize time base unit
    TIM_TimeBaseInitStructure.TIM_Period = arr;
    TIM_TimeBaseInitStructure.TIM_Prescaler = psc;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;   // 極性の設定っぽい
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = ccp;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_CtrlPWMOutputs(TIM2, ENABLE);
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Disable);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Disable);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Disable);
    TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Disable);
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Disable);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Disable);
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Disable);
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Disable);

    TIM_ARRPreloadConfig(TIM1, ENABLE);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

void setPwm(int num, int val)
{
    switch (num) {
    case 0:
        TIM_SetCompare1(TIM1, val);
        break;
    case 1:
        TIM_SetCompare2(TIM1, val);
        break;
    case 2:
        TIM_SetCompare3(TIM1, val);
        break;
    case 3:
        TIM_SetCompare4(TIM1, val);
        break;
    case 4:
        TIM_SetCompare1(TIM2, val);
        break;
    case 5:
        TIM_SetCompare2(TIM2, val);
        break;
    case 6:
        TIM_SetCompare3(TIM2, val);
        break;
    case 7:
        TIM_SetCompare4(TIM2, val);
        break;
    }
}

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

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    GPIO_Toggle_INIT();
    GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_SET);  // Enable DCDC

    TIM_PWMOut_Init(255, 250 - 1, 0);  // 8MHz / 250 / 256 = 125Hz

    Delay_Ms(1000);

    for (int n = 0; n < NUM_LEDS; n++) {
        fadein(n);
    }

    Delay_Ms(5000);

    for (int n = 0; n < NUM_LEDS; n++) {
        fadeout(n);
    }

    Delay_Ms(1000);
    GPIO_WriteBit(GPIOC, GPIO_Pin_1, Bit_RESET);    // Disable DCDC

    while(1) {
    }
}
