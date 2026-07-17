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
#include "gpio.h"

const struct_pin querty_sense[13] = {
  {NULL, 0x0000, 0x0000},             /* D0 is at VCC on typewriter. N/C */
  {QWERTY_D1_PORT,  QWERTY_D1_PIN,  0x0000},
  {QWERTY_D2_PORT,  QWERTY_D2_PIN,  0x0000},
  {QWERTY_D3_PORT,  QWERTY_D3_PIN,  0x0000},
  {QWERTY_D4_PORT,  QWERTY_D4_PIN,  0x0000},
  {NULL, 0x0000, 0x0000},             /* D5 is at GND on typewriter. Connected to MCU ground */
  {QWERTY_D6_PORT,  QWERTY_D6_PIN,  0x0000},
  {QWERTY_D7_PORT,  QWERTY_D7_PIN,  0x0000},
  {QWERTY_D8_PORT,  QWERTY_D8_PIN,  0x0000},
  {NULL, 0x0000, 0x0001},     /* F9  is FIXED LINE <=> CAPS LOCK. N/C */
  {QWERTY_F10_PORT, QWERTY_F10_PIN, 0x0002},     /* F10 is FIXED LINE <=> SHIFT */
  {QWERTY_D11_PORT, QWERTY_D11_PIN, 0x0000},
  {QWERTY_F12_PORT, QWERTY_F12_PIN, 0x0004},     /* F12 is FIXED LINE <=> CODE  */
};

volatile uint32_t gpio_pin_initial_pull;

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
#if defined(GPIO_INVERTED_MODE_INACTIVE_STATE_DIGITAL_OUTPUT)

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
  GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_MUX_PORT, &GPIO_InitStruct);
  // HAL_GPIO_WritePin(QWERTY_MUX_PORT, QWERTY_MUX_PIN, GPIO_PIN_RESET);
   /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
#endif

#ifdef QWERTY_D1_PIN
  GPIO_InitStruct.Pin = QWERTY_D1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = gpio_pin_initial_pull;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_D1_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(QWERTY_D1_PORT, QWERTY_D1_PIN, mux_it_mode_gpio_pin_edge0_state);
#endif

#ifdef QWERTY_D2_PIN
  GPIO_InitStruct.Pin = QWERTY_D2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = gpio_pin_initial_pull;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_D2_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(QWERTY_D2_PORT, QWERTY_D2_PIN, mux_it_mode_gpio_pin_edge0_state);
#endif

#ifdef QWERTY_D3_PIN
  GPIO_InitStruct.Pin = QWERTY_D3_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = gpio_pin_initial_pull;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_D3_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(QWERTY_D3_PORT, QWERTY_D3_PIN, mux_it_mode_gpio_pin_edge0_state);
#endif

#ifdef QWERTY_D4_PIN
  GPIO_InitStruct.Pin = QWERTY_D4_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = gpio_pin_initial_pull;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_D4_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(QWERTY_D4_PORT, QWERTY_D4_PIN, mux_it_mode_gpio_pin_edge0_state);
#endif

#ifdef QWERTY_D6_PIN
  GPIO_InitStruct.Pin = QWERTY_D6_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = gpio_pin_initial_pull;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_D6_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(QWERTY_D6_PORT, QWERTY_D6_PIN, mux_it_mode_gpio_pin_edge0_state);
#endif

#ifdef QWERTY_D7_PIN
  GPIO_InitStruct.Pin = QWERTY_D7_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = gpio_pin_initial_pull;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_D7_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(QWERTY_D7_PORT, QWERTY_D7_PIN, mux_it_mode_gpio_pin_edge0_state);
#endif

#ifdef QWERTY_D8_PIN
  GPIO_InitStruct.Pin = QWERTY_D8_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = gpio_pin_initial_pull;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_D8_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(QWERTY_D8_PORT, QWERTY_D8_PIN, mux_it_mode_gpio_pin_edge0_state);
#endif

#ifdef QWERTY_F9_PIN
  GPIO_InitStruct.Pin = QWERTY_F9_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = gpio_pin_initial_pull;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_F9_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(QWERTY_F9_PORT, QWERTY_F9_PIN, mux_it_mode_gpio_pin_edge0_state);
#endif

#ifdef QWERTY_F10_PIN
  GPIO_InitStruct.Pin = QWERTY_F10_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = gpio_pin_initial_pull;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_F10_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(QWERTY_F10_PORT, QWERTY_F10_PIN, mux_it_mode_gpio_pin_edge0_state);
#endif

#ifdef QWERTY_D11_PIN
  GPIO_InitStruct.Pin = QWERTY_D11_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = gpio_pin_initial_pull;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_D11_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(QWERTY_D11_PORT, QWERTY_D11_PIN, mux_it_mode_gpio_pin_edge0_state);
#endif

#ifdef QWERTY_F12_PIN
  GPIO_InitStruct.Pin = QWERTY_F12_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = gpio_pin_initial_pull;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_F12_PORT, &GPIO_InitStruct);
  HAL_GPIO_WritePin(QWERTY_F12_PORT, QWERTY_F12_PIN, mux_it_mode_gpio_pin_edge0_state);
#endif

}

#elif defined(GPIO_INVERTED_MODE_INACTIVE_STATE_ANALOG_INPUT)

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
  GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_MUX_PORT, &GPIO_InitStruct);
  // HAL_GPIO_WritePin(QWERTY_MUX_PORT, QWERTY_MUX_PIN, GPIO_PIN_RESET);
  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
  #endif

  #ifdef QWERTY_D1_PIN
  GPIO_InitStruct.Pin = QWERTY_D1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_D1_PORT, &GPIO_InitStruct);
  #endif

  #ifdef QWERTY_D2_PIN
  GPIO_InitStruct.Pin = QWERTY_D2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_D2_PORT, &GPIO_InitStruct);
  #endif

  #ifdef QWERTY_D3_PIN
  GPIO_InitStruct.Pin = QWERTY_D3_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_D3_PORT, &GPIO_InitStruct);
  #endif

  #ifdef QWERTY_D4_PIN
  GPIO_InitStruct.Pin = QWERTY_D4_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_D4_PORT, &GPIO_InitStruct);
  #endif

  #ifdef QWERTY_D6_PIN
  GPIO_InitStruct.Pin = QWERTY_D6_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_D6_PORT, &GPIO_InitStruct);
  #endif

  #ifdef QWERTY_D7_PIN
  GPIO_InitStruct.Pin = QWERTY_D7_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_D7_PORT, &GPIO_InitStruct);
  #endif

  #ifdef QWERTY_D8_PIN
  GPIO_InitStruct.Pin = QWERTY_D8_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_D8_PORT, &GPIO_InitStruct);
  #endif

  #ifdef QWERTY_F9_PIN
  GPIO_InitStruct.Pin = QWERTY_F9_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_F9_PORT, &GPIO_InitStruct);
  #endif

  #ifdef QWERTY_F10_PIN
  GPIO_InitStruct.Pin = QWERTY_F10_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_F10_PORT, &GPIO_InitStruct);
  #endif

  #ifdef QWERTY_D11_PIN
  GPIO_InitStruct.Pin = QWERTY_D11_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_D11_PORT, &GPIO_InitStruct);
  #endif

  #ifdef QWERTY_F12_PIN
  GPIO_InitStruct.Pin = QWERTY_F12_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(QWERTY_F12_PORT, &GPIO_InitStruct);
  #endif

}

#endif

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
