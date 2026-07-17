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
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */
volatile uint32_t qwerty_output_counter_new;
volatile uint32_t qwerty_output_counter;
volatile uint32_t qwerty_output_counter_default;
volatile uint8_t  qwerty_output_counter_have_update;
volatile uint8_t  qwerty_typing = 0;

volatile uint8_t   idx_f12_code_ch=0;
volatile uint8_t   idx_f10_shift_ch;
volatile uint8_t   idx_drive_ch;
volatile uint8_t   idx_drive_ch_new;
volatile uint8_t   idx_sense_ch=0xff;
volatile uint8_t   idx_sense_ch_new=0xff;

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
    * PWM MODE CH1 <=> PB8
    *   TIM16_PWM_MODE_POSITIVE QWERTY_MODE_POSITIVE
    *   TIM16_PWM_MODE_INVERTED QWERTY_MODE_INVERTED
    **/
  qwerty_tim16_pwm_mode = QWERTY_MODE_INVERTED;
  // qwerty_tim16_pwm_mode = QWERTY_MODE_POSITIVE;

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

  // PWM setup and control:
  qwerty_tim16_pwm_new_mode = qwerty_tim16_pwm_mode;
  qwerty_tim16_pwm_period  = 24600;
  qwerty_tim16_pwm_pulse   = 200;
  new_pwm_period = qwerty_tim16_pwm_period;
  new_pwm_pulse = qwerty_tim16_pwm_pulse;

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

  // volatile uint32_t qwerty_tim14_oc_ccr = 1;
  // volatile uint32_t qwerty_tim14_oc_arr = 183;
  qwerty_tim14_oc_ccr =
        (TIM14_DELAY == 0 ?  1        : TIM14_CCR + TIM14_PULSE * (TIM14_DELAY - 1));
  qwerty_tim14_oc_arr =
        (TIM14_DELAY == 0 ? TIM14_CCR : TIM14_CCR + TIM14_PULSE *  TIM14_DELAY     );

  edge1 = 0;
  edge2 = 0;
  previous_IC_Value1 = IC_Value1 = 0;
  previous_IC_Value3 = IC_Value3 = 0;
  period1 = 0;
  pulse1  = 0;
  period2 = 0;
  pulse2  = 0;
  delay2  = 0;
  qwerty_output_counter_default     = QWERTY_OUTPUT_COUNTER_DEFAULT;
  qwerty_output_counter_new         = 0;
  qwerty_output_counter             = 0;
  qwerty_output_counter_have_update = 0;

  mux_it_edge = 0;
  if (mux_it_mode == QWERTY_MODE_INVERTED)
  {
    gpio_pin_initial_pull            = GPIO_PULLUP;
    mux_it_mode_gpio_pin_edge0_state = GPIO_PIN_SET;
    mux_it_mode_gpio_pin_edge1_state = GPIO_PIN_RESET;
  }
  else
  {
    gpio_pin_initial_pull            = GPIO_PULLDOWN;
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
  MX_TIM16_Init();

  /* USER CODE BEGIN 2 */
  HAL_TIM_IC_Start_IT (&htim3, TIM_CHANNEL_2);
  HAL_TIM_IC_Start_IT (&htim3, TIM_CHANNEL_4);

  __HAL_TIM_ENABLE_IT  (&htim14, TIM_IT_CC1);
  __HAL_TIM_ENABLE_IT  (&htim14, TIM_IT_UPDATE);
  __HAL_TIM_CLEAR_IT   (&htim14, TIM_IT_CC1 | TIM_IT_UPDATE);

  HAL_TIM_PWM_Start (&htim16, TIM_CHANNEL_1);

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
  qwerty_output_buffer_font_pitch     = 0;
  qwerty_output_buffer_kbd_2          = 0;
  qwerty_output_buffer_just           = 0;
  qwerty_output_buffer_mode           = 0;
  qwerty_output_buffer_esc_received   = 0;
  qwerty_output_buffer_null_received  = 0;
  idx_f10_shift_ch                    = 0;
  idx_f12_code_ch                     = 0;
  qwerty_output_buffer_par_indent     = 0;
  qwerty_output_buffer_tab_set        = 0;

  qwerty_page_lmar = -1;
  qwerty_page_rmar = -1;

  print_serial("QWERTY FIRMWARE: ");
  if ( (qwerty_tim3_oc_mode   == QWERTY_MODE_INVERTED)
    && (mux_it_mode           == QWERTY_MODE_INVERTED)
    && (qwerty_tim16_pwm_mode == QWERTY_MODE_INVERTED) )
  {
    print_serial("MODE: INV: ");
  }

  if ( (qwerty_tim3_oc_mode   == QWERTY_MODE_POSITIVE)
    && (mux_it_mode           == QWERTY_MODE_POSITIVE)
    && (qwerty_tim16_pwm_mode == QWERTY_MODE_POSITIVE) )
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
      idx_drive_ch_new   = qwerty_output_buffer[qwerty_output_buffer_start].symbol.drive;
      idx_sense_ch_new = qwerty_output_buffer[qwerty_output_buffer_start].symbol.sense;
      idx_f12_code_ch  = qwerty_output_buffer[qwerty_output_buffer_start].symbol.code;
      idx_f10_shift_ch = qwerty_output_buffer[qwerty_output_buffer_start].symbol.shift;
      INC_BUFFER_IDX(qwerty_output_buffer_start, 1);

      // if (idx_f12_code_ch)
      //   HAL_GPIO_WritePin(QWERTY_F12_PORT, QWERTY_F12_PIN, mux_it_mode_gpio_pin_edge1_state);
      // else
      //   HAL_GPIO_WritePin(QWERTY_F12_PORT, QWERTY_F12_PIN, mux_it_mode_gpio_pin_edge0_state);
      qwerty_typing = 1;

      // interrupts on TIM14 process data for printing
      TIM14->CR1   &= ~TIM_CR1_CEN;
      qwerty_tim14_oc_ccr =
        (idx_drive_ch_new == 0 ?  1        : TIM14_CCR + TIM14_PULSE * (idx_drive_ch_new - 1));
      qwerty_tim14_oc_arr =
        (idx_drive_ch_new == 0 ? TIM14_CCR : TIM14_CCR + TIM14_PULSE *  idx_drive_ch_new     );

      qwerty_count_typed_characters++;

      if ( idx_f10_shift_ch || idx_f12_code_ch )
      {
        // shift/code key has to be pressed at least 20ms before
        // pressing the actual key
        qwerty_output_counter_have_update = QWERTY_BASE_PULSES_CODE_SHIFT_AHEAD;
      }
      else
        qwerty_output_counter_have_update = 1;

      while (qwerty_typing){;}

    } /*if (qwerty_output_buffer_end != qwerty_output_buffer_start) if (!qwerty_typing)*/

    //
    //  get character from DMA buffered serial port
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
        if (rcv_byte == 0x0f)
        {
          // condensed, aka 15 CPI pitch - excludes bold
          while (qwerty_output_buffer_font_pitch != 2)
          {
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x006b;
            // qwerty_output_buffer[qwerty_output_buffer_end].symbol.drive = 6;
            // qwerty_output_buffer[qwerty_output_buffer_end].symbol.sense  = 11;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            qwerty_output_buffer_font_pitch = (qwerty_output_buffer_font_pitch + 1) % qwerty_state_font_pitch_length;
          }
          qwerty_output_buffer_font_pitch = 2;
          qwerty_output_buffer_esc_received = 0;
          continue;
        }

        /* ESC 0 - Line Spacing 1 */
        if (rcv_byte == '0')
        {
          while (qwerty_output_buffer_line_spacing != 0)
          {
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x000b;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            qwerty_output_buffer_line_spacing
              = (qwerty_output_buffer_line_spacing + 1) % qwerty_state_line_spacing_limit;
          }
          qwerty_output_buffer_line_spacing = 0;
          qwerty_output_buffer_esc_received = 0;
          continue;
        }

        /* ESC 1 - Line Spacing 1 1/2*/
        if (rcv_byte == '1')
        {
          while (qwerty_output_buffer_line_spacing != 1)
          {
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x000b;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            qwerty_output_buffer_line_spacing
              = (qwerty_output_buffer_line_spacing + 1) % qwerty_state_line_spacing_limit;
          }
          qwerty_output_buffer_line_spacing = 1;
          qwerty_output_buffer_esc_received = 0;
          continue;
        }

        /* ESC 1 - Line Spacing 1 1/2*/
        if (rcv_byte == '2')
        {
          while (qwerty_output_buffer_line_spacing != 2)
          {
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x000b;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            qwerty_output_buffer_line_spacing
            = (qwerty_output_buffer_line_spacing + 1) % qwerty_state_line_spacing_limit;
          }
          qwerty_output_buffer_line_spacing = 2;
          qwerty_output_buffer_esc_received = 0;
          continue;
        }

        if (rcv_byte == 'M')
        {
          // elite, aka 12 CPI pitch
          while (qwerty_output_buffer_font_pitch != 1)
          {
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x006b;
            // qwerty_output_buffer[qwerty_output_buffer_end].symbol.drive = 6;
            // qwerty_output_buffer[qwerty_output_buffer_end].symbol.sense  = 11;

            INC_BUFFER_IDX(qwerty_output_buffer_end,1);

            qwerty_output_buffer_font_pitch =
                (qwerty_output_buffer_font_pitch + 1) % qwerty_state_font_pitch_length;
          }
          qwerty_output_buffer_font_pitch = 1;
          qwerty_output_buffer_esc_received = 0;
          continue;
        }

        if (rcv_byte == 'P')
        {
          sprintf(msg, "10CPI on (%u)\r\n", qwerty_output_buffer_font_pitch);
          print_serial(msg);

          // pica, aka 10 CPI pitch
          while (qwerty_output_buffer_font_pitch != 0)
          {
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x006b;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            qwerty_output_buffer_font_pitch = (qwerty_output_buffer_font_pitch + 1) % qwerty_state_font_pitch_length;
          }
          qwerty_output_buffer_font_pitch = 0;
          qwerty_output_buffer_esc_received = 0;

          sprintf(msg,"10CPI on (%u)\r\n", qwerty_output_buffer_font_pitch);
          print_serial(msg);
          continue;
        }

        if (rcv_byte == 'p')
        {
#if defined(DEBUG)
          sprintf(msg, "PS on (%u)\r\n", qwerty_output_buffer_font_pitch);
          print_serial(msg);
#endif

          // PS, aka proportional script
          while (qwerty_output_buffer_font_pitch != 3)
          {
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x006b;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            qwerty_output_buffer_font_pitch++;
            if (qwerty_output_buffer_font_pitch == qwerty_state_font_pitch_length)
              qwerty_output_buffer_font_pitch = 0;
          }
          qwerty_output_buffer_font_pitch = 3;
          qwerty_output_buffer_esc_received = 0;
          sprintf(msg, "PS on (%u)\r\n", qwerty_output_buffer_font_pitch);
          print_serial(msg);
          continue;
        }


        // BOLD ON: ESC + E/G
        if ((rcv_byte == 'E') || (rcv_byte == 'G'))
        {
          // toggle bold if OFF
          if (!qwerty_output_buffer_bold)
          {
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0046;
            // qwerty_output_buffer[qwerty_output_buffer_end].symbol.drive = 4;
            // qwerty_output_buffer[qwerty_output_buffer_end].symbol.sense  = 6;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            qwerty_output_buffer_bold = 1;
          }
          qwerty_output_buffer_esc_received = 0;
          continue;
        }

        // BOLD OFF: ESC + F/H
        if ((rcv_byte == 'F') || (rcv_byte == 'H'))
        {
          // toggle bold if ON
          if (qwerty_output_buffer_bold)
          {
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0046;
            // qwerty_output_buffer[qwerty_output_buffer_end].symbol.drive = 4;
            // qwerty_output_buffer[qwerty_output_buffer_end].symbol.sense  = 6;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            qwerty_output_buffer_bold = 0;
          }
          qwerty_output_buffer_esc_received = 0;
          continue;
        }


        /** Escape sequence:
          *   ESC - 0/1 -> set underline on/off
          *   ESC Q n   -> set right margin to n
          *   ESC l m   -> set left  margin to m
          *   ESC i 0/1 -> paragraph indent on/off
          */
        if (rcv_byte == '-') /* underline  */
        {
          iwait_esc_underline = 1;
          qwerty_output_buffer_esc_received = 2;
          continue;
        }
        else if (rcv_byte == 'Q') /* r margin n */
        {
          iwait_esc_rmar      = 1;
          qwerty_output_buffer_esc_received = 2;
          continue;
        }
        else if (rcv_byte == 'l') /* l margin n */
        {
          iwait_esc_lmar      = 1;
          qwerty_output_buffer_esc_received = 2;
          continue;
        }
        else if (rcv_byte == 'a') /* Justification n */
        {
          iwait_esc_just      = 1;
          qwerty_output_buffer_esc_received = 2;
          continue;
        }
        else if (rcv_byte == 'i') /* paragraph indent */
        {
          iwait_esc_par_indent = 1;
          qwerty_output_buffer_esc_received = 2;
          continue;
        }
        else if (rcv_byte == 'D') /* Horizontal tabs setup */
        {
          iwait_esc_tab_set = 1;
          qwerty_output_buffer_esc_received = 2;
          continue;
        }

      }

      // process second argument of ESC command
      if (qwerty_output_buffer_esc_received == 2)
      {
        if (iwait_esc_tab_set)
        {
          if ((rcv_byte == '0') || (rcv_byte == 0))
          {
            // press CODE + TAB CLR to clear all tabs
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x024b;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            qwerty_output_buffer_tab_set = 0;
          }
          else if ((rcv_byte == '1') || (rcv_byte == 1))
          {
            // press TAB SET at the current position of the carriage
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x005b;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            qwerty_output_buffer_tab_set ++;
          }

          iwait_esc_tab_set = 0;
          qwerty_output_buffer_esc_received = 0;
          continue;
        } /* if (iwait_esc_tab_set) */

        if (iwait_esc_just)
        {

          if (rcv_byte == '0')
          {
            /* turn off center justification*/
            /* turn off right flush  */
            if ((qwerty_output_buffer_just == 1) || (qwerty_output_buffer_just == 2))
            {
              // press enter to get to the current left margin
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0034;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);

              // press rev index to go back up to the current line
              for (int8_t i=0; i<2+qwerty_output_buffer_line_spacing; i++)
              {
                qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0038;
                INC_BUFFER_IDX(qwerty_output_buffer_end,1);
              }

              /* enter typewriter mode */
              qwerty_output_buffer_just = 0;
            }

            while (qwerty_output_buffer_mode != 0)
            {
              /* ESC a 0 - left justification or mode typewriter */
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x009b;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
              qwerty_output_buffer_mode
                = (qwerty_output_buffer_mode + 1) % qwerty_state_mode_limit;
            }
            qwerty_output_buffer_just = 0;
            qwerty_output_buffer_mode = 0;
          } /* if (rcv_mode == '0') */
          else if (rcv_byte == '1')
          {
            /* if justification is right flush  */
            if (qwerty_output_buffer_just == 2)
            {
              // press enter to get to the current left margin
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0034;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);

              // press rev index to go back up to the current line
              for (int8_t i=0; i<2+qwerty_output_buffer_line_spacing; i++)
              {
                qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0038;
                INC_BUFFER_IDX(qwerty_output_buffer_end,1);
              }
            }

            /* set mode to typewriter */
            while (qwerty_output_buffer_mode != 0)
            {
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x009b;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
              qwerty_output_buffer_mode
                = (qwerty_output_buffer_mode + 1) % qwerty_state_mode_limit;
            }

            if (qwerty_output_buffer_just != 1)
            {
              /* press CODE + C (Center)*/
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0232;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }

            /* enter typewriter mode */
            qwerty_output_buffer_just = 1;
          } /* if (rcv_mode == '1') */
          else if (rcv_byte == '2')
          {
            /* if justification is center */
            if (qwerty_output_buffer_just == 1)
            {
              // press enter to get to the current left margin
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0034;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);

              // press rev index to go back up to the current line
              for (int8_t i=0; i<2+qwerty_output_buffer_line_spacing; i++)
              {
                qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0038;
                INC_BUFFER_IDX(qwerty_output_buffer_end,1);
              }
            }

            /* set mode to typewriter */
            while (qwerty_output_buffer_mode != 0)
            {
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x009b;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
              qwerty_output_buffer_mode
              = (qwerty_output_buffer_mode + 1) % qwerty_state_mode_limit;
            }

            if (qwerty_output_buffer_just != 2)
            {
              /* press CODE + R (RMF) */
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0252;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }

            /* enter typewriter mode */
            qwerty_output_buffer_just = 2;
          } /* if (rcv_mode == '2') */
          else if (rcv_byte == '3')
          {
            /* turn off center justification*/
            /* turn off right flush  */
            if ((qwerty_output_buffer_just == 1) || (qwerty_output_buffer_just == 2))
            {
              // press enter to get to the current left margin
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0034;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);

              // press rev index to go back up to the current line
              for (int8_t i=0; i<2+qwerty_output_buffer_line_spacing; i++)
              {
                qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0038;
                INC_BUFFER_IDX(qwerty_output_buffer_end,1);
              }

              /* enter typewriter mode */
              qwerty_output_buffer_just = 0;
            }

            /* ESC a 3 - full justification or mode typewriter */
            while (qwerty_output_buffer_mode != 2)
            {
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x009b;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
              qwerty_output_buffer_mode
              = (qwerty_output_buffer_mode + 1) % qwerty_state_mode_limit;
            }
            qwerty_output_buffer_mode = 2;
            qwerty_output_buffer_just = 3;
          }

          iwait_esc_just = 0;
          qwerty_output_buffer_esc_received = 0;
          continue;
        }

        if (iwait_esc_par_indent)
        {

          if ( (rcv_byte == '0') || (rcv_byte == 0x00) )
          {
            // toggle par indent if ON
            if (qwerty_output_buffer_par_indent)
            {
              print_serial("par indent: off\r\n");
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0253;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
              qwerty_output_buffer_par_indent = 0;
            }
          }
          else if ( (rcv_byte == '1') || (rcv_byte == 0x01) )
          {
            // toggle par indent if OFF
            if (!qwerty_output_buffer_par_indent)
            {
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0253;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
              qwerty_output_buffer_par_indent = 1;
            }
          }
          iwait_esc_par_indent = 0;
          qwerty_output_buffer_esc_received = 0;
          continue;
        }

        if (iwait_esc_underline)
        {
          if ( (rcv_byte == '0') || (rcv_byte == 0x00) )
          {
            // toggle underline if ON
            if (qwerty_output_buffer_underline)
            {
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0056;
              // qwerty_output_buffer[qwerty_output_buffer_end].symbol.drive = 5;
              // qwerty_output_buffer[qwerty_output_buffer_end].symbol.sense  = 6;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
              qwerty_output_buffer_underline = 0;
            }
          }
          else if ( (rcv_byte == '1') || (rcv_byte == 0x01) )
          {
            // toggle underline if OFF
            if (!qwerty_output_buffer_underline)
            {
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0056;
              // qwerty_output_buffer[qwerty_output_buffer_end].symbol.drive = 5;
              // qwerty_output_buffer[qwerty_output_buffer_end].symbol.sense  = 6;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
              qwerty_output_buffer_underline = 1;
            }
          }
          iwait_esc_underline = 0;
          qwerty_output_buffer_esc_received = 0;
          continue;
        }

        if (iwait_esc_lmar)
        {
          if (qwerty_page_lmar == rcv_byte)
          {
            iwait_esc_lmar = 0;
            qwerty_output_buffer_esc_received = 0;
            continue;
          }

          // if left margin is greater than new right margin: reject change
          if ( (qwerty_page_rmar > 0) && (qwerty_page_rmar < rcv_byte) )
          {
            iwait_esc_lmar = 0;
            qwerty_output_buffer_esc_received = 0;
            continue;
          }

          if (qwerty_page_lmar < 0)
          {
            /* go back and release left margin */
            for (int8_t i=0; i<84; i++)
            {
              // MAR REL
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0081;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
              // BS
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0076;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }

            /* go right m places */
            for (int8_t i=0; i<rcv_byte; i++)
            {
              // space
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0027;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }
          }
          else if (qwerty_page_lmar != rcv_byte)
          {
            // press enter to get to the current left margin
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0034;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);

            // press rev index to go back up to the current line
            for (int8_t i=0; i<2+qwerty_output_buffer_line_spacing; i++)
            {
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0038;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }

            if (rcv_byte > qwerty_page_lmar)
            {
              /* go right */
              for (int8_t i=0; i<rcv_byte-qwerty_page_lmar; i++)
              {
                // SPACE
                qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0086;
                INC_BUFFER_IDX(qwerty_output_buffer_end,1);
              }
            }
            else
            {
              /* go left and release left margin */
              for (int8_t i=0; i<qwerty_page_lmar-rcv_byte; i++)
              {
                // MAR REL
                qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0081;
                INC_BUFFER_IDX(qwerty_output_buffer_end,1);
                // BS
                qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0076;
                INC_BUFFER_IDX(qwerty_output_buffer_end,1);
              }
            }
          }

          // set L margin
          qwerty_output_buffer[qwerty_output_buffer_end].val = 0x008b;
          INC_BUFFER_IDX(qwerty_output_buffer_end,1);
          // store internally L margin
          qwerty_page_lmar = rcv_byte;
          iwait_esc_lmar = 0;
          //
          qwerty_output_buffer_esc_received = 0;
          continue;
        }

        if (iwait_esc_rmar)
        {
          // if new and old value are the same: nothing to do
          if (qwerty_page_rmar == rcv_byte)
          {
            iwait_esc_rmar = 0;
            qwerty_output_buffer_esc_received = 0;
            continue;
          }

          // if left margin is greater than new right margin: reject change
          if (qwerty_page_lmar > rcv_byte)
          {
            iwait_esc_rmar = 0;
            qwerty_output_buffer_esc_received = 0;
            continue;
          }

          if (qwerty_page_lmar < 0)
          {
            /* go back and delete left margin */
            for (int8_t i=0; i<84; i++)
            {
              // BS
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0076;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);

              // MAR REL
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0081;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }
            // set L margin at 0
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x008b;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);

            // store internally L margin
            qwerty_page_lmar = 0;
          }
          else
          {
            // press enter to get to the current left margin
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0034;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);

            // press rev index to go back up to the current line
            for (int8_t i=0; i<2+qwerty_output_buffer_line_spacing; i++)
            {
              qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0038;
              INC_BUFFER_IDX(qwerty_output_buffer_end,1);
            }
          }

          /* go right m places */
          for (uint8_t i=0; i<rcv_byte-qwerty_page_lmar; i++)
          {
            // space
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0027;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);

            // MAR REL
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0081;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
          }

          // set R margin
          qwerty_output_buffer[qwerty_output_buffer_end].val = 0x007b;
          INC_BUFFER_IDX(qwerty_output_buffer_end,1);
          qwerty_page_rmar = rcv_byte;
          iwait_esc_rmar = 0;

          //
          qwerty_output_buffer_esc_received = 0;
          continue;

        } /* if (iwait_esc_rmar) */

      } /* if (qwerty_output_buffer_esc_received == 2) */

      if (rcv_byte == 9)
      {
        // horizontal tab
        if (qwerty_output_buffer_tab_set)
        {
          qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0061;
          INC_BUFFER_IDX(qwerty_output_buffer_end,1);
        }
      }

      //if (rcv_byte == 13)
      if (rcv_byte == 13)
      {
        //
        // CR received
        //
        qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0034;
        // qwerty_output_buffer[qwerty_output_buffer_end].symbol.drive = 3;
        // qwerty_output_buffer[qwerty_output_buffer_end].symbol.sense  = 4;
        INC_BUFFER_IDX(qwerty_output_buffer_end,1);

        /* check if we are in global justification modes: center or right flush */
        if (qwerty_output_buffer_just == 2)
        {
          /* press CODE + R (RMF) */
          qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0252;
          INC_BUFFER_IDX(qwerty_output_buffer_end,1);
        }
        else if (qwerty_output_buffer_just == 1)
        {
          /* press CODE + C (center) */
          qwerty_output_buffer[qwerty_output_buffer_end].val = 0x0232;
          INC_BUFFER_IDX(qwerty_output_buffer_end,1);
        }

        continue;
      }

      if ( (rcv_byte > 31) && (rcv_byte < 128) )
      {
        //
        // ascii character received
        //
        symbol1.val = symbol[rcv_byte - 32].val;

        if (symbol1.val == KEYBOARD_UNSUPPORTED)
          continue;

        if (symbol1.symbol.kbd_2)
        {
          if (!qwerty_output_buffer_kbd_2)
          {
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x001b;
            // qwerty_output_buffer[qwerty_output_buffer_end].symbol.drive = 1;
            // qwerty_output_buffer[qwerty_output_buffer_end].symbol.sense  = 11;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
          }
          qwerty_output_buffer_kbd_2 = 1;
        }
        else
        {
          if (qwerty_output_buffer_kbd_2)
          {
            qwerty_output_buffer[qwerty_output_buffer_end].val = 0x001b;
            // qwerty_output_buffer[qwerty_output_buffer_end].symbol.drive = 1;
            // qwerty_output_buffer[qwerty_output_buffer_end].symbol.sense  = 11;
            INC_BUFFER_IDX(qwerty_output_buffer_end,1);
          }
          qwerty_output_buffer_kbd_2 = 0;
        } /*if (symbol1.symbol.kbd_2)*/

        qwerty_output_buffer[qwerty_output_buffer_end].val = symbol1.val;
        INC_BUFFER_IDX(qwerty_output_buffer_end,1);

        // // sprintf(msg, "rcvd = %c: (%u) s%ud%u", (char) rcv_byte,
        // //         symbol1.symbol.shift, symbol1.symbol.drive, symbol1.symbol.sense);
        // // print_serial(msg);
        // // sprintf(msg, ": buffer start=%u, end=%u", qwerty_output_buffer_start, qwerty_output_buffer_end);
        // // print_serial(msg);
        // // sprintf(msg, ": typing = %u\r\n", qwerty_typing);
        // // print_serial(msg);

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

    // print_serial("serial input: ");
    // print_serial(usart2_rx_cmd_buffer);
    // print_serial("\r\n");

    idx_cmd=0;

_process_more:

    // sprintf(msg,"1:%lu:%lu\r\n",len_cmd,idx_cmd);
    // print_serial(msg);

    if (  *(usart2_rx_cmd_buffer+idx_cmd) == '\0'
      || (*(usart2_rx_cmd_buffer+idx_cmd) == '\n')
      || (*(usart2_rx_cmd_buffer+idx_cmd) == '\r') )
    {
      // print_serial("_p:exit\r\n");
      goto _exit;
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
            && (mux_it_mode           == QWERTY_MODE_INVERTED)
            && (qwerty_tim16_pwm_mode == QWERTY_MODE_INVERTED) )
          {
            qwerty_tim3_oc_mode   = QWERTY_MODE_POSITIVE;
            mux_it_mode           = QWERTY_MODE_POSITIVE;
            qwerty_tim16_pwm_mode = QWERTY_MODE_POSITIVE;
            goto _restart;
          }
        }

        if (*(usart2_rx_cmd_buffer+idx_cmd) == 'i')
        {
          idx_cmd++;
          if ( (qwerty_tim3_oc_mode   == QWERTY_MODE_POSITIVE)
            && (mux_it_mode           == QWERTY_MODE_POSITIVE)
            && (qwerty_tim16_pwm_mode == QWERTY_MODE_POSITIVE) )
          {
            qwerty_tim3_oc_mode    = QWERTY_MODE_INVERTED;
            mux_it_mode            = QWERTY_MODE_INVERTED;
            qwerty_tim16_pwm_mode  = QWERTY_MODE_INVERTED;
            goto _restart;
          }
        }
      }

    } /* if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"sys:", 4)) */

    if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"pwm:", 4))
    {
      idx_cmd += 4;

      if (strlen(usart2_rx_cmd_buffer+idx_cmd) < 6)
        goto _exit;

      if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"mode", 4))
      {
        idx_cmd += 4;

        GOBBLESPACES;

        if (*(usart2_rx_cmd_buffer+idx_cmd) == 'p')
        {
          idx_cmd++;
          qwerty_tim16_pwm_new_mode = TIM16_PWM_MODE_POSITIVE;
        }
        else if (*(usart2_rx_cmd_buffer+idx_cmd) == 'i')
        {
          idx_cmd++;
          qwerty_tim16_pwm_new_mode = TIM16_PWM_MODE_INVERTED;
        }

        goto _process_more;
      }

      if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"period", 6))
      {
        // gobble up spaces
        idx_cmd += 6;

        GOBBLESPACES;

        // check if user provided '?'
        if (*(usart2_rx_cmd_buffer+idx_cmd) == '?')
        {
          sprintf(msg,"%lu\r\n",qwerty_tim16_pwm_period);
          print_serial(msg);

          if (*(usart2_rx_cmd_buffer+idx_cmd) == '\0')
          {
            // print_serial("exiting\r\n");
            goto _exit;
          }

          // print_serial("more\r\n");
          goto _process_more;
        }

        // check that atoi() is getting digits to chew on
        if ( (*(usart2_rx_cmd_buffer+idx_cmd) < '0') || (*(usart2_rx_cmd_buffer+idx_cmd) > '9') )
        {
          print_serial("Err\r\n");
          goto _exit;
        }

        // gobble up the string either until ';' or '\r' and convert it into number
        new_pwm_period = atoi(usart2_rx_cmd_buffer+idx_cmd);

        if ( (0xffffffff == new_pwm_period) || (new_pwm_period == 0) )
        {
          new_pwm_period = qwerty_tim16_pwm_period;
        }
        sprintf(msg,"%lu\r\n",new_pwm_period);
        print_serial(msg);

        GOBBLEDIGITS;

      } /* if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"period", 6)) */

      if (strlen(usart2_rx_cmd_buffer+idx_cmd) < 5)
        goto _exit;
      if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"pulse", 5))
      {
        // gobble up spaces
        idx_cmd += 5;

        GOBBLESPACES;

        // check if user provided '?'
        // print_serial(usart2_rx_cmd_buffer);
        if (*(usart2_rx_cmd_buffer+idx_cmd) == '?')
        {
          sprintf(msg,"%lu\r\n",qwerty_tim16_pwm_pulse);
          print_serial(msg);
          goto _process_more;
        }

        // check that atoi() is getting digits to chew on
        if ( (*(usart2_rx_cmd_buffer+idx_cmd) < '0') || (*(usart2_rx_cmd_buffer+idx_cmd) > '9') )
        {
          print_serial("Err\n");
          goto _exit;
        }

        // gobble up the string either until ';' or '\r' and convert it into number
        new_pwm_pulse = atoi(usart2_rx_cmd_buffer+idx_cmd);

        if ( (0xffffffff == new_pwm_pulse) || (new_pwm_pulse == 0) )
        {
          new_pwm_pulse = qwerty_tim16_pwm_pulse;
        }
        sprintf(msg,"%lu\r\n",new_pwm_pulse);
        print_serial(msg);

        GOBBLEDIGITS;

      } /* if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"pulse", 6)) */

      if (strlen(usart2_rx_cmd_buffer+idx_cmd) < 6)
        goto _exit;

      if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"update", 6))
      {
        idx_cmd += 6;

        if (qwerty_tim16_pwm_new_mode == qwerty_tim16_pwm_mode)
        {
          if ( ((qwerty_tim16_pwm_period != new_pwm_period) ||
            (qwerty_tim16_pwm_pulse != new_pwm_pulse))
            && (new_pwm_period > new_pwm_pulse) )
          {
            TIM16->CCR1 = new_pwm_pulse;
            TIM16->ARR  = new_pwm_period;
            qwerty_tim16_pwm_pulse  = new_pwm_pulse;
            qwerty_tim16_pwm_period = new_pwm_period;
          }
        }
        else
        {
          qwerty_tim16_pwm_pulse  = new_pwm_pulse;
          qwerty_tim16_pwm_period = new_pwm_period;
          qwerty_tim16_pwm_mode   = qwerty_tim16_pwm_new_mode;
          MX_TIM16_Init();
          HAL_TIM_PWM_Start (&htim16, TIM_CHANNEL_1);
        }


        if (*(usart2_rx_cmd_buffer+idx_cmd) == '\0')
          goto _exit;

        goto _process_more;

      }

    } /* if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"pwm:", 4)) */

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
          IC_Value3 = 0;
          previous_IC_Value1 = 0;
          previous_IC_Value3 = 0;
          HAL_TIM_Base_MspDeInit(&htim3);
          MX_TIM3_Init();
          HAL_TIM_IC_Start_DMA(&htim3, TIM_CHANNEL_2,(uint32_t*)&IC_Value1,1);
          HAL_TIM_IC_Start_IT (&htim3, TIM_CHANNEL_4);
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
          IC_Value3 = 0;
          previous_IC_Value1 = 0;
          previous_IC_Value3 = 0;
          HAL_TIM_Base_MspDeInit(&htim3);
          MX_TIM3_Init();
          HAL_TIM_IC_Start_DMA(&htim3, TIM_CHANNEL_2,(uint32_t*)&IC_Value1,1);
          HAL_TIM_IC_Start_IT (&htim3, TIM_CHANNEL_4);
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
      idx_cmd += 11;

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

    } /*if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"drive:burst", 11))*/

    if ( *(usart2_rx_cmd_buffer+idx_cmd) == 'd' )
    {
      idx_cmd += 1;

      if (strlen(usart2_rx_cmd_buffer+idx_cmd) < 1)
        goto _exit;

      idx_drive_ch_new = *(usart2_rx_cmd_buffer+idx_cmd) - '0';
      if ((idx_drive_ch_new < 0) || (idx_drive_ch_new > 9))
      {
        print_serial("Drive Channel out of bounds! Err.\r\n");
        goto _exit;
      }

      idx_cmd += 1;

      if (strncmp(usart2_rx_cmd_buffer+idx_cmd,"s", 1))
        goto _exit;

      idx_cmd += 1;

      idx_sense_ch_new = atoi(usart2_rx_cmd_buffer+idx_cmd);
      if ( (idx_sense_ch_new < 1)
        || (idx_sense_ch_new == 5)
        || (idx_sense_ch_new == 9)
        || (idx_sense_ch_new == 10)
        || (idx_sense_ch_new >= 12) )
      {
        print_serial("Sense Channel out of bounds! Err.\r\n");
        goto _exit;
      }

      // update registers
      while (qwerty_typing){;}

      qwerty_tim14_oc_ccr =
        (idx_drive_ch_new == 0 ?  1        : TIM14_CCR + TIM14_PULSE * (idx_drive_ch_new - 1));
      qwerty_tim14_oc_arr =
        (idx_drive_ch_new == 0 ? TIM14_CCR : TIM14_CCR + TIM14_PULSE *  idx_drive_ch_new     );

      TIM14->CR1   &= ~TIM_CR1_CEN;
      qwerty_output_counter_have_update = 1;
      qwerty_count_typed_characters++;

      while (qwerty_output_counter_have_update){;}

      GOBBLEDIGITS;

      GOBBLESPACES;

    } /*if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"s", 1))*/

    if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"f10", 3))
    {
      idx_cmd += 3;

      // pressing 'shift' is ignored if 'code' is ON
      if (qwerty_state_code_on == 1)
        goto _exit;

      // wait until typing of previous character is over
      while (qwerty_typing){;}

      print_serial("Ok\r\n");

      if (qwerty_state_shift_on == 1)
      {
        HAL_GPIO_WritePin(QWERTY_F10_PORT, QWERTY_F10_PIN, mux_it_mode_gpio_pin_edge0_state);
        qwerty_state_shift_on = 0;
      }
      else
      {
        HAL_GPIO_WritePin(QWERTY_F10_PORT, QWERTY_F10_PIN, mux_it_mode_gpio_pin_edge1_state);
        qwerty_state_shift_on = 1;
      }

      GOBBLESPACES;
    }

    if (!strncmp(usart2_rx_cmd_buffer+idx_cmd,"f12", 3))
    {
      idx_cmd += 3;

      // pressing 'code' is ignored if 'shift' is ON
      if (qwerty_state_shift_on == 1)
        goto _exit;

      // wait until typing of previous character is over
      while (qwerty_typing){;}

      print_serial("Ok\r\n");

      if (qwerty_state_code_on == 1)
      {
        HAL_GPIO_WritePin(QWERTY_F12_PORT, QWERTY_F12_PIN, mux_it_mode_gpio_pin_edge0_state);
        qwerty_state_code_on = 0;
      }
      else
      {
        HAL_GPIO_WritePin(QWERTY_F12_PORT, QWERTY_F12_PIN, mux_it_mode_gpio_pin_edge1_state);
        qwerty_state_code_on = 1;
      }

      GOBBLESPACES;
    }

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
