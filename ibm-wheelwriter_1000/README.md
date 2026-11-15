# QWERTY (**Q**uery and **W**hisper to **E**lectronic **R**emote **TY**pewriter)

# Flavor: IBM WheelWriter 1000 Typewriter

## Executive Summary
Nucleo 64 - STM32F030 (Arm Cortex M0, 8KB SRAM, 64KB FLASH) 
development board is used for emulation of keyboard matrix in a IBM WheelWriter (WW) 1000 typewriter.
The PC host controls the typewriter through serial over USB, where it can be used either as a standard ASCII printer, or through a terminal program such as minicom it can be used directly as a typewriter.

## Abstract

The project provides firmware for Nucleo 64 - STM32F030, which with proper wiring to the typewriter sense and drive lines, 
allows it to control the typewriter IBM WW 1000 keyboard matrix.
In addition, additional PITCH SWITCH  wiring modification is presented, which allows the firmware to partially control the font pitch of
the typewriter (10p, 12p, 15p and PS).

While the QWERTY project addresses how to emulate a keyboard matrix in general terms, this flavor addresses relevant details of implementation:

0. TTL to 3.3V conversion: All tested typewriters use 5V positive logic. STM32 MCU operates at 3.3V.
    - The typewriter sense lines are driven by STM32 through usage of two output states: Active State at 0V, and Inactive State as Analog Input. This is because in the typewriter, all sense lines have pull-up resistors. 
    - The typewriter drive line(s) feeding into input capture channels on STM32 MCU may be protected with a signal diode (from STM to TY) and a 3.3V pull-up resistor.
1.  S-connector (right on the main board) has 13 lines:  S0 is 5V, S5 is 0V, while S1:4 and S6:12 are used for keys (11 lines).  
2.  D-connector (left on the main board) has 13 lines: D0:7 and D9 for keys, D8 is 0V and D12 is 5V.
3. The period of drive pulse bursts is $tper = 10.3$ ms (97 Hz), while a pulse length is $tpul = 83$ us (12 kHz). 
4. The drive pulses relative delays in units of $tpul$ are: D0/0, D1/1, D2/-1, D3/-2, D4/-3, D5/-9, D6/-5, D7/-4, D9/-7. Upon pressing a key, the drive pulse is unmodified. Thus, the firmware uses D5 line for input capture, and from it computes the timings of the other 8 drive pulses. D5 through interrupt on leading edge drives the multiplexer for sense lines (from the leading edge, the firmware computes start and end of the drive pulse, and dumps it on the sense line for the key that was pressed).
5. The sense lines are split into two groups: (true) sense lines S1:4 and S6:9, and fixed (sense) lines S10:12. Fixed lines correspond to the keys LOCK, SHIFT and CODE. The firmware emulates drive pulses to sense lines through multiplexing, while synchronously activating and inactivating the fixed lines.
6. To get a key registered by the typewriter, the minimum number of pulses is 4 periods. Between two consecutive keys there has to be 6 period blank. This is consistent with the 9 characters per second (CPS) speed of the typewriter. 

Firmware through various MACROS allow configuration of delays, and sense and fixed lines. Firmware requires D5 to be to specific pin (TIM3/CH1 Input Capture, with pulse edge generated interrupts for measuring the pulse duration, and period, and reporting it to the user when firmware is operating in CLI mode).

## Supported EPSON Printer Codes

Native font pitch: 12

1.  Font Pitch:
     - ESC  P            - Pica, or Pitch 10
     - ESC  M            - Elite, or Pitch 12
     - ESC 0x0f          - Condensed, or Pitch 15
     - ESC  p            - Proportional, Pitch PS
2. Vertical or Horizontal Motion
    - 0x08              - Backspace
    - 0x09              - Horizontal Tab
    - ESC  0            - Line space 1
    - ESC  1            - Line space 1-1/2
    - ESC  2            - Line space 2
    - ESC  D  0         - Clear Horizontal Tabs
    - ESC  D  1         - Set Horizontal Tab Here
    - ESC  Q  0xNN      - Set right margin at NN column in hex
    - ESC  l  0xMM      - Set left margin at MM column in hex
