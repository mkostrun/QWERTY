/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f0xx_it.c
  * @brief   Interrupt Service Routines.
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "stm32f0xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */
volatile uint8_t  mux_it_edge=0;
volatile uint8_t  mux_it_mode;
volatile uint32_t mux_it_mode_gpio_pin_edge0_state;
volatile uint32_t mux_it_mode_gpio_pin_edge1_state;

volatile uint32_t qwerty_count_typewriter_base_pulses;
volatile uint32_t qwerty_count_typewriter_base_pulses_after_last_printed_character;
volatile uint32_t qwerty_count_typed_characters;

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVC_IRQn 0 */

  /* USER CODE END SVC_IRQn 0 */
  /* USER CODE BEGIN SVC_IRQn 1 */

  /* USER CODE END SVC_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel 4 and 5 interrupts.
  */
void DMA1_Channel4_5_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_usart2_rx);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance != TIM3)
    return;

  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) // Rising edge
  {

    if (edge1 == 0)
    {
      IC_Value1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
      edge1 = 1;
      period1 = IC_Value1 - previous_IC_Value1;
      previous_IC_Value1 = IC_Value1;

      __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, qwerty_tim3_oc_it_edge_trail);

      // software another trigger for one pulse on TIM14/CH1
      if (qwerty_output_counter)
      {

        // TIM14->CCMR1  = 0b1100000; /* output mode PWM1 */
        //TIM14->CCMR1  = 0b1110000; /* output mode PWM2 */
        TIM14->CNT    = 0;
        // TIM14->ARR    = qwerty_tim14_oc_arr;
        // TIM14->CCR1   = qwerty_tim14_oc_ccr;
        TIM14->CR1   |= TIM_CR1_CEN;
        // __HAL_TIM_ENABLE_IT  (&htim14, TIM_IT_CC1);
        // __HAL_TIM_ENABLE_IT  (&htim14, TIM_IT_UPDATE);
        // __HAL_TIM_CLEAR_IT   (&htim14, TIM_IT_CC1 | TIM_IT_UPDATE);
        // __HAL_TIM_ENABLE     (&htim14);
        qwerty_output_counter--;
        qwerty_count_typewriter_base_pulses_after_last_printed_character = 0;
        return;
      }

      qwerty_count_typewriter_base_pulses++;

      qwerty_count_typewriter_base_pulses_after_last_printed_character++;

      if (qwerty_count_typewriter_base_pulses_after_last_printed_character <
          QWERTY_MIN_BASE_PULSES_BETWEEN_CHARACTERS)
        return;

      if (qwerty_output_counter_have_update == 0)
        return;

      // this is to activate shift/code (N-1) base pulses before the
      // sense lines are trigered
      if (qwerty_output_counter_have_update > 1)
      {
        if (idx_f10_shift_ch)
        {
#if defined(GPIO_INVERTED_MODE_INACTIVE_STATE_ANALOG_INPUT)
          GPIO_InitTypeDef GPIO_InitStruct = {0};
          GPIO_InitStruct.Pin = QWERTY_F10_PIN;
          GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
          GPIO_InitStruct.Pull = gpio_pin_initial_pull;
          GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
          HAL_GPIO_Init(QWERTY_F10_PORT, &GPIO_InitStruct);
#endif
          HAL_GPIO_WritePin(QWERTY_F10_PORT, QWERTY_F10_PIN, mux_it_mode_gpio_pin_edge1_state);
        }

        if (idx_f12_code_ch)
        {
#if defined(GPIO_INVERTED_MODE_INACTIVE_STATE_ANALOG_INPUT)
          // flip line back to analog
          GPIO_InitTypeDef GPIO_InitStruct = {0};
          GPIO_InitStruct.Pin = QWERTY_F12_PIN;
          GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
          GPIO_InitStruct.Pull = gpio_pin_initial_pull;
          GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
          HAL_GPIO_Init(QWERTY_F12_PORT, &GPIO_InitStruct);
#endif
          HAL_GPIO_WritePin(QWERTY_F12_PORT, QWERTY_F12_PIN, mux_it_mode_gpio_pin_edge1_state);
        }

        qwerty_output_counter_have_update--;
        return;
      }

      // set everything for next character to be printed
      // reset counter to default:
      qwerty_output_counter = qwerty_output_counter_default;

      // set sense pin for the GPIO MUX if necessary
      idx_sense_ch = idx_sense_ch_new;

      // update One Pulse CCR and ARR if necessary
      TIM14->CR1   &= ~TIM_CR1_CEN;
      TIM14->CNT    = 0;
      TIM14->ARR    = qwerty_tim14_oc_arr;
      TIM14->CCR1   = qwerty_tim14_oc_ccr;
      TIM14->CR1   |= TIM_CR1_CEN;

      qwerty_output_counter_have_update = 0;
    }
    else
    {
      IC_Value1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
      pulse1 = IC_Value1 - previous_IC_Value1;
      __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, qwerty_tim3_oc_it_edge_lead);
      edge1 = 0;
    }

  } /*if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) // Rising edge*/

  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) // Rising edge
  {
    IC_Value3 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
    if (edge2 == 0)
    {
      period2 = IC_Value3 - previous_IC_Value3;
      previous_IC_Value3 = IC_Value3;

      // find delay
      delay2 = previous_IC_Value3 - previous_IC_Value1;
      __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_4, qwerty_tim3_oc_it_edge_trail);
      edge2 = 1;
    }
    else
    {
      pulse2 = IC_Value3 - previous_IC_Value3;
      __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_4, qwerty_tim3_oc_it_edge_lead);
      edge2 = 0;
    }
  }

}

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim3);
}

