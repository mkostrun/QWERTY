//
// typewriter states
//
#include "main.h"

/** 0 - command and configure state, uses CLI to interpret commands
  * 1 - typewriter, everything received on the serial port is forwarded to the
  *     typewriter to type
  **/
volatile uint8_t qwerty_typewriter_system_state;

// fixed lines:
uint8_t qwerty_state_shift_on = 0;
uint8_t qwerty_state_code_on = 0;

//
// special combinations of Drive/Sense
//
uint8_t qwerty_output_buffer_bold;
uint8_t qwerty_state_bold_on = 0;
uint8_t qwerty_output_buffer_underline;
uint8_t qwerty_state_underline_on = 0;
uint8_t qwerty_state_ext_on = 0;
uint8_t qwerty_output_buffer_par_indent;
uint8_t qwerty_output_buffer_tab_set;
uint8_t qwerty_output_buffer_just;

// line spacing
// 0, line spacing 1
// 1, line spacing 1 1/2
// 2, line spacing 2
// 3, line spacing 3
uint8_t       qwerty_output_buffer_line_spacing;
uint8_t       qwerty_state_line_spacing_state = 0;
const uint8_t qwerty_state_line_spacing_limit = 4;
const char   *qwerty_state_line_spacing_msg[] = {
  "LINE SPACING 1", "LINE SPACING 1 1/2", "LINE SPACING 2",
};

// font pitch
// 0, 10 CPI
// 1, 12 CPI, original font
// 2, 15 CPI
// 3, PS
uint8_t       qwerty_output_buffer_font_pitch;
uint8_t       qwerty_state_font_pitch_state   = 1;
const uint8_t qwerty_state_font_pitch_length  = 4;
const char   *qwerty_state_font_pitch_msg[] = {
  "10 CPI", "12 CPI", "15 CPI", "PS",
};


// typewriter has two keyboard available
uint8_t qwerty_output_buffer_kbd_2;
uint8_t qwerty_state_kbd_2 = 0;

// modes available
// 0,
// 1,
// 2,
uint8_t       qwerty_output_buffer_mode;
uint8_t       qwerty_state_mode_state = 0;
const uint8_t qwerty_state_mode_limit = 3;
const char   *qwerty_state_mode_msg[] = {
  "XXX", "XXX", "XXX",
};

uint8_t       qwerty_output_buffer_esc_received;
uint8_t       qwerty_output_buffer_null_received;


data qwerty_output_buffer[QWERTY_OUTPUT_BUFFER_LENGTH];
uint16_t qwerty_output_buffer_start;
uint16_t qwerty_output_buffer_end;



