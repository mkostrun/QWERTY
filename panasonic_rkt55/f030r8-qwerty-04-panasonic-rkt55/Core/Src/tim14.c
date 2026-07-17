#include "main.h"

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim14;

// volatile uint32_t qwerty_tim14_oc_delay = TIM14_CCR + 200 *  TIME14_DELAY ;
// volatile uint32_t qwerty_tim14_oc_pulse = TIM14_CCR + 200 * (TIME14_DELAY + 1) ;

// volatile uint32_t qwerty_tim14_oc_ccr = 1;
// volatile uint32_t qwerty_tim14_oc_arr = 183;
volatile uint32_t qwerty_tim14_oc_ccr =
    (TIM14_DELAY == 0 ?  1        : TIM14_CCR + TIM14_PULSE * (TIM14_DELAY - 1));
volatile uint32_t qwerty_tim14_oc_arr =
    (TIM14_DELAY == 0 ? TIM14_CCR : TIM14_CCR + TIM14_PULSE *  TIM14_DELAY     );

void MX_TIM14_Init(void)
{
  htim14.Instance = TIM14;
  htim14.Init.Prescaler =  qwerty_tim_base_prescaler - 1;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = qwerty_tim14_oc_arr;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }


  /* TIM14 interrupt Init */
  HAL_NVIC_SetPriority(TIM14_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM14_IRQn);

}

void HAL_TIM14_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
  if(tim_baseHandle->Instance==TIM14)
  {
    __HAL_RCC_TIM14_CLK_ENABLE();
  }
}


