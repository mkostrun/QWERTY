#ifndef __stm32f0xx_it_H
#define __stm32f0xx_it_H

extern volatile uint8_t  mux_it_edge ;
extern volatile uint8_t  mux_it_mode ;
extern volatile uint32_t mux_it_mode_gpio_pin_edge0_state ;
extern volatile uint32_t mux_it_mode_gpio_pin_edge1_state ;

extern volatile uint32_t qwerty_count_typewriter_base_pulses;
extern volatile uint32_t qwerty_count_typewriter_base_pulses_after_last_printed_character;
extern volatile uint32_t qwerty_count_typed_characters;

extern void NMI_Handler(void);

extern void HardFault_Handler(void);

extern void SVC_Handler(void);

extern void PendSV_Handler(void);

extern void SysTick_Handler(void);

extern void DMA1_Channel4_5_IRQHandler(void);

extern void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);

extern void TIM3_IRQHandler(void);

extern void TIM14_IRQHandler(void);

extern void HAL_TIM_OC_DelayElapsedCallback (TIM_HandleTypeDef *htim);

extern void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *htim);


#ifdef QWERTY_MUX_PIN
  extern void EXTI0_1_IRQHandler(void);
  extern void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
#endif

#endif

