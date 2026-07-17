/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opendrive.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

const int32_t qwerty_pulse_ccr_drive[]=
{
  /* 0,7 */      1,
  /* 1,8 */   2079 - 100,
  /* 2,9 */   4051 - 100,
  /* 3,a */   6023 - 100,
  /* 4,b */   7995 - 100,
  /* 5,c */   9967 - 100,
  /* 6,d */  11939 - 100,
  /* 7,  */  13911 - 100,
  /* 8,  */  15883 - 100,
  /* 9,  */  17855 - 100,
};

const int32_t qwerty_pulse_arr_drive[]=
{
  /* 0,7 */  5748 + 1000,
  /* 1,8 */  9767 - 1000,
  /* 2,9 */ 11739 - 1000,
  /* 3,a */ 13711 - 1000,
  /* 4,b */ 15683 - 1000,
  /* 5,c */ 17655 - 1000,
  /* 6,d */ 19627 - 1000,
  /* 7,  */ 21599 - 1000,
  /* 8,  */ 23571 - 1000,
  /* 9,  */ 25543 - 1000,
};

const int32_t qwerty_pulse_arr2_drive[]=
{
  /* 0,7 */   1916,
  /* 1,8 */   3856,
  /* 2,9 */   5828,
  /* 3,a */   7800,
  /* 4,b */   9772,
  /* 5,c */  11744,
  /* 6,d */  13716,
  /* 7,  */  15688,
  /* 8,  */  17660,
  /* 9,  */  19632,
};

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */
volatile uint32_t qwerty_output_counter_new;
volatile uint32_t qwerty_output_counter_default;
volatile uint32_t qwerty_output_counter1;
volatile uint32_t qwerty_output_counter2;
volatile uint32_t qwerty_output_counter3;
volatile uint8_t  qwerty_output_counter1_have_update;
volatile uint8_t  qwerty_output_counter2_have_update;
volatile uint8_t  qwerty_output_counter3_have_update;
volatile uint8_t  qwerty_typing = 0;
volatile uint32_t qwerty_pitch_rising_edge_counter;
volatile uint8_t  qwerty_pitch_ignore_dl;

volatile uint16_t qwerty_pitch_state = QWERTY_PITCH_SWITCH_12P;
volatile uint32_t qwerty_dl_tick;
volatile uint32_t qwerty_dl_time_elapsed_ms;

volatile uint8_t   idx_code_ch=0;
volatile uint8_t   idx_shift_ch=0;
volatile uint8_t   idx_drive_ch=0;
volatile uint8_t   idx_drive_ch_new=0;
volatile uint8_t   idx_sense_ch=0xff;
volatile uint8_t   idx_sense_ch_new=0xff;
volatile uint8_t   shift_first=0;
volatile uint8_t   code_first=0;

uint16_t idx_cmd=0, len_cmd=0;
int32_t  rcv_byte;
uint32_t new_pwm_period, new_pwm_pulse;
char msg[256];
data symbol1;

// private variables for parsing esc codes:
uint8_t iwait_esc_lmar        = 0;
uint8_t iwait_esc_rmar        = 0;
uint8_t iwait_esc_underline   = 0;
uint8_t iwait_esc_just        = 0;
uint8_t iwait_esc_par_indent  = 0;
uint8_t iwait_esc_tab_set     = 0;
uint8_t iwait_esc_term_set     = 0;

// private variables that determine the state of typewriter :
int16_t qwerty_page_lmar;
int16_t qwerty_page_rmar;

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_DMA_Init(void);

/* USER CODE BEGIN PFP */
//volatile int8_t qwerty_state = QWERTY_STATE_QUERY;
// volatile int8_t qwerty_state = QWERTY_STATE_WHISPER;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /**
    Init typewriter variables
    */
  // symbol[0].val = 0x0027;

  /**
   * TIM3 OUTPUT CAPTURE MODE USING
   *                INPUTS:
   *                    CH3 <=> PC8
   *                      TO_EDGE1 INTERRUPT CREATES SOFTWARE TRIGGER FOR TIM14/ONE PULSE MODE
   *                    CH4 <=> PC9
   * TO_EDGE1 TRANSITION:
   *   TIM3_OC_MODE_POSITIVE QWERTY_MODE_POSITIVE
   *   TIM3_OC_MODE_INVERTED QWERTY_MODE_INVERTED
   **/
  qwerty_tim3_oc_mode = QWERTY_MODE_INVERTED;
  // qwerty_tim3_oc_mode = QWERTY_MODE_POSITIVE;

  /**
   * GPIO/MUX MODE:
   *   MUX_IT_MODE_POSITIVE QWERTY_MODE_POSITIVE
   *   MUX_IT_MODE_INVERTED QWERTY_MODE_INVERTED
   **/
  mux_it_mode           = QWERTY_MODE_INVERTED;
  // mux_it_mode           = QWERTY_MODE_POSITIVE;

  // timers: 1clock = 48MHz/20 = 2.4Mhz <-> 0.4166 us,
  qwerty_tim_base_prescaler   = 20;

  qwerty_typewriter_system_state = QWERTY_TYPEWRITER_SYSTEM_STATE_DEFAULT;

