# QWERTY (**Q**uery and **W**hisper to **E**lectronic **R**emote **TY**pewriter)

## Executive Summary

Micro-controller (MC) based solution for emulation of typewriter keyboard matrix is proposed.

In an electronic typewriter, the keyboard matrix consists of two sets of connections to the typewriter motherboard: drive pins and sense pins, where a key on the keyboard makes steady connection between one drive and one sense pin.\
Novelty in the presented solution is that it uses the earliest of the drive signals in the drive signals set, and from it generates internally delayed drive pulses. These delayed pulses are then provided to different sense pins.\
Besides emulating the keyboard, the solution allows one to explore the typewriter matrix and find values of the delays for drive pins. 
This is done through built-in command line interface over serial port.\
STM32F030 (Arm Cortex M0) is used for realization of the solution, but this is done using generic HAL-infrastructure, so other MC's can be implemented, as well.

## Word of the wise

To successfully complete this project, one will have to learn a lot about 40-year old electronic typewriters.\
Through synchronicity, this project was initially organized around a Panasonic RK-T55 typewriter, which is similar to other typewriters from RK, or T-series in that the printing element is the, so called, cupwheel:\
The cupwheel comprise pairs of letters and symbols on plastic bars attached to a gear. A plastic bar carries an elevated relief of the letter covered with thin metal layer. 
While developing the MC firmware, it was discovered that under hits from the typewriter hammer these letters would first decompose (metal separated from plastic resulting is a messy imprint of a symbol or letter) and then fall of (a piece would break off the plastic bar at the place of hammer impact). 
After 40 or so years of hibernation (silent aging of the part) when trying to use such a converted typewriter as a printer, one need be prepared that its cupwheel may become unusable after few pages of text. 
This became apparent on the original cupwheel which came with the typewriter, and was confirmed on the second cupwheel purchased shortly after the first started failing.\
The main inference, if not conclusion, is that in an electronic typewriter the cupwheel (or likely the typewheel, as well) is a consumable, and that persons using old electronic typewriters as printers better have a supply of cupwheels/typewheels.\
For that reason, after initial development of firmware different typewriter was chosen for project continuation, namely, IBM Wheelwriter 1000, for which there is abundance of parts cupwheels/typewheels available on the internet, and which has reputation that it was "built like a tank."

Another mechanicists theory that could explain this rapid demise of cupwheels is that a rate of loss of symbols off the well-aged cupwheels is a function of steady typewriting speed.  

## Abstract
The project addresses one solution to remote control of old Panasonic electronic typewriters,
which is through keyboard matrix emulation. The keyboard matrix of a Panasonic typewriter RK-T55 is described.
A solution is presented based on STM32F03-Nucleo64 micro-controller platform, which implements the following functional elements:
1. PWM, through TIM16, for creating test pulses that emulate base line pulses in the typewriter;
2. 2 Channel Input Capture, through TIM3, which measures pulse, period and delay between two channels, which with external pull-resistors to 5V can be used for sensing the typewriter S[0:9] channels;
3. Software timer TIM14, which can be triggered by TIM3/CH2, in one-pulse-like mode. It creates interrupts on delay and period overflow, and the IRQ Handlers in effect create GPIP Multiplexer for the typewriter sense channels.

The micro-controller STM32F03 is command driven through serial port, and command line interface is developed that allows one to control all elements of each functional part.
In the presented solution, the MC is connected to typewriter through 13 wires (S5 with pull-up, D/F[1:12]), and to the computer through USB-to-serial cable.
This allows full access to all keyboard keys for typewriting and control of the RK-T55 typewriter.

## 1. On the Shoulders Of

There is considerable amount of work posted on how to connect the electronic typewriters from the period 1980-90's to modern computers.
Most of the work aims at reverse engineering the communication protocol of various serial-look alike protocols using inexpensive micro-controller prototyping boards.
Two of the projects listed below, however, 
recognized the presence of keyboard matrix inside the typewriter,
provided rudimentary signal plots of various drive lines:
- McChristy created a massive micro-controller controlled analog switching matrix through multi-channel multiplexer/demultiplexer integrated circuits. Obviously, such a solution required additional electronic components, specialized printed circuit boards, and extensive wiring.
-  On the other hand, Chataignon used Arduino input and output pins to emulate typewriter matrix through bit-banging. For predefined duration of time, Arduino sits on one [Drive] pin and transfer all activity to another [Sense] pin.

