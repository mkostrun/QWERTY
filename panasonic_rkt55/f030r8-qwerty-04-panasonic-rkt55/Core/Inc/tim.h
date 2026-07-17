#ifndef __tim_H
#define __tim_H

#define   TIM16_PWM_MODE_POSITIVE (1)
#define   TIM16_PWM_MODE_INVERTED (0)

#define   TIM3_OC_MODE_POSITIVE (1)
#define   TIM3_OC_MODE_INVERTED (0)


/* Private variables ---------------------------------------------------------*/

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim14;
extern TIM_HandleTypeDef htim16;
extern DMA_HandleTypeDef hdma_tim3_ch3;
extern DMA_HandleTypeDef hdma_tim3_ch4_up;

void HAL_TIM3_Base_MspInit    (TIM_HandleTypeDef* htim_base);
void HAL_TIM3_MspPostInit     (TIM_HandleTypeDef* timHandle);
void HAL_TIM_Base_MspPostInit (TIM_HandleTypeDef* htim_base);
void HAL_TIM_Base_MspDeInit   (TIM_HandleTypeDef* htim_base);
void HAL_TIM14_Base_MspInit   (TIM_HandleTypeDef* htim_base);
void HAL_TIM14_MspPostInit    (TIM_HandleTypeDef* htim_base);
void HAL_TIM16_Base_MspInit   (TIM_HandleTypeDef* htim_base);
void HAL_TIM16_MspPostInit    (TIM_HandleTypeDef* htim_base);
void HAL_TIM16_Base_MspDeInit (TIM_HandleTypeDef* htim_base);

extern void MX_TIM3_Init(void);
extern void MX_TIM14_Init(void);
extern void MX_TIM16_Init(void);
extern uint32_t           qwerty_tim_base_prescaler;
extern volatile uint32_t  qwerty_tim14_oc_arr;
extern volatile uint32_t  qwerty_tim14_oc_ccr;
extern volatile uint8_t   qwerty_tim3_oc_mode, qwerty_tim3_oc_new_mode;
extern volatile uint32_t  qwerty_tim3_oc_it_edge_lead, qwerty_tim3_oc_it_edge_trail;
extern volatile uint8_t   qwerty_tim16_pwm_mode, qwerty_tim16_pwm_new_mode;
extern uint32_t           qwerty_tim16_pwm_period;
extern uint32_t           qwerty_tim16_pwm_pulse;

extern volatile uint8_t edge1;
extern volatile uint8_t edge2;
extern volatile uint32_t IC_Value1;
extern volatile uint32_t IC_Value3;
extern volatile uint32_t IC_Value4;
extern volatile uint32_t previous_IC_Value1;
extern volatile uint32_t previous_IC_Value3;
extern volatile uint32_t counter;
extern volatile uint32_t period1;
extern volatile uint32_t period1_safe;
extern volatile uint32_t pulse1;
extern volatile uint32_t period2;
extern volatile uint32_t period2_safe;
extern volatile uint32_t pulse2;
extern volatile uint32_t delay2;



#endif
