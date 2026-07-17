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
#define QWERTY_MUX_PIN  GPIO_PIN_1
#define QWERTY_MUX_PORT GPIOA

  /** QWERTY SENSE DEFINITION: NUMBERING FOLLOWS THE
  * NUMBERING OF THE SENSE (RIGHT) RIBBON CABLE:
  *   0 (LEFT) to 7 (MIDDLE) WHERE 8:13 SELECT KEYBOARD
  *
  **/
#define QWERTY_NUM_SENSE (8L)

/* TYPEWRITER SENSE: PIN S0 <-> PA12 */
#undef  QWERTY_S0_PIN
#define QWERTY_S0_PIN   GPIO_PIN_12
#define QWERTY_S0_PORT  GPIOA

/* TYPEWRITER SENSE: PIN S1 <-> PA5 */
#undef  QWERTY_S1_PIN
#define QWERTY_S1_PIN   GPIO_PIN_5
#define QWERTY_S1_PORT  GPIOA

/* TYPEWRITER SENSE: PIN S2 <-> PA11 */
#undef  QWERTY_S2_PIN
#define QWERTY_S2_PIN   GPIO_PIN_11
#define QWERTY_S2_PORT  GPIOA

/* TYPEWRITER SENSE: PIN S3 <-> PB12 */
#undef  QWERTY_S3_PIN
#define QWERTY_S3_PIN   GPIO_PIN_12
#define QWERTY_S3_PORT  GPIOB

/* TYPEWRITER SENSE: PIN S4 <-> PB11 */
#undef  QWERTY_S4_PIN
#define QWERTY_S4_PIN   GPIO_PIN_11
#define QWERTY_S4_PORT  GPIOB

/* TYPEWRITER SENSE: PIN S5 <-> PB6 */
#undef  QWERTY_S5_PIN
#define QWERTY_S5_PIN   GPIO_PIN_6
#define QWERTY_S5_PORT  GPIOB

/* TYPEWRITER SENSE: PIN S6 <-> PC7 */
#undef  QWERTY_S6_PIN
#define QWERTY_S6_PIN   GPIO_PIN_7
#define QWERTY_S6_PORT  GPIOC

/* TYPEWRITER SENSE: PIN S7 <-> PB2 */
#undef  QWERTY_S7_PIN
#define QWERTY_S7_PIN   GPIO_PIN_2
#define QWERTY_S7_PORT  GPIOB

/* TYPEWRITER DRIVE: PIN DL <-> PB8 from Typewriter*/
#undef  QWERTY_DL_PIN
#define QWERTY_DL_PIN   GPIO_PIN_8
#define QWERTY_DL_PORT  GPIOB

/* TYPEWRITER SENSE: PIN SL <-> PC6 to Typewriter*/
#undef  QWERTY_SL_PIN
#define QWERTY_SL_PIN   GPIO_PIN_6
#define QWERTY_SL_PORT  GPIOC

typedef struct _struct_pin {
  GPIO_TypeDef * port;
  uint16_t        pin;
  uint16_t      fixed;
} struct_pin;

extern volatile uint16_t idx_sense;
extern volatile uint16_t new_idx_sense;
extern volatile uint8_t gpio_dl_pin_edge;
extern const struct_pin querty_sense[QWERTY_NUM_SENSE];

void MX_GPIO_Init(void);

#ifdef __cplusplus
}
#endif
#endif