_restart:

  // TIM3: output capture
  if (qwerty_tim3_oc_mode == TIM3_OC_MODE_POSITIVE)
  {
    qwerty_tim3_oc_it_edge_lead  = TIM_INPUTCHANNELPOLARITY_RISING;
    qwerty_tim3_oc_it_edge_trail = TIM_INPUTCHANNELPOLARITY_FALLING;
  }
  else
  {
    qwerty_tim3_oc_it_edge_lead  = TIM_INPUTCHANNELPOLARITY_FALLING;
    qwerty_tim3_oc_it_edge_trail = TIM_INPUTCHANNELPOLARITY_RISING;
  }

  edge1 = 0;
  edge2 = 0;
  previous_IC_Value1 = IC_Value1 = 0;
  previous_IC_Value2 = IC_Value2 = 0;
  period1 = 0;
  pulse1  = 0;
  period2 = 0;
  pulse2  = 0;
  delay2  = 0;
  qwerty_output_counter_default       = QWERTY_OUTPUT_COUNTER_DEFAULT;
  qwerty_output_counter_new           = 0;
  qwerty_output_counter1              = 0;
  qwerty_output_counter2              = 0;
  qwerty_output_counter1_have_update  = 0;
  qwerty_output_counter2_have_update  = 0;
  qwerty_pitch_rising_edge_counter    = 0;
  qwerty_pitch_ignore_dl = 0;
  qwerty_dl_tick = HAL_GetTick();
  idx_code_ch=0;
  idx_shift_ch=0;
  idx_drive_ch=0;
  idx_drive_ch_new=0;
  idx_sense_ch=0xff;
  idx_sense_ch_new=0xff;
  shift_first = 0;
  code_first = 0;

  mux_it_edge = 0;
  if (mux_it_mode == QWERTY_MODE_INVERTED)
  {
    mux_it_mode_gpio_pin_edge0_state = GPIO_PIN_SET;
    mux_it_mode_gpio_pin_edge1_state = GPIO_PIN_RESET;
  }
  else
  {
    mux_it_mode_gpio_pin_edge0_state = GPIO_PIN_RESET;
    mux_it_mode_gpio_pin_edge1_state = GPIO_PIN_SET;
  }


  /* USER CODE END 1 */
  qwerty_count_typewriter_base_pulses_after_last_printed_character = 0;

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  MX_TIM14_Init();
  MX_TIM17_Init();

  /* USER CODE BEGIN 2 */
  HAL_TIM_IC_Start_IT (&htim3, TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT (&htim3, TIM_CHANNEL_2);

  HAL_UART_Receive_DMA (&huart2, usart2_rx_dma_buffer, USART2_RX_DMA_BUFFER_SIZE);
  /* USER CODE END 2 */

  qwerty_count_typewriter_base_pulses = 0;
  qwerty_count_typewriter_base_pulses_after_last_printed_character = 0;
  qwerty_count_typed_characters = 0;

  // initialize typewriter output state variables and counters
  qwerty_output_buffer_start          = 0;
  qwerty_output_buffer_end            = 0;
  qwerty_output_buffer_bold           = 0;
  qwerty_output_buffer_underline      = 0;
  qwerty_output_buffer_line_spacing   = 0;
  qwerty_output_buffer_kbd_2          = 0;
  qwerty_output_buffer_mode           = 0;
  qwerty_output_buffer_esc_received   = 0;
  qwerty_output_buffer_null_received  = 0;
  qwerty_output_buffer_just           = 0;
  idx_shift_ch                        = 0;

  int16_t qwerty_page_lmar = -1;
  int16_t qwerty_page_rmar = -1;

  print_serial("QWERTY FIRMWARE " TYPEWRITER_FIRMWARE_VERSION " FOR ");
  print_serial(TYPEWRITER_MFG "/" TYPEWRITER_MODEL " : ");
  if ( (qwerty_tim3_oc_mode   == QWERTY_MODE_INVERTED)
    && (mux_it_mode           == QWERTY_MODE_INVERTED) )
  {
    print_serial("MODE: INV: ");
  }

  if ( (qwerty_tim3_oc_mode   == QWERTY_MODE_POSITIVE)
    && (mux_it_mode           == QWERTY_MODE_POSITIVE) )
  {
    print_serial("MODE: POS: ");
  }
  if (qwerty_typewriter_system_state == QWERTY_TYPEWRITER_SYSTEM_STATE_CLI)
  {
    print_serial("BEGIN: CLI\r\n");
  }
  else
  {
    print_serial("BEGIN: TYPEWRITER\r\n");
  }

  /* USER CODE BEGIN WHILE */
  while (1)
  {
    //
    // keep processing OUTPUT buffer
    //
    if (qwerty_output_buffer_end != qwerty_output_buffer_start)
      if (!qwerty_typing)
    {
      qwerty_typing = 1;

      qwerty_output_counter1             = 0;
      qwerty_output_counter2             = 0;
      qwerty_output_counter1_have_update = 0;
      qwerty_output_counter2_have_update = 0;

      // check for unsupported bit:
      //    It is is used send pitch change inducing commands to the typewriter
      if (qwerty_output_buffer[qwerty_output_buffer_start].val & KEYBOARD_UNSUPPORTED)
      {

        if (qwerty_output_buffer[qwerty_output_buffer_start].val & qwerty_pitch_state)
        {
          // pitch hasn't changed: nothing to do'
          qwerty_typing = 0;
          goto _skip_pitch_set;
        }

        // wait 200ms until the printwheel stops spinning
        do
        {
          HAL_Delay(50);
          qwerty_dl_time_elapsed_ms = HAL_GetTick() - qwerty_dl_tick;
        }
        while (qwerty_dl_time_elapsed_ms < 1000L);

        // sprintf(msg, "elapsed time = %lu\r\n", qwerty_dl_time_elapsed_ms);
        // print_serial(msg);

        qwerty_pitch_rising_edge_counter = 0;

        qwerty_pitch_ignore_dl = 1;

        // set SL line to 0:
        FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
        HAL_Delay( 600 );

        // set SL line to 1: flip line back to analog
        FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
        HAL_Delay( 200 );

        // set SL line to 0:
        FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
        HAL_Delay( 600 );

        // set SL line to 1: flip line back to analog
        FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
        HAL_Delay( 200 );

        uint32_t stop_pressed = HAL_GetTick();

        if ( qwerty_output_buffer[qwerty_output_buffer_start].val & QWERTY_PITCH_SWITCH_STOP )
        {
          // press CODE using D0
          idx_drive_ch_new = 0x04;
          idx_sense_ch_new = 0x07;
          idx_code_ch      = 0x01;
          idx_shift_ch     = 0x00;
          qwerty_type_symbol_and_wait();
        }

        if ( qwerty_output_buffer[qwerty_output_buffer_start].val & QWERTY_PITCH_SWITCH_12P)
        {
          //
          // 12p is built in
          //
          // do not ignore what is going on DL line: 12P is built in font for this typewheel
          // qwerty_pitch_ignore_dl = 0;
          //
          // // wait 200ms until the printwheel stops spinning
          // do
          // {
          //   HAL_Delay(50);
          //   qwerty_dl_time_elapsed_ms = HAL_GetTick() - qwerty_dl_tick;
          // }
          // while (qwerty_dl_time_elapsed_ms < 1000L);
          //
          // sprintf(msg,"pitch switch: time since stop = %lu (%lu)\r\n",
          //         HAL_GetTick() - stop_pressed, qwerty_dl_time_elapsed_ms);
          // print_serial(msg);

          // Pulse 1
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 110 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 6 );
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 3 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 5 );
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 11 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 5 );
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 28 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 45 );

          // Pulse 2: Repeat of 1
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 110 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 6 );
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 3 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 5 );
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 11 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 5 );
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 28 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 45 );

          // Pulse 3: blank
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 612 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 45 );

          // Pulse 4: repeat of 3
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 612 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 45 );

          // Pulse 5
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 29 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 5 );
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 11 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 5 );
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 4 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 48 );
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 7 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 47 );
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 60 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 47 );
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 30 );

          qwerty_pitch_state = QWERTY_PITCH_SWITCH_12P;

        } /* if ( qwerty_output_buffer[qwerty_output_buffer_start].val & QWERTY_PITCH_SWITCH_12P) */
        else
        {
          // Pulse 1
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 110 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 3 );
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 21 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 5 );
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 28 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 45 );
          // Pulse 2: repeat of 1
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 110 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 3 );
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 21 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 5 );
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 28 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 45 );
          // Pulse 3: blank
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 612 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 48 );
          // Pulse 4: repeat of 3: blank
          FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 612 );
          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
          HAL_Delay( 48 );

          if ( qwerty_output_buffer[qwerty_output_buffer_start].val & QWERTY_PITCH_SWITCH_10P)
          {
            //
            // send 10p:
            //
            // Pulse 5:
            FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
            HAL_Delay( 30 );
            FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
            HAL_Delay( 5 );
            FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
            HAL_Delay( 180 );
            FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
            HAL_Delay( 48 );
            FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
            HAL_Delay( 28 );

            qwerty_pitch_state = QWERTY_PITCH_SWITCH_10P;

          } /* if ( qwerty_output_buffer[qwerty_output_buffer_start].val & QWERTY_PITCH_SWITCH_10P) */
          else if ( qwerty_output_buffer[qwerty_output_buffer_start].val & QWERTY_PITCH_SWITCH_15P)
          {
            //
            // send 15p:
            //
            // Pulse 5:
            FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
            HAL_Delay( 30 );
            FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
            HAL_Delay( 5 );
            FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
            HAL_Delay( 20 );
            FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
            HAL_Delay( 48 );
            FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
            HAL_Delay( 115 );
            FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
            HAL_Delay( 48 );
            FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
            HAL_Delay( 30 );

            qwerty_pitch_state = QWERTY_PITCH_SWITCH_15P;

          } /* else if ( qwerty_output_buffer[qwerty_output_buffer_start].val & QWERTY_PITCH_SWITCH_15P) */
          else if ( qwerty_output_buffer[qwerty_output_buffer_start].val & QWERTY_PITCH_SWITCH_PSP)
          {
            //
            // send PSp:
            //
            // Pulse 5:
            FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
            HAL_Delay( 30 );
            FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
            HAL_Delay( 5 );
            FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
            HAL_Delay( 11 );
            FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
            HAL_Delay( 5 );
            FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
            HAL_Delay( 58 );
            FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
            HAL_Delay( 48 );
            FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
            HAL_Delay( 60 );
            FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);
            HAL_Delay( 46 );
            FLIP_GPIO_2_RESET(QWERTY_SL_PIN,QWERTY_SL_PORT);
            HAL_Delay( 30 );

            qwerty_pitch_state = QWERTY_PITCH_SWITCH_PSP;
          } /* else if ( qwerty_output_buffer[qwerty_output_buffer_start].val & QWERTY_PITCH_SWITCH_PSP) */

          FLIP_GPIO_2_ANALOG(QWERTY_SL_PIN,QWERTY_SL_PORT);

          HAL_Delay( 500 );
        }


        // press CODE using D0
        idx_drive_ch_new = 0x04;
        idx_sense_ch_new = 0x07;
        idx_code_ch      = 0x01;
        idx_shift_ch     = 0x00;
        qwerty_type_symbol_and_wait();
        do
        {
          HAL_Delay(50);
          qwerty_dl_time_elapsed_ms = HAL_GetTick() - qwerty_dl_tick;
        }
        while (qwerty_dl_time_elapsed_ms < 600L);
        // HAL_Delay( 600 );

        // press CODE using D0
        idx_drive_ch_new = 0x04;
        idx_sense_ch_new = 0x07;
        idx_code_ch      = 0x01;
        idx_shift_ch     = 0x00;
        qwerty_type_symbol_and_wait();
        do
        {
          HAL_Delay(50);
          qwerty_dl_time_elapsed_ms = HAL_GetTick() - qwerty_dl_tick;
        }
        while (qwerty_dl_time_elapsed_ms < 600L);
        // HAL_Delay( 600 );

