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

  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) // Rising edge
  {

    IC_Value1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

    if (edge1 == 0)
    {
      edge1 = 1;
      period1 = IC_Value1 - previous_IC_Value1;
      previous_IC_Value1 = IC_Value1;

      __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, qwerty_tim3_oc_it_edge_trail);

      if (code_first)
        return;

      shift_first = 0;

      if ( idx_shift_ch )
      {
        int32_t sense_ch = 7;
        // switch pin to digital output
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = querty_sense[sense_ch].pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(querty_sense[sense_ch].port, &GPIO_InitStruct);
        // write edge1 to digital output pin
        HAL_GPIO_WritePin(querty_sense[sense_ch].port,
                          querty_sense[sense_ch].pin, mux_it_mode_gpio_pin_edge1_state);
        if (idx_shift_ch > 1)
        {
          idx_shift_ch --;
          return;
        }
      }

    }
    else
    {
      //
      // trailing edge of D0 pulse: initiate drive pulse on sense channel
      //
      pulse1 = IC_Value1 - previous_IC_Value1;
      __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, qwerty_tim3_oc_it_edge_lead);
      edge1 = 0;

      if (code_first)
        return;

      if ( idx_shift_ch )
      {
        int32_t sense_ch = 7;
        // flip line back to analog
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = querty_sense[sense_ch].pin;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(querty_sense[sense_ch].port, &GPIO_InitStruct);
      }

      // software another trigger for one pulse on TIM14/CH1
      if (qwerty_output_counter1)
      {
        //
        __HAL_TIM_ENABLE_IT  (&htim14, TIM_IT_CC1);
        __HAL_TIM_ENABLE_IT  (&htim14, TIM_IT_UPDATE);
        __HAL_TIM_CLEAR_IT   (&htim14, TIM_IT_CC1 | TIM_IT_UPDATE);

        TIM14->CNT    = 0;
        TIM14->ARR    = qwerty_tim14_oc_arr2;
        TIM14->CCR1   = qwerty_tim14_oc_ccr;
        if (qwerty_output_counter1 == 1)
        {
          TIM14->ARR    = qwerty_tim14_oc_arr;
        }
        TIM14->CR1   |= TIM_CR1_CEN;

        qwerty_output_counter1--;

        qwerty_count_typewriter_base_pulses_after_last_printed_character = 0;

        return;
      }

      qwerty_count_typewriter_base_pulses++;

      qwerty_count_typewriter_base_pulses_after_last_printed_character++;

      if (qwerty_count_typewriter_base_pulses_after_last_printed_character <
        QWERTY_MIN_BASE_PULSES_BETWEEN_CHARACTERS)
        return;

      if (qwerty_output_counter1_have_update == 0)
        return;

      // set everything for next character to be printed
      // reset counter to default:
      qwerty_output_counter1 = qwerty_output_counter_default - 1;

      // set sense pin for the GPIO MUX
      idx_sense_ch = idx_sense_ch_new;

      // update One Pulse CCR and ARR
      __HAL_TIM_ENABLE_IT  (&htim14, TIM_IT_CC1);
      __HAL_TIM_ENABLE_IT  (&htim14, TIM_IT_UPDATE);
      __HAL_TIM_CLEAR_IT   (&htim14, TIM_IT_CC1 | TIM_IT_UPDATE);

      TIM14->CNT    = 0;
      TIM14->ARR    = qwerty_tim14_oc_arr;
      TIM14->CCR1   = qwerty_tim14_oc_ccr;
      TIM14->CR1   |= TIM_CR1_CEN;

      qwerty_output_counter1_have_update = 0;
    }

  } /*if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) */

  if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
  {
    // Channel 2:
    IC_Value2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);

    if (edge2 == 0)
    {
      // Leading edge:
      //    Used for measuring the period of base pulses
      edge2 = 1;
      period2 = IC_Value2 - previous_IC_Value2;
      previous_IC_Value2 = IC_Value2;
      delay2 = previous_IC_Value2 - previous_IC_Value1;

      __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, qwerty_tim3_oc_it_edge_trail);

      if (shift_first)
        return;

      code_first = 0;

      if ( idx_code_ch )
      {
        int32_t sense_ch = 7;
        // switch pin to digital output
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = querty_sense[sense_ch].pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(querty_sense[sense_ch].port, &GPIO_InitStruct);
        // write edge1 to digital output pin
        HAL_GPIO_WritePin(querty_sense[sense_ch].port,
                          querty_sense[sense_ch].pin, mux_it_mode_gpio_pin_edge1_state);
      }
    }
    else
    {
      // Trailing edge:
      //  Used for:
      //    1. measuring the pulse width
      //    2. initiate sense channel through GPIO MUX for drive pulses 10:12
      pulse2 = IC_Value2 - previous_IC_Value2;
      __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_2, qwerty_tim3_oc_it_edge_lead);
      edge2 = 0;

      if (shift_first)
        return;

      if ( idx_code_ch )
      {
        int32_t sense_ch = 7;
        // flip line back to analog
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        GPIO_InitStruct.Pin = querty_sense[sense_ch].pin;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(querty_sense[sense_ch].port, &GPIO_InitStruct);
        if (idx_code_ch > 1)
        {
          idx_code_ch --;
          return;
        }
      }

      // software another trigger for one pulse on TIM14/CH1
      if (qwerty_output_counter2)
      {

        __HAL_TIM_ENABLE_IT  (&htim14, TIM_IT_CC1);
        __HAL_TIM_ENABLE_IT  (&htim14, TIM_IT_UPDATE);
        __HAL_TIM_CLEAR_IT   (&htim14, TIM_IT_CC1 | TIM_IT_UPDATE);

        TIM14->CNT    = 0;
        TIM14->ARR    = qwerty_tim14_oc_arr2;
        if (qwerty_output_counter2 == 1)
        {
          TIM14->ARR  = qwerty_tim14_oc_arr;
        }
        TIM14->CCR1   = qwerty_tim14_oc_ccr;
        TIM14->CR1   |= TIM_CR1_CEN;

        qwerty_output_counter2--;

        qwerty_count_typewriter_base_pulses_after_last_printed_character = 0;

        return;
      }

      if (qwerty_count_typewriter_base_pulses_after_last_printed_character <
        QWERTY_MIN_BASE_PULSES_BETWEEN_CHARACTERS)
        return;

      if (qwerty_output_counter2_have_update == 0)
        return;

      // set everything for next character to be printed
      // reset counter to default:
      qwerty_output_counter2 = qwerty_output_counter_default - 1;

      // set sense pin for the GPIO MUX if necessary
      idx_sense_ch = idx_sense_ch_new;

      // update One Pulse CCR and ARR if necessary
      __HAL_TIM_ENABLE_IT  (&htim14, TIM_IT_CC1);
      __HAL_TIM_ENABLE_IT  (&htim14, TIM_IT_UPDATE);
      __HAL_TIM_CLEAR_IT   (&htim14, TIM_IT_CC1 | TIM_IT_UPDATE);

      TIM14->CNT    = 0;
      TIM14->ARR    = qwerty_tim14_oc_arr;
      TIM14->CCR1   = qwerty_tim14_oc_ccr;
      TIM14->CR1   |= TIM_CR1_CEN;

      qwerty_output_counter2_have_update = 0;
    }
  }

}

