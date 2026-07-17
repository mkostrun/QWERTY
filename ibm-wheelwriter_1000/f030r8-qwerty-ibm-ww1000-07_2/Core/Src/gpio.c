/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

const struct_pin querty_sense[QWERTY_NUM_SENSE] = {
  {QWERTY_S0_PORT,  QWERTY_S0_PIN,  0x0000},
  {QWERTY_S1_PORT,  QWERTY_S1_PIN,  0x0000},
  {QWERTY_S2_PORT,  QWERTY_S2_PIN,  0x0000},
  {QWERTY_S3_PORT,  QWERTY_S3_PIN,  0x0000},
  {QWERTY_S4_PORT,  QWERTY_S4_PIN,  0x0000},
  {QWERTY_S5_PORT,  QWERTY_S5_PIN,  0x0000},
  {QWERTY_S6_PORT,  QWERTY_S6_PIN,  0x0000},
  {QWERTY_S7_PORT,  QWERTY_S7_PIN,  0x0000},
};

volatile uint8_t gpio_dl_pin_edge = 0;

// GPIO_TypeDef * sense_port[QWERTY_NUM_SENSE] =
// {
//   GPIOA,
//   GPIOB,
//   GPIOB,
//   GPIOB,
//   NULL,
//   NULL,
//   NULL,
//   NULL,
//   NULL,
//   NULL
// };
// uint16_t sense_pin [QWERTY_NUM_SENSE] =;
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
     PA2   ------> USART2_TX
     PA3   ------> USART2_RX
*/
/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
  //     GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  //     GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  //     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  //     GPIO_InitStruct.Alternate = GPIO_AF4_TIM14;
  //     HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


#ifdef QWERTY_MUX_PIN
  GPIO_InitStruct.Pin   = QWERTY_MUX_PIN;
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_MUX_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(QWERTY_MUX_PORT, QWERTY_MUX_PIN, GPIO_PIN_SET);
#endif

#ifdef QWERTY_S0_PIN
  GPIO_InitStruct.Pin = QWERTY_S0_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_S0_PORT, &GPIO_InitStruct);
#endif

#ifdef QWERTY_S1_PIN
  GPIO_InitStruct.Pin = QWERTY_S1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_S1_PORT, &GPIO_InitStruct);
#endif

#ifdef QWERTY_S2_PIN
  GPIO_InitStruct.Pin = QWERTY_S2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_S2_PORT, &GPIO_InitStruct);
#endif

#ifdef QWERTY_S3_PIN
  GPIO_InitStruct.Pin = QWERTY_S3_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_S3_PORT, &GPIO_InitStruct);
#endif

#ifdef QWERTY_S4_PIN
  GPIO_InitStruct.Pin = QWERTY_S4_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_S4_PORT, &GPIO_InitStruct);
#endif

#ifdef QWERTY_S5_PIN
  GPIO_InitStruct.Pin = QWERTY_S5_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_S5_PORT, &GPIO_InitStruct);
#endif

#ifdef QWERTY_S6_PIN
  GPIO_InitStruct.Pin = QWERTY_S6_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_S6_PORT, &GPIO_InitStruct);
#endif

#ifdef QWERTY_S7_PIN
  GPIO_InitStruct.Pin = QWERTY_S7_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_S7_PORT, &GPIO_InitStruct);
#endif

/**
  *  PITCH SWITCH:
  *     SL -> Sense Light Pin: Input from Typewriter Optical Block
  *     DL -> Drive Light Pin: Output to Typewriter Optical Block Analyzer
  *
  **/
#ifdef QWERTY_DL_PIN
  GPIO_InitStruct.Pin = QWERTY_DL_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_DL_PORT, &GPIO_InitStruct);

  /**
    * EXTI interrupt init: EXTI8 for PB8
    **/
  gpio_dl_pin_edge = 0;
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ  (EXTI4_15_IRQn);
#endif

#ifdef QWERTY_SL_PIN
  GPIO_InitStruct.Pin = QWERTY_SL_PIN;
  #if (QWERTY_PITCH_SWITCH_MODE == QWERTY_PITCH_SWITCH_MODE_DIGITAL)
  GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull  = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_SL_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(QWERTY_SL_PORT, QWERTY_SL_PIN, GPIO_PIN_SET);
  #else
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_SL_PORT, &GPIO_InitStruct);
  #endif
#endif

}

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
