#include "main.h"

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim16;

uint32_t qwerty_tim_base_prescaler    = 20;

volatile uint8_t  qwerty_tim3_oc_mode      = TIM3_OC_MODE_POSITIVE, qwerty_tim3_oc_new_mode;
volatile uint32_t  qwerty_tim3_oc_it_edge_lead  = TIM_INPUTCHANNELPOLARITY_RISING;
volatile uint32_t  qwerty_tim3_oc_it_edge_trail = TIM_INPUTCHANNELPOLARITY_FALLING;

volatile uint8_t edge1=0;
volatile uint8_t edge2=0;
volatile uint32_t IC_Value1 = 0;
volatile uint32_t IC_Value2 = 0;
volatile uint32_t previous_IC_Value1 = 0;
volatile uint32_t previous_IC_Value2 = 0;
volatile uint32_t period1 = 0;
volatile uint32_t pulse1  = 0;
volatile uint32_t period2 = 0;
volatile uint32_t pulse2  = 0;
volatile uint32_t delay2  = 0;

/**
 * @brief TIM_Base MSP Initialization
 * This function configures the hardware resources used in this example
 * @param htim_base: TIM_Base handle pointer
 * @retval None
 */
void HAL_TIM3_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance!=TIM3)
    return;

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_TIM3_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /**TIM3 GPIO Configuration
   *    PA6     ------> TIM3_CH1
   *    PA7     ------> TIM3_CH2
   */
  GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM3;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* TIM3 interrupt Init */
  HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM3_IRQn);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  HAL_TIM3_Base_MspInit(htim_base);
  HAL_TIM14_Base_MspInit(htim_base);
  HAL_TIM17_Base_MspInit(htim_base);
}


/**
 * @brief TIM_Base MSP De-Initialization
 * This function freeze the hardware resources used in this example
 * @param htim_base: TIM_Base handle pointer
 * @retval None
 */
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
  if(htim_base->Instance==TIM3)
  {
    __HAL_RCC_TIM3_CLK_DISABLE();

    /**TIM3 GPIO Configuration
      *    PA6     ------> TIM3_CH1
      *    PA7     ------> TIM3_CH2
      */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6|GPIO_PIN_7);
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
  }
  else if(htim_base->Instance==TIM14)
  {
    __HAL_RCC_TIM14_CLK_DISABLE();
  }
  else if(htim_base->Instance == TIM17)
  {
    __HAL_RCC_TIM17_CLK_DISABLE();
  }

  return;
}

 /**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
void MX_TIM3_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  htim3.Instance = TIM3;
  //htim3.Init.Prescaler = 8000;
  htim3.Init.Prescaler = qwerty_tim_base_prescaler - 1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  edge1=0;
  edge2=0;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (qwerty_tim3_oc_mode == TIM3_OC_MODE_POSITIVE)
  {
    sConfigIC.ICPolarity = qwerty_tim3_oc_it_edge_lead;
  }
  else
  {
    sConfigIC.ICPolarity = qwerty_tim3_oc_it_edge_trail;
  }
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }

}