3. Print Enhancement
    - ESC  -  0         - Cancel Underline
    - ESC  -  1         - Start Underline
    - ESC  E            - Start Bold
    - ESC  F            - Cancel Bold
 4. Justification
    - ESC  a  0         - Left
    - ESC  a  1         - Center
    - ESC  a  2         - Right
    - ESC  a  3         - "Full" when close to the right margin, on key-space jump to the next line
5. Custom ESC (EPSON non-compliant) Sequences
    - ESC  i  0         - Clear paragraph indent
    - ESC  i  1         - Set paragraph indent here
6. Keyboard
    - ARROWS (UP, DOWN, LEFT, RIGHT)
    - BACKSPACE
    - DELETE
    - required space as TILDE.

## 2. TLDR;

As this was done before I worked on IBM WW1000, the pin designation below and in the source code was exactly oposite: 
- me  at that time: Source Pins and Drain Pins
- IBM: Source -> Drive Pins; and Drain -> Sense Pins. Ahhh. Too lazy to go back and change PAN firmware with updated notation.
- Smith-Corona and Panasonic seem to use terms 'row pins' and 'column pins,' which, to my opinion, obfuscate their functional role.

### 2.0 I wondered lonely as a cloud, when all at once ...

IBM WW 1000 was an unexpected Craigslist find for $40. It came with extra ribbons and correction tapes. On 
e-bay these go for $100 up. 

### 2.1 Keyboard access

Through keyboard, one has access to line spacing to 1, 1 1/2, 2 or 3. IBM supports left (typewriter) and center justification, and right-flush-margin. It has this interesting "Auto" feature: if space is typed "near" the right margin, then it acts as if Carriage Return was pressed and it moves to the beginning of new line.

### 2.2 Printer Option

IBM WW 1000 does not come with computer control interface. Back in a day, one was able to buy an add-on called "printer option." 
With it, a computer was able to control a typewriter through parallel port, where the typewriter would appear as an ASCII or raw printer. All printable characters could have been printed this way, and the printer supported BOLD and UNDERLINE features. These features were implemented through Epson Printer Codes (EPC).

Motivation for developing the keyboard emulator for IBM typewriters comes from a desire to have all features of the typewriter accessible through computer interface. Furthermore, it would be great if the keyboard emulator would implement these additional features in form of EPCs, so that it would turn this typewriter into an EPC compliant ASCII printer.


## 3. Analysis of IBM WW1000 Keyboard Matrix

Based on existing body of contemporaneous knowledge and visual and electrical inspection of the typewriter, it was found that the keyboard matrix is executed through 15 drive lines D0:14, and 8 sense lines S0:7. 
Notice that we count drive lines by delay and not by the cable pin at which they appear. Remaining 7 sense lines S8:14 consist of six lines that the typewriter logic (in further text just _the logic_) uses to identify the keyboard connected to it, and GND.

A keyboard key connects one drive line to one sense line. Unlike other typewriters, and as discussed in IBM patents on the topic from the era, upon pressing a key on the keyboard the first drive pulse extends. Similarly, upon releasing that key, the logic makes drive pulse put out one more pulse of the same length as the first. It is not difficult to see that with this pulse management, there is no fixed drive pulse repetition rate.
For that reason, the QWERTY firmware uses trailing edge of the drive pulses to simulate drive pulses that are fed to the sense lines. In the case of PAN typewriter, which had fixed pulse width and phase irrespectively whether a key was pressed or not, the leading edge of drive pulses was used for the same purpose.

In IBM logic, there is only a single type of sense line. The challange this presented is how to send two pulses simultaneosly to the typewriter logic sense lines.
The QWERTY firmware solves that as follows. The delay associated with SHIFT is chosen as a reference and so sits on D0 (Pin 3) drive line and CODE on D4 (Pin 14). The timing difference between these two of 4 pulse widths allows all drive pulses with delays to be created: Limitations of input capture is that the stretched pulse created from line D0 has to end before D0 repeats itself. 

For the drive lines to be read with external analyzer or oscilloscope, it need be pulled up to 5V.

The drive lines were each examined with the oscilloscope.

It was confirmed that overall, the typewriter electronics operate in "inverted" mode (HI = 0, LO = 5V).
More importantly, the electronics of the typewriter identifies each drive line through two elements: 

1.  The drive line has fixed pulse rate
The baseline pulse rate was measured to be around *f* = 98 Hz (period *tp* = 10.3 ms). Each baseline pulse has duration *td* = 84 us (*fd* = 12 kHz).

