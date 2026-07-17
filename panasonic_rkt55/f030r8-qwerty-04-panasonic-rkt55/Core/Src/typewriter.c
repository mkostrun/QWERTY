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

// line spacing
// 0, line spacing 1
// 1, line spacing 1 1/2
// 2, line spacing 2
uint8_t       qwerty_output_buffer_line_spacing;
uint8_t       qwerty_state_line_spacing_state = 0;
const uint8_t qwerty_state_line_spacing_limit = 3;
const char   *qwerty_state_line_spacing_msg[] = {
  "LINE SPACING 1", "LINE SPACING 1 1/2", "LINE SPACING 2",
};
// font pitch
// 0, 10 CPI
// 1, 12 CPI
// 2, 15 CPI
// 3, PS
uint8_t       qwerty_output_buffer_font_pitch;
uint8_t       qwerty_state_font_pitch_state   = 0;
const uint8_t qwerty_state_font_pitch_length  = 4;
const char   *qwerty_state_font_pitch_msg[] = {
  "10 CPI", "12 CPI", "15 CPI", "PS",
};

// typewriter has two keyboard available
uint8_t qwerty_output_buffer_kbd_2;
uint8_t qwerty_state_kbd_2 = 0;

// justification: per EPSON printer codes
uint8_t       qwerty_output_buffer_just;
// modes available
// 0, typewriter
// 1, L/L (not used)
// 2, justified
uint8_t       qwerty_output_buffer_mode;
uint8_t       qwerty_state_mode_state = 0;
const uint8_t qwerty_state_mode_limit = 3;
const char   *qwerty_state_mode_msg[] = {
  "TPWR", "L/L", "JUST",
};


uint8_t       qwerty_output_buffer_esc_received;
uint8_t       qwerty_output_buffer_null_received;


data qwerty_output_buffer[QWERTY_OUTPUT_BUFFER_LENGTH];
uint16_t qwerty_output_buffer_start;
uint16_t qwerty_output_buffer_end;