The work presented here may be considered an evolution of these two approaches.

Sources, all retrived in March, 2026, relevant for this project are:
1. [McChristy](https://mrchristyengineering.wordpress.com/2024/12/09/hello-world/)  (n.d.)
2. [Alexandre Chataignon, Typewriter Arduino project](https://github.com/xouillet/TypeWriter) (n.d.)
3. [Troy Deck,  Reverse engineering the Panasonic Thermalwriter Computer Interface](https://github.com/tdeck/thermalwriter-interface) (n.d.) pages on reverse engineering a communication protocol between Panasonic parallel interface box and the typewriter.
4. [stackexchange](https://retrocomputing.stackexchange.com/questions/13385/panasonic-kx-w50th-data-transfer/13510) lament on data transfer protocol. 

Useful is service manual of another Panasonic thermal typewriter, which gives an idea of how many and what kind of signals to expect between drive and sense channels in their **keyboard matrix.**

## 2. Introduction

Through actions of *The Big Random Number Generator in The Sky*, a typewriter Panasonic RK-T55 landed in the author's lap.

### 2.1 Keyboard access

Through keyboard, one has full access to the typewriter's features.
E.g., one can access various font pitch sizes (10, 12, 15, PS), 
and change line spacing to 1, 1 1/2 or 2.
One can also set/change margins, vertical and horizontal tabs, move around the paper (UP, DOWN, LEFT, RIGHT), or execute Micro-motion.

### 2.2 Parallel Interface

The typewriter comes with a Centronics parallel interface, 
which allows a computer to control the typewriter as an ASCII or raw printer.
All printable characters can be printed this way, and the printer
supports BOLD and UNDERLINE features.
The features are implemented through Epson Printer Codes (EPC).
These are sequences of symbols starting with ESC symbol (0x1b)
after which one or more symbols follow.
Interestingly, the typewriter absorbs the simplest EPCs (ESC + one symbol) even though it doesn't act on them (except the aforementioned two).

Motivation for developing the keyboard emulator for Panasonic typewriters comes  from desire to have all features of the typewriter accessible through computer interface.
Furthermore, it would be great if the keyboard emulator would implement these additional features in form of EPCs, so that it would turn this typewriter into an EPC compliant ASCII printer.


## 3. Analysis of RK-T55 Keyboard Matrix

Based on existing body of contemporaneous knowledge and visual and electrical inspection of the typewriter, it was found that the keyboard matrix is executed through 10 drive lines, and 11 sense lines. 

A keyboard key connects one drive line to one sense line. There are two types of sense lines: those which expect signals described below, and those which to become active need be grounded (LOCK, SHIFT, CODE).

For the drive lines to be read with external analyzer or oscilloscope, it need be pulled up to 5V.

The drive lines were each examined with the oscilloscope.

It was confirmed that overall, the typewriter electronics operate in "inverted" mode (HI = 0, LO = 5V).
More importantly, the electronics of the typewriter identifies each drive line through two elements: 

1.  The drive line has fixed pulse rate\
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

## 3. Firmware Realization

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

### 3.1 PWM

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

### 3.2 Input Capture

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

### 3.3 Typing through CLI

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

Panasonic RK-T55 Typewriter Keyboard Matrix

|**S**	|**0**	|**1**	|**2**	|**3**	|**4**	|**5**	|**6**	|**7**	|**8**	|**9**
| :-:|
|**F0** |5V
|**D1**	|w	|s	|x	|z	|a	|q|		|2	|	|1
|**D2**|t	|f	|v	|c	|d	|r	|e	|5	|3	|4
|**D3**|k	|m	|.	|,	|l	|i	|0	|-	|8	|9
|**D4**|SPELL	|	|	|RET	|	||lb|	=|FILE	|	|
|**F5** |GND
|**D6**|AUTO|EXT	|	|	|BOLD	|UNDER	|	|	BS|	SPACE|
|**D7**|g	|b	|SPACE	|n	|j	|h	|u	|y	|6	|7
|**D8**|'	|DEL |DOWN|UP	|/	|;	|1/2|p	|VTAB	|o
|**F9** |LOCK
|**F10** |SHIFT
|**D11**|LINE SP|KBD|||||PITCH|||MODE
|**F12**|CODE


### 3.4 Production Firmware

In final version, fully customized to the typewriter, one would ditch PWM, and IC, and just trigger TIM14 using the baseline pulses from the typewriter. 