_skip_pitch_set:

        qwerty_pitch_ignore_dl = 0;

        // gobble up a symbol
        INC_BUFFER_IDX(qwerty_output_buffer_start, 1);

        continue;
      } /* if (qwerty_output_buffer[qwerty_output_buffer_start].val & KEYBOARD_UNSUPPORTED) */

      idx_drive_ch_new = qwerty_output_buffer[qwerty_output_buffer_start].symbol.drive;
      idx_sense_ch_new = qwerty_output_buffer[qwerty_output_buffer_start].symbol.sense;
      idx_code_ch      = qwerty_output_buffer[qwerty_output_buffer_start].symbol.code;
      idx_shift_ch     = qwerty_output_buffer[qwerty_output_buffer_start].symbol.shift;
      INC_BUFFER_IDX(qwerty_output_buffer_start, 1);

      qwerty_count_typed_characters++;

      qwerty_type_symbol_and_wait();

    } /*if (qwerty_output_buffer_end != qwerty_output_buffer_start) if (!qwerty_typing)*/

    //
    //  get character from serial port
    //
    rcv_byte = __io_getchar();

    //
    //  stop here if nothing was received
    //
    if (rcv_byte < 0)
    {
      continue;
    }

    if (qwerty_typewriter_system_state == QWERTY_TYPEWRITER_SYSTEM_STATE_TYPEWRITER)
    {
      // receiving two 0x00 bytes switches typewriter mode off and back to command line
      if (rcv_byte == 0x00)
      {
        if (!qwerty_output_buffer_null_received)
        {
          qwerty_output_buffer_null_received = 1;
        }
        else
        {
          qwerty_typewriter_system_state = QWERTY_TYPEWRITER_SYSTEM_STATE_CLI;
          qwerty_output_buffer_null_received = 0;
        }
        continue;
      }

      qwerty_output_buffer_null_received = 0;

      if (rcv_byte == 0x1b)
      {
        //
        // ESC -received
        //
        qwerty_output_buffer_esc_received = 1;
        continue;
      }

      if (qwerty_output_buffer_esc_received == 1)
      {
        /* failed to recognize ESC code */
        qwerty_output_buffer_esc_received = 0;

        /** Escape sequence:
         *   ESC 0      -> line spacing 1
         *   ESC 1      -> line spacing 1-1/2
         *   ESC 2      -> line spacing 2
         *   ESC 3      -> line spacing 3
         *   ESC - 0/1  -> set underline on/off
         *   ESC Q n    -> set right margin to n
         *   ESC l m    -> set left  margin to m
         *   ESC i 0/1  -> paragraph indent on/off
         */
        if (rcv_byte == '?')
        {
          qwerty_pitch_rising_edge_counter = 0;
        }
        else if (rcv_byte == 0x7f)
        {
          //
          // ESC+BS received
          //
          qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00c2 | KEYBOARD_CODE;
          INC_BUFFER_IDX(qwerty_output_buffer_end,1);
          continue;
        }
        else if (rcv_byte == '0')
        {
          /* ESC 0 - Line Spacing 1 */
          while (qwerty_output_buffer_line_spacing != 0)
          {
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0011 | KEYBOARD_CODE;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            qwerty_output_buffer_line_spacing
              = (qwerty_output_buffer_line_spacing + 1) % qwerty_state_line_spacing_limit;
          }
          qwerty_output_buffer_line_spacing = 0;
        }
        else if (rcv_byte == '1')
        {
          /* ESC 1 - Line Spacing 1 1/2*/
          while (qwerty_output_buffer_line_spacing != 1)
          {
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0011 | KEYBOARD_CODE;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            qwerty_output_buffer_line_spacing
                = (qwerty_output_buffer_line_spacing + 1) % qwerty_state_line_spacing_limit;
          }
          qwerty_output_buffer_line_spacing = 1;
        }
        else if (rcv_byte == '2')
        {
          /* ESC 2 - Line Spacing 2*/
          while (qwerty_output_buffer_line_spacing != 2)
          {
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0011 | KEYBOARD_CODE;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            qwerty_output_buffer_line_spacing
                = (qwerty_output_buffer_line_spacing + 1) % qwerty_state_line_spacing_limit;
          }
          qwerty_output_buffer_line_spacing = 2;
        }
        else if (rcv_byte == '3')
        {
          /* ESC 3 - Line Spacing 3*/
          while (qwerty_output_buffer_line_spacing != 3)
          {
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0011 | KEYBOARD_CODE;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            qwerty_output_buffer_line_spacing
                = (qwerty_output_buffer_line_spacing + 1) % qwerty_state_line_spacing_limit;
          }
          qwerty_output_buffer_line_spacing = 3;
        }
        else if (rcv_byte == 'P')
        {
          // PICA, aka 10 CPI pitch
          qwerty_output_buffer[qwerty_output_buffer_end].val =
              QWERTY_PITCH_SWITCH_STOP | QWERTY_PITCH_SWITCH_10P | KEYBOARD_UNSUPPORTED;
          INC_BUFFER_IDX(qwerty_output_buffer_end,1);
        }
        else if (rcv_byte == 'M')
        {
          // elite, aka 12 CPI pitch
          qwerty_output_buffer[qwerty_output_buffer_end].val =
              QWERTY_PITCH_SWITCH_STOP | QWERTY_PITCH_SWITCH_12P | KEYBOARD_UNSUPPORTED;
          INC_BUFFER_IDX(qwerty_output_buffer_end,1);
        }
        else if ((rcv_byte == 0X0F)||(rcv_byte == 'o'))
        {
          // CONDENSED, aka 15 CPI
          qwerty_output_buffer[qwerty_output_buffer_end].val =
              QWERTY_PITCH_SWITCH_STOP | QWERTY_PITCH_SWITCH_15P | KEYBOARD_UNSUPPORTED;
          INC_BUFFER_IDX(qwerty_output_buffer_end,1);
        }
        else if (rcv_byte == 'p')
        {
          // PROPORTIONAL SCRIPT, aka ps
          qwerty_output_buffer[qwerty_output_buffer_end].val =
                QWERTY_PITCH_SWITCH_STOP | QWERTY_PITCH_SWITCH_PSP | KEYBOARD_UNSUPPORTED;
          INC_BUFFER_IDX(qwerty_output_buffer_end,1);
        }
        else if (rcv_byte == 's')
        {
          // stop
          qwerty_output_buffer[qwerty_output_buffer_end].val = QWERTY_PITCH_SWITCH_STOP
              | KEYBOARD_UNSUPPORTED;
          INC_BUFFER_IDX(qwerty_output_buffer_end,1);
        }
        else if (rcv_byte == 'z')
        {
          // switch and sensor test
          qwerty_output_buffer[qwerty_output_buffer_end].val = QWERTY_PITCH_SWITCH_SENSOR_TEST
              | KEYBOARD_UNSUPPORTED;
          INC_BUFFER_IDX(qwerty_output_buffer_end,1);
        }
        else if ((rcv_byte == 'E') || (rcv_byte == 'G'))
        {
          // toggle bold if OFF
          if (!qwerty_output_buffer_bold)
          {
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0077 | KEYBOARD_CODE;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            qwerty_output_buffer_bold = 1;
          }
        }
        else if ((rcv_byte == 'F') || (rcv_byte == 'H'))
        {
          // toggle bold if ON
          if (qwerty_output_buffer_bold)
          {
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0077 | KEYBOARD_CODE;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            qwerty_output_buffer_bold = 0;
          }
        }
        else if (rcv_byte == '-') /* underline  */
        {
          iwait_esc_underline = 1;
          qwerty_output_buffer_esc_received = 2;
        }
        else if (rcv_byte == 'l') /* l margin n */
        {
          iwait_esc_lmar      = 1;
          qwerty_output_buffer_esc_received = 2;
        }
        else if (rcv_byte == 'Q') /* r margin n */
        {
          iwait_esc_rmar      = 1;
          qwerty_output_buffer_esc_received = 2;
        }
        else if (rcv_byte == 'D') /* Horizontal tabs setup */
        {
          iwait_esc_tab_set = 1;
          qwerty_output_buffer_esc_received = 2;
        }
        else if (rcv_byte == '[') /* escape terminal codes */
        {
          iwait_esc_term_set = 1;
          qwerty_output_buffer_esc_received = 2;
        }
        else if (rcv_byte == 'i') /* paragraph indent */
        {
          iwait_esc_par_indent = 1;
          qwerty_output_buffer_esc_received = 2;
        }
        else if (rcv_byte == 'a') /* Justification n */
        {
          iwait_esc_just      = 1;
          qwerty_output_buffer_esc_received = 2;
        }

        continue;
      } /* if (qwerty_output_buffer_esc_received == 1) */

      if (qwerty_output_buffer_esc_received == 2)
      {
        /* failed to recognize ESC code */
        qwerty_output_buffer_esc_received = 0;

        if (iwait_esc_term_set)
        {
          if (iwait_esc_term_set == 1)
          {
            iwait_esc_term_set = 0;

            if (rcv_byte == 'A')
            {
              // UP
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00c6;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }
            else if (rcv_byte == 'B')
            {
              // DOWN
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0010;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }
            else if (rcv_byte == 'C')
            {
              // RIGHT
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0017;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }
            else if (rcv_byte == 'D')
            {
              // LEFT
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00C7;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }
            else if (rcv_byte == '3')
            {
              // is it DEL?
              iwait_esc_term_set = 3;
              qwerty_output_buffer_esc_received = 3;
            }
          }

          continue;
        }

        if (iwait_esc_tab_set)
        {
          iwait_esc_tab_set = 0;

          if ((rcv_byte == '0') || (rcv_byte == 0))
          {
            // press TAB CLR to clear all tabs
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0030;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            qwerty_output_buffer_tab_set = 0;
          }
          else if ((rcv_byte == '1') || (rcv_byte == 1))
          {
            // press TAB SET at the current position of the carriage
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00d1;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            qwerty_output_buffer_tab_set ++;
          }

          continue;
        } /* if (iwait_esc_tab_set) */

        if (iwait_esc_rmar)
        {
          iwait_esc_rmar = 0;

          // if new and old value are the same: nothing to do
          // or if left margin is greater than new right margin: reject change
          if ((qwerty_page_rmar == rcv_byte) || (qwerty_page_lmar > rcv_byte))
          {
            continue;
          }

          // lmar has not been setup yet
          if (qwerty_page_lmar < 0)
          {
            // MAR REL
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00d2;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);

            /* go back and delete left margin */
            for (int8_t i=0; i<84; i++)
            {
              // BS
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00c2;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);

            }
            // set L margin at 0
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0034;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);

            // store internally L margin as 0
            qwerty_page_lmar = 0;
          }
          else
          {
            // press enter to get to the current left margin
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00c0;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);

            // press rev index to go back up to the current line
            for (int8_t i=0; i<2+qwerty_output_buffer_line_spacing; i++)
            {
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00c6;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }
          }

          // MAR REL
          qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00d2;
          INC_BUFFER_IDX(qwerty_output_buffer_end,1);
          /* go right m places */
          for (uint8_t i=0; i<rcv_byte-qwerty_page_lmar; i++)
          {
            // space
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0037;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
          }

          // set R margin
          qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00d5;
          INC_BUFFER_IDX(qwerty_output_buffer_end,1);
          qwerty_page_rmar = rcv_byte;

          continue;
        } /* if (iwait_esc_rmar) */

        if (iwait_esc_lmar)
        {
          iwait_esc_lmar = 0;

          if (qwerty_page_lmar == rcv_byte)
          {
            continue;
          }

          // if left margin is greater than new right margin: reject change
          if ( (qwerty_page_rmar > 0) && (qwerty_page_rmar < rcv_byte) )
          {
            continue;
          }

          if (qwerty_page_lmar < 0)
          {
            // MAR REL
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00d2;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);

            /* go back  */
            for (int8_t i=0; i<84; i++)
            {
              // BS
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00c2;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }

            /* go right m places */
            for (int8_t i=0; i<rcv_byte; i++)
            {
              // space
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0037;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }
          }
          else if (qwerty_page_lmar != rcv_byte)
          {
            // press enter to get to the current left margin
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00c0;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);

            // press UP to go back up to the current line
            for (int8_t i=0; i<2+qwerty_output_buffer_line_spacing; i++)
            {
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00c6;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }

            if (rcv_byte > qwerty_page_lmar)
            {
              /* go right */
              for (int8_t i=0; i<rcv_byte-qwerty_page_lmar; i++)
              {
                // SPACE
                qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0037;
                INC_BUFFER_IDX(qwerty_output_buffer_end,1);
              }
            }
            else
            {
              // MAR REL
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00d2;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);

              /* go left and release left margin */
              for (int8_t i=0; i<qwerty_page_lmar-rcv_byte; i++)
              {
                // BS
                qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00c2;
                INC_BUFFER_IDX(qwerty_output_buffer_end,1);
              }
            }
          }

          // set L margin
          qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0034;
          INC_BUFFER_IDX(qwerty_output_buffer_end,1);
          // store internally L margin
          qwerty_page_lmar = rcv_byte;
          continue;
        } /* if (iwait_esc_lmar) */

        if (iwait_esc_par_indent)
        {
          iwait_esc_par_indent = 0;
          qwerty_output_buffer_esc_received = 0;

          if ( (rcv_byte == '0') || (rcv_byte == 0x00) )
          {
            // toggle par indent if ON
            if (qwerty_output_buffer_par_indent)
            {
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00c0 | KEYBOARD_CODE;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
              qwerty_output_buffer_par_indent = 0;
            }
          }
          else if ( (rcv_byte == '1') || (rcv_byte == 0x01) )
          {
            // toggle par indent if OFF
            if (!qwerty_output_buffer_par_indent)
            {
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00d4 | KEYBOARD_CODE;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
              qwerty_output_buffer_par_indent = 1;
            }
          }
        } /* if (iwait_esc_par_indent) */

        if (iwait_esc_underline)
        {
          iwait_esc_underline = 0;
          qwerty_output_buffer_esc_received = 0;

          if ( (rcv_byte == '0') || (rcv_byte == 0x00) )
          {
            // toggle underline if ON
            if (qwerty_output_buffer_underline)
            {
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0084 | KEYBOARD_CODE;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
              qwerty_output_buffer_underline = 0;
            }
          }
          else if ( (rcv_byte == '1') || (rcv_byte == 0x01) )
          {
            // toggle underline if OFF
            if (!qwerty_output_buffer_underline)
            {
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0084 | KEYBOARD_CODE;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
              qwerty_output_buffer_underline = 1;
            }
          }
        } /* if (iwait_esc_underline) */

        if (iwait_esc_just)
        {
          iwait_esc_just = 0;

          if (rcv_byte == '0')
          {
            // justification: none, user controlled
            if (qwerty_output_buffer_just == 1)
            {
              /* turn off center justification : press CODE + C */
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0066 | KEYBOARD_CODE;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }
            else if (qwerty_output_buffer_just == 2)
            {
              /* turn off right justification : press CODE + O */
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00a4 | KEYBOARD_CODE;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }
            else if (qwerty_output_buffer_just == 3)
            {
              /* turn off fake justification : press CODE + R */
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0074 | KEYBOARD_CODE;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }

            /* enter typewriter mode */
            qwerty_output_buffer_just = 0;
          } /* if (rcv_byte == '0') */
          else if (rcv_byte == '1')
          {
            // justification: center
            if (qwerty_output_buffer_just == 1)
            {
              continue;
            }

            if (qwerty_output_buffer_just == 2)
            {
              /* turn off right justification : press CODE + O */
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00a4 | KEYBOARD_CODE;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }
            else if (qwerty_output_buffer_just == 3)
            {
              /* turn off fake justification : press CODE + R */
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0074 | KEYBOARD_CODE;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }

            /* turn on center justification : press CODE + C */
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0066 | KEYBOARD_CODE;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);

            qwerty_output_buffer_just = 1;
          } /* if (rcv_mode == '1') */
          else if (rcv_byte == '2')
          {
            // justification: right flush
            if (qwerty_output_buffer_just == 2)
            {
              continue;
            }
            else if (qwerty_output_buffer_just == 3)
            {
              /* turn off fake justification : press CODE + R */
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0074 | KEYBOARD_CODE;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }

            if (qwerty_output_buffer_just == 1)
            {
              /* turn on center justification : press CODE + C */
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0066 | KEYBOARD_CODE;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }

            /* turn on right justification : press CODE + O */
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00a4 | KEYBOARD_CODE;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);

            qwerty_output_buffer_just = 2;
          } /* if (rcv_mode == '2') */
          else if (rcv_byte == '3')
          {
            // justification: fake full
            if (qwerty_output_buffer_just == 1)
            {
              /* turn off center justification : press CODE + C */
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0066 | KEYBOARD_CODE;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }
            else if (qwerty_output_buffer_just == 2)
            {
              /* turn off right justification : press CODE + O */
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00a4 | KEYBOARD_CODE;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }
            else if (qwerty_output_buffer_just == 3)
            {
              continue;
            }

            /* turn off fake justification : press CODE + R */
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0074 | KEYBOARD_CODE;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);

            qwerty_output_buffer_just = 3;
          } /* if (rcv_mode == '3') */

        }  /* if (iwait_esc_just) */

        continue;
      } /* if (qwerty_output_buffer_esc_received == 2) */

      if (qwerty_output_buffer_esc_received == 3)
      {
        qwerty_output_buffer_esc_received = 0;

        if (iwait_esc_term_set == 3)
        {
          iwait_esc_term_set = 0;

          if (rcv_byte == '~')
          {
            // DEL
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00d7;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
          }
          continue;
        }

      } /* if (qwerty_output_buffer_esc_received == 3) */

      if (rcv_byte == 8)
      {
        //
        // BS received
        //
        qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00c2;
        INC_BUFFER_IDX(qwerty_output_buffer_end,1);
        continue;
      }

      if (rcv_byte == 9)
      {
        //
        // TAB received
        //
        qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00d4;
        INC_BUFFER_IDX(qwerty_output_buffer_end,1);
        continue;
      }

      if (rcv_byte == 13)
      {
        //
        // CR received
        //
        qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00c0;
        INC_BUFFER_IDX(qwerty_output_buffer_end,1);

        /* check if we are in global justification modes: center or right flush */
        if (qwerty_output_buffer_just == 2)
        {
          /* Right FLush: press CODE + O (R Flsh) */
          qwerty_output_buffer[qwerty_output_buffer_end].val = 0x00a4 | KEYBOARD_CODE;
          INC_BUFFER_IDX(qwerty_output_buffer_end,1);
        }
        else if (qwerty_output_buffer_just == 1)
        {
          /* press CODE + C (Ctr) */
          qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0066 | KEYBOARD_CODE;
          INC_BUFFER_IDX(qwerty_output_buffer_end,1);
        }

        continue;
      }

      if ( (rcv_byte > 31) && (rcv_byte < 128) )
      {
        // sprintf(msg,"rcv_byte=%02x\r\n", rcv_byte);
        // print_serial(msg);

        //
        // ascii character received
        //
        symbol1.val = symbol[rcv_byte - 32].val;
        // if (rcv_byte == 0x7e)
        // {
        //   sprintf(msg,"rcv_byte=%04x\r\n", symbol1.val);
        //   print_serial(msg);
        // }

        if (symbol1.val == KEYBOARD_UNSUPPORTED)
          continue;

        qwerty_output_buffer[qwerty_output_buffer_end].val = symbol1.val;
        INC_BUFFER_IDX(qwerty_output_buffer_end,1);

        continue;
      }

    } /*if (qwerty_typewriter_system_state == QWERTY_TYPEWRITER_SYSTEM_STATE_TYPEWRITER)*/

    if ( ((rcv_byte =='\r') || (rcv_byte =='\n') || (rcv_byte ==' ')
      || (rcv_byte ==';') ) && (len_cmd==0) )
      continue;

    usart2_rx_cmd_buffer[len_cmd] = rcv_byte;
    len_cmd++;
    if (len_cmd >= USART2_RX_CMD_BUFFER_SIZE-1)
    {
      len_cmd = 0;
      continue;
    }

    if ( (rcv_byte != '\r') && (rcv_byte != '\n') )
      continue;

    // terminate command buffer so that it is regular 0-terminated string
    usart2_rx_cmd_buffer[len_cmd] = '\0';

    idx_cmd=0;