const data symbol[128] = {
  /* ' ' */   [  0].val = 0x0027,
  /* '!' */   [  1].val = 0x0191,
  /* '"' */   [  2].val = 0x0108,
  /* '#' */   [  3].val = 0x0182,
  /* '$' */   [  4].val = 0x0192,
  /* '%' */   [  5].val = 0x0172,
  /* '&' */   [  6].val = 0x0197,
  /* ''' */   [  7].val = 0x0008,
  /* '(' */   [  8].val = 0x0193,
  /* ')' */   [  9].val = 0x0163,
  /* '*' */   [ 10].val = 0x0183,
  /* '+' */   [ 11].val = 0x0174,
  /* ',' */   [ 12].val = 0x0033,
  /* '-' */   [ 13].val = 0x0073,
  /* '.' */   [ 14].val = 0x0023,
  /* '/' */   [ 15].val = 0x0048,
  /* '0' */   [ 16].val = 0x0063,
  /* '1' */   [ 17].val = 0x0091,
  /* '2' */   [ 18].val = 0x0071,
  /* '3' */   [ 19].val = 0x0082,
  /* '4' */   [ 20].val = 0x0092,
  /* '5' */   [ 21].val = 0x0072,
  /* '6' */   [ 22].val = 0x0087,
  /* '7' */   [ 23].val = 0x0097,
  /* '8' */   [ 24].val = 0x0083,
  /* '9' */   [ 25].val = 0x0093,
  /* ':' */   [ 26].val = 0x0158,
  /* ';' */   [ 27].val = 0x0058,
  /* '<' */   [ 28].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /* '=' */   [ 29].val = 0x0074,
  /* '>' */   [ 30].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /* '?' */   [ 31].val = 0x0148,
  /* '@' */   [ 32].val = 0x0171,
  /* 'A' */   [ 33].val = 0x0041 | KEYBOARD_SHIFT,
  /* 'B' */   [ 34].val = 0x0017 | KEYBOARD_SHIFT,
  /* 'C' */   [ 35].val = 0x0032 | KEYBOARD_SHIFT,
  /* 'D' */   [ 36].val = 0x0042 | KEYBOARD_SHIFT,
  /* 'E' */   [ 37].val = 0x0062 | KEYBOARD_SHIFT,
  /* 'F' */   [ 38].val = 0x0012 | KEYBOARD_SHIFT,
  /* 'G' */   [ 39].val = 0x0007 | KEYBOARD_SHIFT,
  /* 'H' */   [ 40].val = 0x0057 | KEYBOARD_SHIFT,
  /* 'I' */   [ 41].val = 0x0053 | KEYBOARD_SHIFT,
  /* 'J' */   [ 42].val = 0x0047 | KEYBOARD_SHIFT,
  /* 'K' */   [ 43].val = 0x0003 | KEYBOARD_SHIFT,
  /* 'L' */   [ 44].val = 0x0043 | KEYBOARD_SHIFT,
  /* 'M' */   [ 45].val = 0x0013 | KEYBOARD_SHIFT,
  /* 'N' */   [ 46].val = 0x0037 | KEYBOARD_SHIFT,
  /* 'O' */   [ 47].val = 0x0098 | KEYBOARD_SHIFT,
  /* 'P' */   [ 48].val = 0x0078 | KEYBOARD_SHIFT,
  /* 'Q' */   [ 49].val = 0x0051 | KEYBOARD_SHIFT,
  /* 'R' */   [ 50].val = 0x0052 | KEYBOARD_SHIFT,
  /* 'S' */   [ 51].val = 0x0011 | KEYBOARD_SHIFT,
  /* 'T' */   [ 52].val = 0x0002 | KEYBOARD_SHIFT,
  /* 'U' */   [ 53].val = 0x0067 | KEYBOARD_SHIFT,
  /* 'V' */   [ 54].val = 0x0022 | KEYBOARD_SHIFT,
  /* 'W' */   [ 55].val = 0x0001 | KEYBOARD_SHIFT,
  /* 'X' */   [ 56].val = 0x0021 | KEYBOARD_SHIFT,
  /* 'Y' */   [ 57].val = 0x0077 | KEYBOARD_SHIFT,
  /* 'Z' */   [ 58].val = 0x0031 | KEYBOARD_SHIFT,
  /* '[' */   [ 59].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /* '\' */   [ 60].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /* ']' */   [ 61].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /* '^' */   [ 62].val = 0x0064 | KEYBOARD_SHIFT | KEYBOARD_KBD_2,
  /* '_' */   [ 63].val = 0x0073 | KEYBOARD_SHIFT,
  /* '`' */   [ 64].val =          KEYBOARD_UNSUPPORTED,   /* UNSUPPORTED */
  /* 'a' */   [ 65].val = 0x0041,
  /* 'b' */   [ 66].val = 0x0017,
  /* 'c' */   [ 67].val = 0x0032,
  /* 'd' */   [ 68].val = 0x0042,
  /* 'e' */   [ 69].val = 0x0062,
  /* 'f' */   [ 70].val = 0x0012,
  /* 'g' */   [ 71].val = 0x0007,
  /* 'h' */   [ 72].val = 0x0057,
  /* 'i' */   [ 73].val = 0x0053,
  /* 'j' */   [ 74].val = 0x0047,
  /* 'k' */   [ 75].val = 0x0003,
  /* 'l' */   [ 76].val = 0x0043,
  /* 'm' */   [ 77].val = 0x0013,
  /* 'n' */   [ 78].val = 0x0037,
  /* 'o' */   [ 79].val = 0x0098,
  /* 'p' */   [ 80].val = 0x0078,
  /* 'q' */   [ 81].val = 0x0051,
  /* 'r' */   [ 82].val = 0x0052,
  /* 's' */   [ 83].val = 0x0011,
  /* 't' */   [ 84].val = 0x0002,
  /* 'u' */   [ 85].val = 0x0067,
  /* 'v' */   [ 86].val = 0x0022,
  /* 'w' */   [ 87].val = 0x0001,
  /* 'x' */   [ 88].val = 0x0021,
  /* 'y' */   [ 89].val = 0x0077,
  /* 'z' */   [ 90].val = 0x0031,
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