/**
 * @brief This function handles TIM14 global interrupt.
 */
void TIM14_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim14);
}

void HAL_TIM_OC_DelayElapsedCallback (TIM_HandleTypeDef *htim)
{
  if ((htim->Instance == TIM14) && (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) )
  {
    // rising edge interrupt (PWM2 -> reaching CCR)
#if defined(GPIO_INVERTED_MODE_INACTIVE_STATE_ANALOG_INPUT)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = querty_sense[idx_sense_ch].pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = gpio_pin_initial_pull;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(querty_sense[idx_sense_ch].port, &GPIO_InitStruct);
#endif
    HAL_GPIO_WritePin(querty_sense[idx_sense_ch].port,
                      querty_sense[idx_sense_ch].pin, mux_it_mode_gpio_pin_edge1_state);
    // HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
  }
}

void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM14)
  {
    // falling edge interrupt (PWM2 -> reaching ARR)

#if defined(GPIO_INVERTED_MODE_INACTIVE_STATE_DIGITAL_OUTPUT)
    // flip line back to edge0 (HIGI)
    HAL_GPIO_WritePin(querty_sense[idx_sense_ch].port,
                      querty_sense[idx_sense_ch].pin, mux_it_mode_gpio_pin_edge0_state);
#elif defined(GPIO_INVERTED_MODE_INACTIVE_STATE_ANALOG_INPUT)
    // flip line back to analog
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = querty_sense[idx_sense_ch].pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(querty_sense[idx_sense_ch].port, &GPIO_InitStruct);
#endif

    if (idx_f10_shift_ch)
    {
#if defined(GPIO_INVERTED_MODE_INACTIVE_STATE_ANALOG_INPUT)
      // flip line back to analog
      GPIO_InitTypeDef GPIO_InitStruct = {0};
      GPIO_InitStruct.Pin = QWERTY_F10_PIN;
      GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      HAL_GPIO_Init(QWERTY_F10_PORT, &GPIO_InitStruct);
#else
      HAL_GPIO_WritePin(QWERTY_F10_PORT, QWERTY_F10_PIN, mux_it_mode_gpio_pin_edge0_state);
#endif
      idx_f10_shift_ch = 0;
    }

    if (idx_f12_code_ch)
    {
#if defined(GPIO_INVERTED_MODE_INACTIVE_STATE_ANALOG_INPUT)
      GPIO_InitTypeDef GPIO_InitStruct = {0};
      GPIO_InitStruct.Pin = QWERTY_F12_PIN;
      GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      HAL_GPIO_Init(QWERTY_F12_PORT, &GPIO_InitStruct);
#else
      HAL_GPIO_WritePin(QWERTY_F12_PORT, QWERTY_F12_PIN, mux_it_mode_gpio_pin_edge0_state);
#endif
    }

    // if this was the last pulse triggered by TIM3, announce the typing stopped
    if (!qwerty_output_counter)
      qwerty_typing = 0;

    // stop TIM14 and enable interrupts for next run
    TIM14->CR1   &= ~TIM_CR1_CEN;
    __HAL_TIM_CLEAR_IT   (&htim14, TIM_IT_CC1 | TIM_IT_UPDATE);
    __HAL_TIM_ENABLE_IT  (&htim14, TIM_IT_CC1);
    __HAL_TIM_ENABLE_IT  (&htim14, TIM_IT_UPDATE);

  }
}


/* USER CODE BEGIN 2 */
/**
 * @brief This function handles EXTI line 0 and 1 interrupts.
 */
#ifdef QWERTY_MUX_PIN

void EXTI0_1_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(QWERTY_MUX_PIN);
}

/**
 * @brief Interrupt callback for GPIOs
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == QWERTY_MUX_PIN)
  {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin   = QWERTY_MUX_PIN;
    GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    if (mux_it_edge == 0)
    {
      // enter edge1 state:
      //  output pin set to -> mux_it_mode_gpio_pin_edge1_state
      //  edge of the interupt set for transition to edge0 state ->
      HAL_GPIO_WritePin(querty_sense[idx_sense_ch].port,
        querty_sense[idx_sense_ch].pin, mux_it_mode_gpio_pin_edge1_state);
      GPIO_InitStruct.Mode  = GPIO_MODE_IT_FALLING;
      mux_it_edge = 1;
    }
    else
    {
      // enter edge0 state:
      HAL_GPIO_WritePin(querty_sense[idx_sense_ch].port,
        querty_sense[idx_sense_ch].pin, mux_it_mode_gpio_pin_edge0_state);
      GPIO_InitStruct.Mode  = GPIO_MODE_IT_RISING;
      mux_it_edge = 0;
    }
    HAL_GPIO_Init(QWERTY_MUX_PORT, &GPIO_InitStruct);
  }
}

#endif


/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