_process_more:

    // sprintf(msg,"1:%lu:%lu\r\n",len_cmd,idx_cmd);
    // print_serial(msg);

    if (  *(usart2_rx_cmd_buffer+idx_cmd) == '\0'
      || (*(usart2_rx_cmd_buffer+idx_cmd) == '\n')
      || (*(usart2_rx_cmd_buffer+idx_cmd) == '\r') )
    {
      // print_serial("_p:exit\r\n");
      goto _process_more;
    }

    // print_serial("2\r\n");
    // gobble
    if ( (*(usart2_rx_cmd_buffer+idx_cmd) == '?')
      || (*(usart2_rx_cmd_buffer+idx_cmd) == ' ')
      || (*(usart2_rx_cmd_buffer+idx_cmd) == ';') )
    {
      idx_cmd++;
      goto _process_more;
    }

    // check usart2_rx_cmd_buffer
    // PWM:
    if (strlen(usart2_rx_cmd_buffer+idx_cmd) < 4)
      goto _exit;

    if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"print", 5))
    {
      qwerty_typewriter_system_state = QWERTY_TYPEWRITER_SYSTEM_STATE_TYPEWRITER;

      print_serial("Starting print operation: Send 0x0000 to exit!\r\n");

      goto _exit;

    } /*if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"type", 4))*/

    if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"sys:", 4))
    {
      idx_cmd += 4;

      // gobble up spaces
      while(*(usart2_rx_cmd_buffer+idx_cmd) == ' ')
      {
        idx_cmd++;
        if (*(usart2_rx_cmd_buffer+idx_cmd) == '\0')
          goto _exit;
        if (*(usart2_rx_cmd_buffer+idx_cmd) == ';')
          goto _process_more;
      }

      if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"mode", 4))
      {
        idx_cmd += 4;

        GOBBLESPACES;

        if (*(usart2_rx_cmd_buffer+idx_cmd) == 'p')
        {
          idx_cmd++;

          if ( (qwerty_tim3_oc_mode   == QWERTY_MODE_INVERTED)
            && (mux_it_mode           == QWERTY_MODE_INVERTED) )
          {
            qwerty_tim3_oc_mode   = QWERTY_MODE_POSITIVE;
            mux_it_mode           = QWERTY_MODE_POSITIVE;
            goto _restart;
          }
        }

        if (*(usart2_rx_cmd_buffer+idx_cmd) == 'i')
        {
          idx_cmd++;
          if ( (qwerty_tim3_oc_mode   == QWERTY_MODE_POSITIVE)
            && (mux_it_mode           == QWERTY_MODE_POSITIVE) )
          {
            qwerty_tim3_oc_mode    = QWERTY_MODE_INVERTED;
            mux_it_mode            = QWERTY_MODE_INVERTED;
            goto _restart;
          }
        }
      }

    } /* if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"sys:", 4)) */

    // if (strlen(usart2_rx_cmd_buffer+idx_cmd) < 5)
    //   goto _exit;

    if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"meas:", 5))
    {
      idx_cmd += 5;

      if (strlen(usart2_rx_cmd_buffer+idx_cmd) < 4)
        goto _exit;

      if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"mode", 4))
      {
        idx_cmd += 4;

        GOBBLESPACES;

        if (*(usart2_rx_cmd_buffer+idx_cmd) == '?')
        {
          idx_cmd++;
          if (qwerty_tim3_oc_mode == TIM3_OC_MODE_POSITIVE)
            sprintf(msg,"pos\r\n");
          else
            sprintf(msg,"inv\r\n");
          print_serial(msg);
          goto _process_more;
        }

        if (*(usart2_rx_cmd_buffer+idx_cmd) == 'p')
        {
          idx_cmd++;
          qwerty_tim3_oc_new_mode = TIM3_OC_MODE_POSITIVE;

          if (qwerty_tim3_oc_new_mode == qwerty_tim3_oc_mode)
            goto _process_more;

          qwerty_tim3_oc_mode = qwerty_tim3_oc_new_mode;
          qwerty_tim3_oc_it_edge_lead  = TIM_INPUTCHANNELPOLARITY_RISING;
          qwerty_tim3_oc_it_edge_trail = TIM_INPUTCHANNELPOLARITY_FALLING;
          edge1=0;
          edge2=0;
          IC_Value1 = 0;
          IC_Value2 = 0;
          previous_IC_Value1 = 0;
          previous_IC_Value2 = 0;
          HAL_TIM_Base_MspDeInit(&htim3);
          MX_TIM3_Init();
          // HAL_TIM_IC_Start_DMA(&htim3, TIM_CHANNEL_1,(uint32_t*)&IC_Value1,1);
          HAL_TIM_IC_Start_IT (&htim3, TIM_CHANNEL_1);
          HAL_TIM_IC_Start_IT (&htim3, TIM_CHANNEL_2);
        }
        else if (*(usart2_rx_cmd_buffer+idx_cmd) == 'i')
        {
          idx_cmd++;
          qwerty_tim3_oc_new_mode = TIM3_OC_MODE_INVERTED;

          if (qwerty_tim3_oc_new_mode == qwerty_tim3_oc_mode)
            goto _process_more;

          qwerty_tim3_oc_mode = qwerty_tim3_oc_new_mode;
          qwerty_tim3_oc_it_edge_lead  = TIM_INPUTCHANNELPOLARITY_FALLING;
          qwerty_tim3_oc_it_edge_trail = TIM_INPUTCHANNELPOLARITY_RISING;
          edge1=0;
          edge2=0;
          IC_Value1 = 0;
          IC_Value2 = 0;
          previous_IC_Value1 = 0;
          previous_IC_Value2 = 0;
          HAL_TIM_Base_MspDeInit(&htim3);
          MX_TIM3_Init();
          HAL_TIM_IC_Start_IT (&htim3, TIM_CHANNEL_1);
          HAL_TIM_IC_Start_IT (&htim3, TIM_CHANNEL_2);
          // HAL_TIM_IC_Start_DMA(&htim3, TIM_CHANNEL_1,(uint32_t*)&IC_Value1,1);
        }

        goto _process_more;
      }

      if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"ch1?", 4))
      {
        idx_cmd += 4;
        snprintf(msg,64, "%lu,%lu,0\r\n", period1, pulse1);
        print_serial(msg);
        period1 = 0;
        pulse1 = 0;
        // sprintf(msg,"%lu,%lu,0\r\n",period1,pulse1);
        goto _process_more;
      }

      if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"ch1:", 4))
      {
        idx_cmd += 4;

        if (strlen(usart2_rx_cmd_buffer+idx_cmd) < 5)
          goto _exit;

        if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"pulse", 5))
        {
          idx_cmd += 5;
          sprintf(msg,"%lu\r\n",pulse1);
          print_serial(msg);
          if (*(usart2_rx_cmd_buffer+idx_cmd) == '\0')
          {
            goto _exit;
          }
          goto _process_more;

        } /* if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"pulse", 5)) */
        else if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"delay", 5))
        {
          idx_cmd += 5;
          print_serial("0\r\n");
          if (*(usart2_rx_cmd_buffer+idx_cmd) == '\0')
          {
            goto _exit;
          }
          goto _process_more;

        } /* if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"pulse", 5)) */

        if (strlen(usart2_rx_cmd_buffer+idx_cmd) < 6)
          goto _exit;
        if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"period", 6))
        {
          idx_cmd += 6;
          sprintf(msg,"%lu\r\n",period1);
          print_serial(msg);
          if (*(usart2_rx_cmd_buffer+idx_cmd) == '\0')
          {
            goto _exit;
          }
          goto _process_more;

        } /* if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"period", 6)) */

      }

      if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"ch2?", 4))
      {
        idx_cmd += 4;
        sprintf(msg,"%lu,%lu,%lu\r\n",period2,pulse2,delay2);
        print_serial(msg);
        period2 = 0;
        pulse2 = 0;
        delay2 = 0;

        goto _process_more;
      }

      if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"ch2:", 4))
      {
        idx_cmd += 4;

        if (strlen(usart2_rx_cmd_buffer+idx_cmd) < 5)
          goto _exit;

        if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"pulse", 5))
        {
          idx_cmd += 5;
          sprintf(msg,"%lu\r\n",pulse2);
          print_serial(msg);
          if (*(usart2_rx_cmd_buffer+idx_cmd) == '\0')
          {
            goto _exit;
          }
          goto _process_more;

        } /* if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"pulse", 5)) */
        else if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"delay", 5))
        {
          idx_cmd += 5;
          sprintf(msg,"%lu\r\n",delay2);
          print_serial(msg);
          if (*(usart2_rx_cmd_buffer+idx_cmd) == '\0')
          {
            goto _exit;
          }
          goto _process_more;
        } /* if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"delay", 5)) */

        if (strlen(usart2_rx_cmd_buffer+idx_cmd) < 6)
          goto _exit;
        if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"period", 6))
        {
          idx_cmd += 6;
          sprintf(msg,"%lu\r\n",period2);
          print_serial(msg);
          if (*(usart2_rx_cmd_buffer+idx_cmd) == '\0')
          {
            goto _exit;
          }
          goto _process_more;
        }
      }
    }

    if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"drive:burst", 11))
    {
      idx_cmd += 9;

      if (strlen(usart2_rx_cmd_buffer+idx_cmd) < 1)
        goto _exit;

      GOBBLESPACES;

      qwerty_output_counter_new = atoi(usart2_rx_cmd_buffer+idx_cmd);
      if (qwerty_output_counter_new < QWERTY_OUTPUT_COUNTER_DEFAULT)
      {
        qwerty_output_counter_new = QWERTY_OUTPUT_COUNTER_DEFAULT;
      }
      if (qwerty_output_counter_new != qwerty_output_counter_default)
      {
        qwerty_output_counter_default = qwerty_output_counter_new;
      }

      GOBBLEDIGITS;

    } /*if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"drive:burst", 9))*/

    if ( *(usart2_rx_cmd_buffer+idx_cmd) == 'd' )
    {
      idx_cmd += 1;

      if (strlen(usart2_rx_cmd_buffer+idx_cmd) < 1)
        goto _exit;

      /**
        * d{0:9,a:d} for d{0:13} but with respect to drive S3
        */
      idx_drive_ch_new = *(usart2_rx_cmd_buffer+idx_cmd) - '0';
      if (idx_drive_ch_new < 0)
      {
        print_serial("Drive Channel out of bounds! Err.\r\n");
        goto _exit;
      }
      if (idx_drive_ch_new > 9)
      {
        idx_drive_ch_new += '0' - 'a' + 10;
        if (idx_drive_ch_new > 13)
        {
          print_serial("Drive Channel out of bounds! Err.\r\n");
          goto _exit;
        }
      }

      idx_cmd += 1;

      if ( *(usart2_rx_cmd_buffer+idx_cmd) != 's' )
        goto _exit;

      idx_cmd += 1;

      idx_sense_ch_new = atoi(usart2_rx_cmd_buffer+idx_cmd);
      if ( (idx_sense_ch_new < 0)
        || (idx_sense_ch_new > 7) )
      {
        print_serial("Sense Channel out of bounds! Err.\r\n");
        goto _exit;
      }

      // update registers
      while (qwerty_typing){;}

      qwerty_count_typed_characters++;

      if (idx_drive_ch_new < 7)
      {
        idx_drive_ch_new--;

        qwerty_tim14_oc_ccr  = qwerty_pulse_ccr_drive [idx_drive_ch_new];
        qwerty_tim14_oc_arr  = qwerty_pulse_arr_drive [idx_drive_ch_new];
        qwerty_tim14_oc_arr2 = qwerty_pulse_arr2_drive[idx_drive_ch_new];

        qwerty_output_counter1_have_update = 1;
        qwerty_output_counter2_have_update = 0;
        while (qwerty_output_counter1_have_update){;}
      }
      else
      {
        // idx_drive_ch_new -= 7; /* this one doesnt change: it is fixed at 7 */
        idx_drive_ch_new -= 4;
        idx_drive_ch_new--;

        // At CCR the pulse starts
        qwerty_tim14_oc_ccr  = qwerty_pulse_ccr_drive [idx_drive_ch_new];
        qwerty_tim14_oc_arr  = qwerty_pulse_arr_drive [idx_drive_ch_new];
        qwerty_tim14_oc_arr2 = qwerty_pulse_arr2_drive[idx_drive_ch_new];

        qwerty_output_counter1_have_update = 0;
        qwerty_output_counter2_have_update = 1;
        while (qwerty_output_counter2_have_update){;}
      }

      GOBBLEDIGITS;

      GOBBLESPACES;

    } /*if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"s", 1))*/

_exit:

    // print_serial("exit\r\n");
    len_cmd = 0;

  } /* while (1) */

  // print_serial("END\r\n");
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config_8MHZ(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}



/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel2_3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);

  /* DMA1_Channel4_5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the drive file and the drive line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the drive file name
  * @param  line: assert_param error line drive number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
