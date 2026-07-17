#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"

//#define GPIO_INVERTED_MODE_INACTIVE_STATE_DIGITAL_OUTPUT  (0)
#define GPIO_INVERTED_MODE_INACTIVE_STATE_ANALOG_INPUT    (1)

#undef  QWERTY_MUX_PIN
   /* QWERTY: PIN ARD A2/PA1 */
   // #define QWERTY_MUX_PIN  GPIO_PIN_1
// #define QWERTY_MUX_PORT GPIOA

/** QWERTY SENSE DEFINITION: NUMBERING FOLLOWS THE
  * NUMBERING OF THE SENSE (RIGHT) RIBBON CABLE:
  *   0 (TOP) to 12 (BOTTOM)
  * SENSE Channels:
  *   0   -> VCC
  *  1:4  -> SIGNAL INPUTS TO TYPEWRITER (22.8 KOHM PULLUPS TO VCC)
  *   5   -> GND
  *  6:12 -> SIGNAL INPUTS TO TYPEWRITER (22.8 KOHM PULLUPS TO VCC)
  *
  **/
#define QWERTY_NUM_SENSE (12L)

/* TYPEWRITER SENSE: PIN D0 <-> VCC */

/* TYPEWRITER SENSE: PIN D1 <-> PA12 */
#undef  QWERTY_D1_PIN
#define QWERTY_D1_PIN   GPIO_PIN_12
#define QWERTY_D1_PORT  GPIOA

/* TYPEWRITER SENSE: PIN D2 <-> PA5 */
#undef  QWERTY_D2_PIN
#define QWERTY_D2_PIN   GPIO_PIN_5
#define QWERTY_D2_PORT  GPIOA

/* TYPEWRITER SENSE: PIN D3 <-> PA11 */
#undef  QWERTY_D3_PIN
#define QWERTY_D3_PIN   GPIO_PIN_11
#define QWERTY_D3_PORT  GPIOA

/* TYPEWRITER SENSE: PIN D4 <-> PA6 */
#undef  QWERTY_D4_PIN
#define QWERTY_D4_PIN   GPIO_PIN_6
#define QWERTY_D4_PORT  GPIOA

/* TYPEWRITER SENSE: PIN D5 <-> GND */

/* TYPEWRITER SENSE: PIN D6 <-> PB12 */
#undef  QWERTY_D6_PIN
#define QWERTY_D6_PIN   GPIO_PIN_12
#define QWERTY_D6_PORT  GPIOB

/* TYPEWRITER SENSE: PIN D7 <-> PB11 */
#undef  QWERTY_D7_PIN
#define QWERTY_D7_PIN   GPIO_PIN_11
#define QWERTY_D7_PORT  GPIOB

/* TYPEWRITER SENSE: PIN D8 <-> PB6 */
#undef  QWERTY_D8_PIN
#define QWERTY_D8_PIN   GPIO_PIN_6
#define QWERTY_D8_PORT  GPIOB

/* TYPEWRITER SENSE: PIN F9 <-> N/C */

/* TYPEWRITER SENSE: PIN D10 <-> PC7 */
#undef  QWERTY_F10_PIN
#define QWERTY_F10_PIN   GPIO_PIN_7
#define QWERTY_F10_PORT  GPIOC

/* TYPEWRITER SENSE: PIN D11 <-> PB2 */
#undef  QWERTY_D11_PIN
#define QWERTY_D11_PIN   GPIO_PIN_2
#define QWERTY_D11_PORT  GPIOB

/* TYPEWRITER SENSE: PIN D12 <-> PA9 */
#undef  QWERTY_F12_PIN
#define QWERTY_F12_PIN   GPIO_PIN_9
#define QWERTY_F12_PORT  GPIOA

typedef struct _struct_pin {
  GPIO_TypeDef * port;
  uint16_t        pin;
  uint16_t      fixed;
} struct_pin;

extern volatile uint32_t gpio_pin_initial_state;
extern volatile uint32_t gpio_pin_initial_pull;
extern volatile uint16_t idx_sense;
extern volatile uint16_t new_idx_sense;
extern const struct_pin querty_sense[QWERTY_NUM_SENSE + 1];

void MX_GPIO_Init(void);

#ifdef __cplusplus
}
#endif
#endif

