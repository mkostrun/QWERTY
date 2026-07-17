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

// #define TYPEWRITER_PAN_RKT55
#define TYPEWRITER_IBM_WW1000

#define TYPEWRITER_FIRMWARE_VERSION "7.0"

#define TYPEWRITER_MFG    "IBM"
#define TYPEWRITER_MODEL  "WheelWriter 1000"

#define QWERTY_OUTPUT_BUFFER_LENGTH (512L)

#define QWERTY_TYPEWRITER_SYSTEM_STATE_TYPEWRITER (1)
#define QWERTY_TYPEWRITER_SYSTEM_STATE_CLI        (0)
#define QWERTY_TYPEWRITER_SYSTEM_STATE_DEFAULT    QWERTY_TYPEWRITER_SYSTEM_STATE_TYPEWRITER
//#define QWERTY_TYPEWRITER_SYSTEM_STATE_DEFAULT    QWERTY_TYPEWRITER_SYSTEM_STATE_CLI

#define QWERTY_OUTPUT_COUNTER_DEFAULT             (2L)
#define QWERTY_MIN_BASE_PULSES_BETWEEN_CHARACTERS (5L)
#define QWERTY_INDEX_DRIVE_SPLIT (7)   /* 7 for: {0:6}, {7:d} per original design */

#define QWERTY_PITCH_SWITCH_MODE_ANALOG           (0L)
#define QWERTY_PITCH_SWITCH_MODE_DIGITAL          (1L)
// #define QWERTY_PITCH_SWITCH_MODE QWERTY_PITCH_SWITCH_MODE_DIGITAL
#define QWERTY_PITCH_SWITCH_MODE QWERTY_PITCH_SWITCH_MODE_ANALOG


//#define TIM14_DELTA_FUDGE_ARR (-1000)
//#define TIM14_DELTA_FUDGE_CCR (-500)
#define TIM14_DELTA_FUDGE_ARR (-7000)
#define TIM14_DELTA_FUDGE_CCR (-7000)


#define QWERTY_MODE_POSITIVE                      (0x01)
#define QWERTY_MODE_INVERTED                      (0x00)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f0xx_hal.h"

#include "gpio.h"
#include "serial.h"
#include "stm32f0xx_it.h"
#include "tim14_17.h"
#include "tim.h"
#include "typewriter.h"

extern volatile uint32_t  qwerty_output_counter_default;
extern volatile uint32_t  qwerty_output_counter_new;
extern volatile uint32_t  qwerty_output_counter;
extern volatile uint32_t  qwerty_output_counter1; // drive[0:6]
extern volatile uint32_t  qwerty_output_counter2; // drive[7:e]
extern volatile uint32_t  qwerty_output_counter3; // shift/code
extern volatile uint8_t   qwerty_output_counter1_have_update;
extern volatile uint8_t   qwerty_output_counter2_have_update;
extern volatile uint8_t   qwerty_output_counter3_have_update;
extern volatile uint8_t   qwerty_typing;
extern volatile uint32_t  qwerty_pitch_rising_edge_counter;
extern volatile uint8_t   qwerty_pitch_set;
extern volatile uint8_t  *qwerty_pitch_ptr;

extern const int32_t qwerty_pulse_ccr_drive[];
extern const int32_t qwerty_pulse_arr_drive[];
extern const int32_t qwerty_pulse_arr2_drive[];

extern volatile uint8_t   idx_drive_ch;
extern volatile uint8_t   idx_drive_ch_new;
extern volatile uint8_t   idx_sense_ch;
extern volatile uint8_t   idx_sense_ch_new;
extern volatile uint8_t   idx_shift_ch;
extern volatile uint8_t   idx_code_ch;
extern volatile uint8_t   shift_first;
extern volatile uint8_t   code_first;


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
