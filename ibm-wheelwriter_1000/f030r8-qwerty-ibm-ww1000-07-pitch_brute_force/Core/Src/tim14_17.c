#include "main.h"

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim14;
TIM_HandleTypeDef htim17;
uint32_t qwerty_tim17_base_prescaler = 2000;

// volatile uint32_t qwerty_tim14_oc_delay = TIM14_CCR + 200 *  TIME14_DELAY ;
// volatile uint32_t qwerty_tim14_oc_pulse = TIM14_CCR + 200 * (TIME14_DELAY + 1) ;

// volatile uint32_t qwerty_tim14_oc_ccr = 1;
// volatile uint32_t qwerty_tim14_oc_arr = 183;
volatile uint32_t qwerty_tim14_oc_ccr=0;
volatile uint32_t qwerty_tim14_oc_arr=0;
volatile uint32_t qwerty_tim14_oc_arr2=0;
// volatile uint32_t qwerty_tim17_oc_ccr=0;
// volatile uint32_t qwerty_tim17_oc_arr=0;

void MX_TIM14_Init(void)
{
  htim14.Instance = TIM14;
  htim14.Init.Prescaler =  qwerty_tim_base_prescaler - 1;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 0xffff;
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

void MX_TIM17_Init(void)
{
  htim17.Instance = TIM17;
  htim17.Init.Prescaler =  qwerty_tim17_base_prescaler - 1;
  htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim17.Init.Period = 0xffff;
  htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }


  /* TIM14 interrupt Init */
  HAL_NVIC_SetPriority(TIM17_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM17_IRQn);
}


void HAL_TIM17_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{
  if(tim_baseHandle->Instance==TIM17)
  {
    __HAL_RCC_TIM17_CLK_ENABLE();
  }
}