void TIM3_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim3);
}

void TIM14_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim14);
}

void HAL_TIM_OC_DelayElapsedCallback (TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM14)
  {
#ifdef QWERTY_MUX_PIN
    HAL_GPIO_WritePin(QWERTY_MUX_PORT, QWERTY_MUX_PIN, GPIO_PIN_RESET);
#endif

    // rising edge interrupt (PWM2 -> reaching CCR)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = querty_sense[idx_sense_ch].pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(querty_sense[idx_sense_ch].port, &GPIO_InitStruct);

    HAL_GPIO_WritePin(querty_sense[idx_sense_ch].port,
                      querty_sense[idx_sense_ch].pin, mux_it_mode_gpio_pin_edge1_state);
    return;
  }
}

void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM14)
  {
    // falling edge interrupt (PWM2 -> reaching ARR)
#ifdef QWERTY_MUX_PIN
    HAL_GPIO_WritePin(QWERTY_MUX_PORT, QWERTY_MUX_PIN, GPIO_PIN_SET);
#endif

    // flip line back to analog
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = querty_sense[idx_sense_ch].pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(querty_sense[idx_sense_ch].port, &GPIO_InitStruct);

    // if this was the last pulse triggered by TIM3, announce the typing stopped
    if ( (!qwerty_output_counter1) && (!qwerty_output_counter2)
      && (!qwerty_output_counter1_have_update) && (!qwerty_output_counter2_have_update)

    )
    {
      qwerty_typing = 0;
      idx_shift_ch  = 0;
      idx_code_ch   = 0;
    }

    // stop TIM14 and enable interrupts for next run
    TIM14->CR1   &= ~TIM_CR1_CEN;

    return;
  }

}

/**
 * @brief This function handles EXTI line 4 to 15 interrupts.
 */
void EXTI4_15_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
}