const data symbol[128] = {
  /* ' ' */   [  0].val = 0x0037,
  /* '!' */   [  1].val = 0x0123,
  /* '"' */   [  2].val = 0x01b0,
  /* '#' */   [  3].val = 0x0163,
  /* '$' */   [  4].val = 0x0173,
  /* '%' */   [  5].val = 0x0172,
  /* '&' */   [  6].val = 0x0183,
  /* ''' */   [  7].val = 0x00b0,
  /* '(' */   [  8].val = 0x01a3,
  /* ')' */   [  9].val = 0x01b3,
  /* '*' */   [ 10].val = 0x0193,
  /* '+' */   [ 11].val = 0x0192,
  /* ',' */   [ 12].val = 0x0096,
  /* '-' */   [ 13].val = 0x00b2,
  /* '.' */   [ 14].val = 0x00a6,
  /* '/' */   [ 15].val = 0x00b7,
  /* '0' */   [ 16].val = 0x00b3,
  /* '1' */   [ 17].val = 0x0023,
  /* '2' */   [ 18].val = 0x0053,
  /* '3' */   [ 19].val = 0x0063,
  /* '4' */   [ 20].val = 0x0073,
  /* '5' */   [ 21].val = 0x0072,
  /* '6' */   [ 22].val = 0x0082,
  /* '7' */   [ 23].val = 0x0083,
  /* '8' */   [ 24].val = 0x0093,
  /* '9' */   [ 25].val = 0x00a3,
  /* ':' */   [ 26].val = 0x01b1,
  /* ';' */   [ 27].val = 0x00b1,
  /* '<' */   [ 28].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /* '=' */   [ 29].val = 0x0092,
  /* '>' */   [ 30].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /* '?' */   [ 31].val = 0x01b7,
  /* '@' */   [ 32].val = 0x0153,
  /* 'A' */   [ 33].val = 0x0021 | KEYBOARD_SHIFT,
  /* 'B' */   [ 34].val = 0x0077 | KEYBOARD_SHIFT,
  /* 'C' */   [ 35].val = 0x0066 | KEYBOARD_SHIFT,
  /* 'D' */   [ 36].val = 0x0061 | KEYBOARD_SHIFT,
  /* 'E' */   [ 37].val = 0x0064 | KEYBOARD_SHIFT,
  /* 'F' */   [ 38].val = 0x0071 | KEYBOARD_SHIFT,
  /* 'G' */   [ 39].val = 0x0070 | KEYBOARD_SHIFT,
  /* 'H' */   [ 40].val = 0x0080 | KEYBOARD_SHIFT,
  /* 'I' */   [ 41].val = 0x0094 | KEYBOARD_SHIFT,
  /* 'J' */   [ 42].val = 0x0081 | KEYBOARD_SHIFT,
  /* 'K' */   [ 43].val = 0x0091 | KEYBOARD_SHIFT,
  /* 'L' */   [ 44].val = 0x00a1 | KEYBOARD_SHIFT,
  /* 'M' */   [ 45].val = 0x0086 | KEYBOARD_SHIFT,
  /* 'N' */   [ 46].val = 0x0087 | KEYBOARD_SHIFT,
  /* 'O' */   [ 47].val = 0x00a4 | KEYBOARD_SHIFT,
  /* 'P' */   [ 48].val = 0x00b4 | KEYBOARD_SHIFT,
  /* 'Q' */   [ 49].val = 0x0024 | KEYBOARD_SHIFT,
  /* 'R' */   [ 50].val = 0x0074 | KEYBOARD_SHIFT,
  /* 'S' */   [ 51].val = 0x0051 | KEYBOARD_SHIFT,
  /* 'T' */   [ 52].val = 0x0075 | KEYBOARD_SHIFT,
  /* 'U' */   [ 53].val = 0x0084 | KEYBOARD_SHIFT,
  /* 'V' */   [ 54].val = 0x0076 | KEYBOARD_SHIFT,
  /* 'W' */   [ 55].val = 0x0054 | KEYBOARD_SHIFT,
  /* 'X' */   [ 56].val = 0x0056 | KEYBOARD_SHIFT,
  /* 'Y' */   [ 57].val = 0x0085 | KEYBOARD_SHIFT,
  /* 'Z' */   [ 58].val = 0x0026 | KEYBOARD_SHIFT,
  /* '[' */   [ 59].val = 0x0095 | KEYBOARD_SHIFT,
  /* '\' */   [ 60].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /* ']' */   [ 61].val = 0x0095,
  /* '^' */   [ 62].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /* '_' */   [ 63].val = 0x00b2 | KEYBOARD_SHIFT,
  /* '`' */   [ 64].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /* 'a' */   [ 65].val = 0x0021,
  /* 'b' */   [ 66].val = 0x0077,
  /* 'c' */   [ 67].val = 0x0066,
  /* 'd' */   [ 68].val = 0x0061,
  /* 'e' */   [ 69].val = 0x0064,
  /* 'f' */   [ 70].val = 0x0071,
  /* 'g' */   [ 71].val = 0x0070,
  /* 'h' */   [ 72].val = 0x0080,
  /* 'i' */   [ 73].val = 0x0094,
  /* 'j' */   [ 74].val = 0x0081,
  /* 'k' */   [ 75].val = 0x0091,
  /* 'l' */   [ 76].val = 0x00a1,
  /* 'm' */   [ 77].val = 0x0086,
  /* 'n' */   [ 78].val = 0x0087,
  /* 'o' */   [ 79].val = 0x00a4,
  /* 'p' */   [ 80].val = 0x00b4,
  /* 'q' */   [ 81].val = 0x0024,
  /* 'r' */   [ 82].val = 0x0074,
  /* 's' */   [ 83].val = 0x0051,
  /* 't' */   [ 84].val = 0x0075,
  /* 'u' */   [ 85].val = 0x0084,
  /* 'v' */   [ 86].val = 0x0076,
  /* 'w' */   [ 87].val = 0x0054,
  /* 'x' */   [ 88].val = 0x0056,
  /* 'y' */   [ 89].val = 0x0085,
  /* 'z' */   [ 90].val = 0x0026,
  /*     */   [ 91].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [ 92].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [ 93].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [ 94].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [ 95].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [ 96].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [ 97].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [ 98].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [ 99].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [100].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [101].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [102].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [103].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [104].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [105].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [106].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [107].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [108].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [109].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [110].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [111].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [112].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [113].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [114].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [115].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [116].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [117].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [118].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [119].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [120].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [121].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [122].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [123].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [124].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [125].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [126].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /*     */   [127].val =          KEYBOARD_UNSUPPORTED    /* UNSUPPORTED */
};



