/*
 * peripheral.h
 *
 *  Created on: Sep 16, 2024
 *      Author: main
 */

#ifndef USER_PERIPHERAL_H_
#define USER_PERIPHERAL_H_

#include <stdbool.h>

void systemInit(void);
void GPIO_Toggle_INIT(void);
void TIM_PWMOut_Init(u16 arr, u16 psc, u16 ccp);
void ADC_Function_Init(void);
u16 Get_ADC_Val(u8 ch);
void powerEnbale(bool enable);
void setPwm(int num, int val);


#endif /* USER_PERIPHERAL_H_ */