/**
   * @brief This function handles EXTI line 4 to 15 interrupts.
  **/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (GPIO_Pin == QWERTY_DL_PIN)
  {
    if (gpio_dl_pin_edge == 0)
    {
      //
      // DL: Falling edge
      //
      gpio_dl_pin_edge = 1;

      // change polarity for next interrupt
      GPIO_InitStruct.Pin = QWERTY_DL_PIN;
      GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      HAL_GPIO_Init(QWERTY_DL_PORT, &GPIO_InitStruct);

      // SL output to 0: ACTIVE
      GPIO_InitStruct.Pin = QWERTY_SL_PIN;
      GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
      GPIO_InitStruct.Pull  = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      HAL_GPIO_Init(QWERTY_SL_PORT, &GPIO_InitStruct);
      HAL_GPIO_WritePin(QWERTY_SL_PORT, QWERTY_SL_PIN, GPIO_PIN_RESET);
    }
    else
    {
      //
      // DL: Rising edge
      //
      gpio_dl_pin_edge = 0;
      qwerty_pitch_rising_edge_counter++;

      //
      // DL : change polarity for next interrupt
      //
      GPIO_InitStruct.Pin = QWERTY_DL_PIN;
      GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      HAL_GPIO_Init(QWERTY_DL_PORT, &GPIO_InitStruct);

      if ( (qwerty_pitch_set==2) && (qwerty_pitch_rising_edge_counter<14) )
      {
        return;
      }

      if (!qwerty_pitch_set)
      {
        //
        // Follow what is going on DL and dump it on SL line
        //
#if (QWERTY_PITCH_SWITCH_MODE == QWERTY_PITCH_SWITCH_MODE_DIGITAL)
        // SL output to 1: INACTIBE
        HAL_GPIO_WritePin(QWERTY_SL_PORT, QWERTY_SL_PIN, GPIO_PIN_SET);
#else
        // flip line back to analog
        GPIO_InitStruct.Pin = QWERTY_SL_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(QWERTY_SL_PORT, &GPIO_InitStruct);
#endif
        return;
      }


      if ( (qwerty_pitch_rising_edge_counter>1)  && (qwerty_pitch_rising_edge_counter<6) )
      {
        if(qwerty_pitch_ptr[qwerty_pitch_rising_edge_counter - 2])
        {
#if (QWERTY_PITCH_SWITCH_MODE == QWERTY_PITCH_SWITCH_MODE_DIGITAL)
          // SL output to 1: INACTIBE
          HAL_GPIO_WritePin(QWERTY_SL_PORT, QWERTY_SL_PIN, GPIO_PIN_SET);
#else
          // flip line back to analog
          GPIO_InitStruct.Pin = QWERTY_SL_PIN;
          GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
          GPIO_InitStruct.Pull = GPIO_NOPULL;
          GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
          // SL output to 1: INACTIBE as Analog Input -> PullUp is on WW sense light (SL) line
          HAL_GPIO_Init(QWERTY_SL_PORT, &GPIO_InitStruct);
#endif
        }
        return;
      }

      if ( (qwerty_pitch_rising_edge_counter>8)  && (qwerty_pitch_rising_edge_counter<13) )
      {
        if(qwerty_pitch_ptr[qwerty_pitch_rising_edge_counter - 9])
        {
#if (QWERTY_PITCH_SWITCH_MODE == QWERTY_PITCH_SWITCH_MODE_DIGITAL)
          // SL output to 1: INACTIBE
          HAL_GPIO_WritePin(QWERTY_SL_PORT, QWERTY_SL_PIN, GPIO_PIN_SET);
#else
          // flip line back to analog
          GPIO_InitStruct.Pin = QWERTY_SL_PIN;
          GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
          GPIO_InitStruct.Pull = GPIO_NOPULL;
          GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
          // SL output to 1: INACTIBE as Analog Input -> PullUp is on WW sense light (SL) line
          HAL_GPIO_Init(QWERTY_SL_PORT, &GPIO_InitStruct);
#endif
        }
        return;
      }

      //
      // Follow what is going on DL and dump it on SL line
      //
#if (QWERTY_PITCH_SWITCH_MODE == QWERTY_PITCH_SWITCH_MODE_DIGITAL)
      // SL output to 1: INACTIBE
      HAL_GPIO_WritePin(QWERTY_SL_PORT, QWERTY_SL_PIN, GPIO_PIN_SET);
#else
      // flip line back to analog
      GPIO_InitStruct.Pin = QWERTY_SL_PIN;
      GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      HAL_GPIO_Init(QWERTY_SL_PORT, &GPIO_InitStruct);
#endif

    }
  }
}