2. The drive line has fixed delay with respect to Baseline Pulse:\
There exists a drive line which is in-sync with, what we call, the baseline pulse. 
All the other drive lines  have fixed delay from the baseline pulse, but the same rate.
Of the 10 drive lines, it was found that the line 5 (6 pin from top of the connector when looking onto the PCB) has the earliest appearance in the burst, so it was identified as THE baseline. 
All the other drive lines were some multiple of the delay *td* from it.\
Thus, for the purpose of emulating drive lines in an micro-controller all that is needed is the baseline pulses themselves, and their delay. 
In the firmware the drive channels are numbered S[0:9] where the index of the channel refers to delay from the 5-th baseline pulse: 
S0 corresponding to 5-th line without delay, S1 corresponding to a pulse which starts at the end of baseline pulse, and goes for *td*, and so forth for higher drive channels.\
The micro-controller uses software triggered timer to creates each of the D[0:9] drive channels. 
Here it is important to remark that 0-delay pulses cannot be created using this technique. 
Instead, we choose an appropriate pre-scaler (20 for STM32F030 operating at 48MHz, so the shortest time step is 1/2.4MHz), and then specifying delay for 0-delay pulse as 1. 
The delay achieved in this fashion was around 8 us, and could not be further reduced (corresponds to timing of interrupts and triggers between `TIM3/CH2` input trigger, and interrupts on TIM14
that drove the GPIO multiplexer).

## 4. Firmware Realization

The author chose the micro-controller STM32F03 for no particular reason, except that there was one hiding in the box of stuff. 
An ARM M0 seems like appropriate micro-controller for this project as it has enough memory (68K) and speed (48MHz).
More importantly, the development board allows access to a block of 14 pins that can be wired to emulate the typewriter keyboard.
The code provided on github takes some 22KBytes.

The firmware for micro-controller allows one to send all keyboard keys as inputs to the typewriter.

There is no feedback from the typewriter.

The firmware implements the following functions:

 1. PWM
 2. Two Channel Input Capture for detecting period,pulse and mutual delay between two capture channels;
 3. Software triggered internal timer: Input Capture Primary channel may trigger software timer in delay/period overflow mode, and use those interrupts to drive GPIO multiplexer;
 4.  The command line interface (CLI) through serial port connection to the personal computer. Serial port interface uses DMA channel for receiving to circular buffer, and interrupt transmission. 

**Important** : For the pin that is triggered by the typewriter S5 line, because of voltage difference MCU 3.3V vs typewriter 5V there is a diode (cathode facing S5) and pull-up resistor to 3.3V on MCU side that separate the two.\
For the pins that communicate to the typewriter (sense lines D/F), their states are "analog input" (inactive state) or digital output 0 (active state).\
ONLY IN THIS CONFIGURATION CAN THE TYPEWRITER KEYBOARD AND MCU KEYBOARD EMULATOR CO-EXIST. IF ONE INSISTS MCU PINS TO BE DIGITAL OUTPUT IN ACTIVE AND IN INACTIVE STATE THEN EITHER THE TYPEWRITER KEYBOARD IS INOPERATIONAL OR THE SIGNAL DIODES NEED BE INSERTED IN SENSE/FIXED LINES.

Firmware functions are described next, together with the CLI commands to control them.

### 4.1 PWM

TIM16 is used for creation of  PWM pulses for the purpose of providing test input for the other functions in the firmware. The timer is chosen because the pin on which the pulses are created is conveniently located at Arduino pin D15/PB8.

To obtain Panasonic values for *tp* and *td*,  and considering the micro-controller operating frequency of *F* = 48 MHz, 
with prescaler 20, so one clock cycle is 1/2.4MHz, the pulse is long 

TIM16->ARR=200,

and the period 

TIM16->CCR1=24600. 
 
The timer and the entire firmware can operate in two modes
1. **positive**: HI=3.3V, LO=0V; or,
2.  **inverted** : HI=0V, and LO=3.3V.

 The typewriter uses inverted logic, so this is how the firmware boots as well. The firmware can change its mode, but the command for that is hidden in the source code.

