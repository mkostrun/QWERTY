#ifndef __typewriter_H
#define __typewriter_H

/* for panasonic, sense being is not valid, so this works */
#define KEYBOARD_UNSUPPORTED  (0x8000)
#define KEYBOARD_UNUSED       (0x7800)
#define KEYBOARD_KBD_2        (0x0400)
#define KEYBOARD_CODE         (0x0200)
#define KEYBOARD_SHIFT        (0x0100)
#define KEYBOARD_DRIVE_MASK   (0x00F0)
#define KEYBOARD_SENSE_MASK   (0x000F)

typedef struct _keyboard_matrix_symbol {
  /* LSB */
  uint8_t sense:        4 ;
  uint8_t drive:        4 ;
  /* MSB */
  uint8_t shift:        1 ;
  uint8_t code:         1 ;
  uint8_t kbd_2:        1 ;
  uint8_t unused:       4 ;
  uint8_t unsupported:  1 ;
} qwerty_symbol;

typedef union _union_keyboard_matrix_symbol
{
  uint16_t        val;
  qwerty_symbol   symbol;
} data;

#define  INC_BUFFER_IDX(old_val,inc_val) {\
  old_val += inc_val; \
  old_val  = old_val % QWERTY_OUTPUT_BUFFER_LENGTH; \
}

extern volatile uint8_t qwerty_typewriter_system_state;

extern data qwerty_output_buffer[];
extern uint16_t qwerty_output_buffer_start;
extern uint16_t qwerty_output_buffer_end;
extern uint8_t  qwerty_output_buffer_esc_received;
extern uint8_t  qwerty_output_buffer_null_received;
extern const data symbol[];



// fixed lines:
extern uint8_t qwerty_state_shift_on ;
extern uint8_t qwerty_state_code_on ;

//
// special combinations of Drive/Sense
//
extern uint8_t qwerty_output_buffer_bold ;
extern uint8_t qwerty_state_bold_on ;
extern uint8_t qwerty_output_buffer_underline ;
extern uint8_t qwerty_state_underline_on ;
extern uint8_t qwerty_state_ext_on ;
extern uint8_t qwerty_output_buffer_par_indent;
extern uint8_t qwerty_output_buffer_tab_set;
extern uint8_t qwerty_output_buffer_just;

// line spacing
// 0, line spacing 1
// 1, line spacing 1 1/2
// 2, line spacing 2
extern uint8_t       qwerty_output_buffer_line_spacing;
extern uint8_t       qwerty_state_line_spacing_state ;
extern const uint8_t qwerty_state_line_spacing_limit ;
extern const char   *qwerty_state_line_spacing_msg[];

// font pitch
// 0, 10 CPI
// 1, 12 CPI
// 2, 15 CPI
// 3, PS
extern uint8_t       qwerty_output_buffer_font_pitch;
extern uint8_t       qwerty_state_font_pitch_state   ;
extern const uint8_t qwerty_state_font_pitch_length  ;
extern const char   *qwerty_state_font_pitch_msg[];

// typewriter has two keyboard available
extern uint8_t qwerty_output_buffer_kbd_2;
extern uint8_t qwerty_state_kbd_2 ;

// modes available
// 0,
// 1,
// 2,
extern uint8_t       qwerty_output_buffer_mode;
extern uint8_t       qwerty_state_mode_state ;
extern const uint8_t qwerty_state_mode_limit ;
extern const char   *qwerty_state_mode_msg[];

#endif
