#ifndef __tim14_H
#define __tim14_H

extern TIM_HandleTypeDef htim14;

extern volatile uint32_t qwerty_tim14_oc_ccr;
extern volatile uint32_t qwerty_tim14_oc_arr;

extern void MX_TIM14_Init(void);
extern void HAL_TIM14_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle);




#endif