Command line interface:\
`pwm:pulse 150` set the PWM pulse length to 150. Needs update;\
`pwm:period 1050` set the PWM period length to 150. Needs update;\
`pwm:mode p` set the PWM mode to positive, other option is `i` for inverted. Needs update;\
`pwm:pulse?` 	query the active PWM pulse length;\
`pwm:period?` 	query the active PWM period length;\
`pwm:update`	if values have been changed using the commands above, then this is needed to upload the updated values and restart  the PWM generator.

### 4.2 Input Capture

In the development of the firmware TIM3 was used for this function from the beginning. 
What varied during the development was which channels were used.
At some point even DMA was used to transfer measurement data, but that was abandoned after it was found that on this particular micro-controller only one channel can be DMA'd, while the other had to rely on interrupt for data transfer. For that reason DMA was abandoned altogether and everything is done through interrupts.
Final selection of the used channels was done after the cable for the connection to the Panasonic board was purchased. \

Using `TIM3/CH2 PA7`, and
`TIM3/CH4 PC9` are the most convinient. On one hand side, the PWM pin `PB8` is easily accessed by `PC9` (they are neighbours), while `PA7` is a part of block of 14 pins that are through single 0.1" pitch connector brought to the  typewriter PCB on the solder joint side, so that the emulator can coexist with the native keyboard.

`meas:ch1?` reports period, pulse, and delay (always 0) for Input Capture channel 1;

`meas:ch2?` reports the same for Input Capture channel 2. We notice that the delay2 measurement requires two pull-up resistors.
An oscilloscope with its clamps provides better connection than futzing with the wires, so we used that instead.

E.g., through this method we find for the Panasonic RK-T55 values of the pulse of 200, and period a random number in the range of 24665 to 24681.
Luckily, for this project we source these signals from the typewriter PCB.

### 4.3 Typing through CLI

There are two commands that are available for typing, 

`src:burst B`  selects number of baseline pulses `B` that will be sourced by the microcontroller

`dNsM`  drives the channel `N` to sense line `M` for the number of pulses `B`. For this particular typewriter,\
 `N`=0:9 and `M` = 1:4, 6:8, 12. 
Importantly, the shortest length of `B` is 2. For that reason built-in default value is 3. 
Similarly, after sending this combination, the firmware for next `L` pulses prevents sending anything to the typewriter.
Currently, `L` = 5 is implemented in software. 
Through this selection of values, it takes 8 baseline pulses to send a symbol ( = particular Drive,Sense combination) to the typewriter.
This in effect makes the typing speed to 98Hz/8 = 12 symbols per second.
This is almost identical to 12 characters per second (CPS) nominal speed of typing per the typewriter specifications.

`f10` press shift key, done directly, not through interrupts

`f12` press CODE key. done directly, not through interrupts

While typing, the firmware blocks further sending of symbols to the typewriter.
This blocking mode is necessary if one wants to type more than one symbol over the command line: then one would put semi-column between symbols.

`s1d3;s4d1;s1d3;s4d1;s3d4`

to type *mama*. Refer to Typewriter Matrix Table for interpretation of S/D lines to typewriter symbols.

Panasonic RK-T55 Typewriter Keyboard Matrix. Note that source (IBM:drive) lines are numbered by their delay, and not by their physical numbering on the connector.

| **S** | **0** | **1** | **2** | **3** | **4** | **5** | **6** | **7** | **8** | **9** |
|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|
| **D0** | 5V | | | | | | | | | |
| **D1** | w | s | x | z | a | q | | 2 | | 1 |
| **D2** | t | f | v | c | d | r | e | 5 | 3 | 4 |
| **D3** | k | m | . | , | l | i | 0 | - | 8 | 9 |
| **D4** | SPELL | | | RET | | | lb | | = | FILE |
| **D5** | GND | | | | | | | | | |
| **D6** | AUTO | EXT | | | BOLD | UNDER | | BS | | SPACE |
| **D7** | g | b | SPACE | n | j | h | u | y | 6 | 7 |
| **D8** | ' | DEL | DOWN | UP | / | ; | 1/2 | p | VTAB | o |
| **D9** | LOCK | | | | | | | | | |
| **D10** | SHIFT | | | | | | | | | |
| **D11** | LINE SP | KBD | | | | | PITCH | | | MODE |
| **D12** | CODE | | | | | | | | | |

PS: I am getting old. I had to use _copilot_ to fix the table formatting, as Keenwrite deemed my original table fine but github did not.