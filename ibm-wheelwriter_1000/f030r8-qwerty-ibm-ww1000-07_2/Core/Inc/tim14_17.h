#ifndef __tim14_H

#define __tim14_H

#define   TIM14_DELAY  (1L)
#define   TIM14_PULSE  (1972L)
#define   TIM14_CCR    (1916L)

extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim17;

extern volatile uint32_t qwerty_tim14_oc_ccr;
extern volatile uint32_t qwerty_tim14_oc_arr;
extern volatile uint32_t qwerty_tim14_oc_arr2;
// extern uint32_t qwerty_tim17_base_prescaler;

extern void MX_TIM14_Init(void);
extern void HAL_TIM14_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle);
extern void HAL_TIM14_MspPostInit (TIM_HandleTypeDef* htim_base);

extern void MX_TIM17_Init(void);
extern void HAL_TIM17_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle);

#endif
