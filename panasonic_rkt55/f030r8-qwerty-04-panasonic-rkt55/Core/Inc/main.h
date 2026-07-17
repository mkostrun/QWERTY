#ifndef __main_H
#define __main_H
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

#ifdef __cplusplus
extern "C" {
#endif

//
// typewriter states
//
#define TYPEWRITER_PAN_RKT55


#if defined(TYPEWRITER_PAN_RKT55)
  #define TYPEWRITER_MFG    "Panasonic"
  #define TYPEWRITER_MODEL  "RK-T55"
#endif

#define QWERTY_TYPEWRITER_SYSTEM_STATE_TYPEWRITER (1)
#define QWERTY_TYPEWRITER_SYSTEM_STATE_CLI        (0)
#define QWERTY_TYPEWRITER_SYSTEM_STATE_DEFAULT    QWERTY_TYPEWRITER_SYSTEM_STATE_TYPEWRITER
//#define QWERTY_TYPEWRITER_SYSTEM_STATE_DEFAULT    QWERTY_TYPEWRITER_SYSTEM_STATE_CLI

//
// Serial Port: Buffers
//
#define USART2_RX_DMA_BUFFER_SIZE (4096)
#define USART2_RX_CMD_BUFFER_SIZE (32)
#define EOF (-1)

//
// Typewriter output (printing) buffer
//
#define QWERTY_OUTPUT_BUFFER_LENGTH (512)

//
// Source Pulse Generation Parameters:
//
#define   TIM14_DELAY  (1L)       /* Cannot be less than one */
#define   TIM14_PULSE  (200L)
//#define   TIM14_CCR    (183L)
/**
  * Shortest delay of the pulse that starts at 1, and ends at TIM14_PULSE
  * from it: Should be modified so that there is no overflow: A symbol
  * triggering another symbol at the output because of jitter between
  * STM32 generated delays, and TYPEWRITER internal delays
  **/
#define   TIM14_CCR    (166L)



#define QWERTY_MODE_POSITIVE                      (0x01)
#define QWERTY_MODE_INVERTED                      (0x00)
#define QWERTY_OUTPUT_COUNTER_DEFAULT             (4L)
#define QWERTY_MIN_BASE_PULSES_BETWEEN_CHARACTERS (6L)
#define QWERTY_BASE_PULSES_CODE_SHIFT_AHEAD       (4L)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f0xx_hal.h"

#include "gpio.h"
#include "serial.h"
#include "stm32f0xx_it.h"
#include "tim14.h"
#include "tim.h"
#include "typewriter.h"

extern volatile uint32_t  qwerty_output_counter;
extern volatile uint32_t  qwerty_output_counter_new;
extern volatile uint32_t  qwerty_output_counter_default;
extern volatile uint8_t   qwerty_output_counter_have_update;
extern volatile uint8_t   qwerty_typing;

extern volatile uint8_t   idx_drive_ch;
extern volatile uint8_t   idx_drive_ch_new;
extern volatile uint8_t   idx_sense_ch;
extern volatile uint8_t   idx_sense_ch_new;
extern volatile uint8_t   idx_f10_shift_ch;
extern volatile uint8_t   idx_f12_code_ch;

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */
#define GOBBLESPACES {\
  while(*(usart2_rx_cmd_buffer+idx_cmd) == ' ')   \
  {                                               \
    idx_cmd++;                                    \
  }                                               \
  if (*(usart2_rx_cmd_buffer+idx_cmd) == '\0')  \
    goto _exit;                                 \
  if (*(usart2_rx_cmd_buffer+idx_cmd) == '\r')  \
    goto _exit;                                 \
  if (*(usart2_rx_cmd_buffer+idx_cmd) == '\n')  \
    goto _exit;                                 \
  if (*(usart2_rx_cmd_buffer+idx_cmd) == ';')   \
    goto _process_more;                         \
}

#define GOBBLEDIGITS {\
  if ((*(usart2_rx_cmd_buffer+idx_cmd) - '0' >= 0)        \
  && (*(usart2_rx_cmd_buffer+idx_cmd) - '0' <= 9))        \
  {                                                       \
    idx_cmd++;                                            \
    while ( (*(usart2_rx_cmd_buffer+idx_cmd) - '0' >= 0)  \
         && (*(usart2_rx_cmd_buffer+idx_cmd) - '0' <= 9)) \
    {                                                     \
      idx_cmd++;                                          \
    }                                                     \
  }                                                       \
}

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif
