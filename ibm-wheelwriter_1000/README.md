# QWERTY (**Q**uery and **W**hisper to **E**lectronic **R**emote **TY**pewriter)

![DEMO](/main/ibm-wheelwriter_1000/art/2026-07-17-IBM-WW1000-Demo-All_Features-scanned_copy.png)

# Flavor: IBM WheelWriter 1000 Typewriter

## Executive Summary
Nucleo 64 - STM32F030 (Arm Cortex M0, 8KB SRAM, 64KB FLASH) 
development board is used for emulation of keyboard matrix in a IBM WheelWriter (WW) 1000 typewriter.
The PC host controls the typewriter through serial over USB, where it can be used either as a standard ASCII printer, or through a terminal program such as minicom it can be used directly as a typewriter.
Additionally, a hardware hack "Pitch Switch" is proposed which allows access to 10p, 12p, 15p and PS font pitches using single
typewriter font wheel (preferably 10p) like in only other IBM typewriter ActionWriter 1.

## Abstract

The project provides firmware for Nucleo 64 - STM32F030, which with proper wiring to the typewriter sense and drive lines, 
allows it to control the typewriter IBM WW 1000 keyboard matrix.
In addition, additional PITCH SWITCH  wiring modification is presented, which allows the firmware to partially control the font pitch of
the typewriter (10p, 12p, 15p and PS).

While the QWERTY project addresses how to emulate a keyboard matrix in general terms, this flavor addresses relevant details of implementation:

0. TTL to 3.3V conversion: All tested typewriters use 5V positive logic. STM32 MCU operates at 3.3V.
    - The typewriter sense lines are driven by STM32 through usage of two output states: Active State at 0V, and Inactive State as Analog Input. This is because in the typewriter, all sense lines have pull-up resistors. 
    - The typewriter drive line(s) feeding into input capture channels on STM32 MCU may be protected with a signal diode (from STM to TY) and a 3.3V pull-up resistor.
1.  (S)ense lines are 8:  S0:7. This are directly wired to MCU GPIO MUX pins.  
2.  (D)rive lines D1:14 are numbered by location/relative delay when in-active: D1/3, D2/d, D3/0, D4/2, D5/5, D6/6, D7/7, D8/8, D9/9, Da/10,
     Db/11. Dc/12, Dd/1, De/4. 
     Two D-lines, D3/0 (L/R SHIFT) and De/4 (CODE) are used as signal inputs (to MCU for computing signals to be passed to S-lines.
3. The period of drive pulse bursts is $tper = 11.48$ ms (87 Hz), while a pulse length is $tpul = 820$ us (1.2 kHz). 
4. Unlike PAN, IBM typewriter logic upon pressing a key modifies the driving pulse train: the first pulse is extended in lenght, and there is one more, equally long, pulse after the key is depressed, the mid-pulses in the train are un-changed in duration. This change of pulse length becomes important if one is to type two-key combination (one being SHIFT for capital letters, the other being CODE for special commands). 
The firmware resolves this as follows: If single key is pressed, then the timing of a pulse sequence is computed from the closer of CODE and SHIFT drive lines - to type single key it suffices to send two extended drive pulses to the required sense line; If key is wanted with SHIFT xor CODE modifiers, then firmware first presses the respective modifier key by passing-through the drive pulse to their sense line, and then on the second pulse from modifier signal starts sending drive pulses to the second sense lines.
The other drive pulse lengths are computed from the minimum duration of the extended pulse (shorter than what CADETWRITER project stated),
and there are two of them and not more.
5. To get a key registered by the typewriter, the minimum number of pulses is 2 periods. Between two consecutive keys there has to be 2 period blank. This is consistent with the $87/4 \sim 20$ characters per second (CPS) speed of the typewriter.
6. Changing pitch of the font from the printwheel was explored in PITCH SWITCH-subproject.

Firmware through various MACROS allow configuration of delays, and sense and fixed lines. Firmware requires D3/0 and De/4 to be two specific pins (TIM3/CH1 and CH2 Input Capture, with pulse edge generated interrupts for measuring the pulse duration, and period, and reporting it to the user when firmware is operating in CLI mode).

## Pitch Switch

For this project we looked into possibility of changing the pitch of the font without changing the printer wheel. The feature exists in PAN RK-T55 through keyboard, and an IBM typewriter ActionWriter 1 also supports it.

Basically, for a single pitch font size printer wheel (say 12), by changing the amount of spacing between typed symbols one can simulate pitches 10, 12, and 15. With QWERTY firmware, there are two ways of doing it:

1. BRUTE FORCE

   IBM WW 1000 keyboard features a key *Bksp 1* (CODE + Back Space), which returns the printer carriage by 1/5 of the space spacing. If the native font pitch is 12p, then by pressing *Bksp 1* after every key, one achives $12 + 12/5 = 14.2 \approx 15$  character placements per 2.54cm. Easy, right? One only suffers drop in CPS from 20 to 10 (comparable to PAN and others of 12 CPS nominal).

   To get 10p through this method one would need micro-space, but the IBM keyboard does not provide it. Hmm, I wonder why.

   One method for producing +1/5 space is to send SPACE follower by four *Bksp 1* keys after each character. While that achieves the goal of typing $12 - 12/5 = 9.8 \approx 10$ characters per 2.54cm.
Watching typewriter go through these steps to print with this pitch is painful.

   Also, it seems like it would void the warranty and shorten the life expectancy of the printer head.

   For those reasons, a better solution is suggested ....

2. HAX

   There exist ribbon cable that connects printer head to the typewriter logic. Line/Pin 20 on that cable carries one-way signal on a pull-up line from the head to the logic. PITCH SWITCH intercepts that signal, and by default provides pass-through feature: When the firmware is running, it transfers the state changes from the head to the logic.
   
   PITCH SWITCH function is realized when, upon the input from computer, the firmware replays an electric signal that simulates replacing the print wheel and its subsequent recalibration as 10p, 12p, 15p, or PS pitch printer wheel. It works mostly OK. Sometimes it fails spectacularly in that typewriter loses a feeling which key is being typed. The failures exclusively happen when pitch is changed, and then centering of the text or right-flush-margin (RFM) is attempted.
   
   For the purpose of this project, the holes on the printwheel were masked, and typewriter induced to recalibrate the printwheel. The signal was read from Line/Pin 20 and recorded for each of the simulated pitches (Did you know if you cover appropriate hole your printerwheel, the logic will think it has that pitch font on it?). This was the second thing we tried. The first was that it was noticed there exist commands that cause the logic to spin the printerwheel in particular way, where it was easy to identify from which hole particular pulse came. This turned out not to work. The only way to force the typewriter to consider switching the font pitch is to stimulate the logic through Line/Pin 20 by two long pulses, which correspond to a person taking out the printwheel and replacing it by another, followed by calibration the printer head performs after the printwheel change. This procedure at the shortest takes some 2 seconds.

   In the folder **ai** is the conversation with Claude on feasibility of doing it prior to me trying to do it. Talking about **botshit**, I mean, *hallucinations* !

   In theory this could be improved: one would first replay the messages that induce typewriter to think it has new printwheel inserted with target pitch. Secondly, when the typewriter goes into *i-am-lost* frenzy and starts to spin the printwheel like a maniac, one would need to detect the direction of spin, and then blank the bits in it that correspond to the appropriate holes on the printwheel.

   For this project we found what works in most of the cases is that replaying messages works for switching pitch, followed by subsequent pass-through behavior (even though pass-through reveals that the printwheel is 12p, and not something else).

## Use Cases

Besides haxing challange, it was difficult to find use cases for typewriter. We found the following uses that still remain :
1. Type notices for kids for school: The school must be freaking out when they receive typewritten notices instead of printed, or called in.
2. Fill the MASS State Tax Forms: For to me unknown reason the MASS DOR refused to make their PDF tax forms fillable, and one had to either fill them by hand, or by using applications such as Xournal++. 
3. Write real mails to people where they may think they are special when they recognize the mail was typed on a typewriter rather than printed or mass e-mailed. Little do they know.
4. For typing invitations, filling application forms and addresses on envelopes, jotting notes on margins, and such on various documents.

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

Based on existing body of contemporaneous knowledge and visual and electrical inspection of the typewriter, it was found that the keyboard matrix is executed through 14 drive lines D1:14, and 8 sense lines S0:7. 
Notice that we count drive lines by delay and not by the cable pin at which they appear. Remaining 7 sense lines S8:14 consist of six lines that the typewriter logic (in further text just _the logic_) uses to identify the keyboard connected to it, and GND. On the web one cal locate help pages on dealing with misbehaving keyboard by short circuiting sense lines 8:14 as for US keyboard.

A keyboard key connects one drive line to one sense line. Unlike other typewriters, and as discussed in IBM patents on the topic from the era, upon pressing a key on the keyboard the first drive pulse extends. Similarly, upon releasing that key, the logic makes drive pulse put out one more pulse of the same length as the first. It is not difficult to see that with this pulse management, there is no fixed drive pulse repetition rate.
For that reason, the QWERTY firmware uses trailing edge of the drive pulses to simulate drive pulses that are fed to the sense lines. In the case of PAN typewriter, which had fixed pulse width and phase irrespectively whether a key was pressed or not, the leading edge of drive pulses was used for the same purpose.

In IBM logic, there is only a single type of sense line. The challange this presented is how to send two pulses simultaneosly to the typewriter logic sense lines.
The QWERTY firmware solves that as follows. The delay associated with SHIFT is chosen as a reference and so sits on D3/0 (Pin 3/Delay 0) drive line and CODE on De/4 (Pin 14/Delay 4). The timing difference between these two of 4 pulse widths allows all drive pulses with delays to be created: Limitations of input capture is that the stretched pulse created from line D0 has to end before D0 repeats itself. 

For the drive lines to be read with external analyzer or oscilloscope, it need be pulled up to 5V.

The drive lines were each examined with the oscilloscope.

It was confirmed that overall, the typewriter electronics operate in "inverted" mode (HI = 0, LO = 5V).
More importantly, the electronics of the typewriter identifies each drive line through two elements: 

1.  The drive line has fixed pulse rate
The baseline pulse rate was measured to be around *f* = 87 Hz (period *tp* = 11.48 ms). Each baseline pulse has duration *td* = 884 us (*fd* = 1.2 kHz).

2. The drive line has fixed delay with respect to Baseline Pulse:\
There exists two drive lines which are in-sync with, what we call, the two baseline pulses: SHIFT and CODE. 
All the other drive lines  have fixed delay from the baseline pulse, but the same rate.

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

**Important** : For the pin that is triggered by the typewriter D4/0 and De/4 lines, because of voltage difference MCU 3.3V vs typewriter 5V there is a diode (cathode facing S5) and pull-up resistor to 3.3V on MCU side that separate the two.\
For the pins that communicate to the typewriter (sense lines D/F), their states are "analog input" (inactive state) or digital output 0 (active state).\
ONLY IN THIS CONFIGURATION CAN THE TYPEWRITER KEYBOARD AND MCU KEYBOARD EMULATOR CO-EXIST. IF ONE INSISTS MCU PINS TO BE DIGITAL OUTPUT IN ACTIVE AND IN INACTIVE STATE THEN EITHER THE TYPEWRITER KEYBOARD IS INOPERATIONAL OR THE SIGNAL DIODES NEED BE INSERTED IN SENSE/FIXED LINES.
PITCH SWITCH: after Line/Pin 20 from the ribbon cable is intercepted by the MCU, the end facing the typewriter logic has to be pulled up, otherwise the logic will declare system error and stop working. The signal entering the MCU (Line/Pin 20 from the printer head) may require diode/pull-up to 3.3V for electrical safety described earlier..

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

Fragments of IBM WW1000 Typewriter Keyboard Matrix. It is slightly different from other IBM keyboards in non-printable keys. E.g., WW 1000 does not support Home, End, or Justify Features.

| **S/D** | **D1/3** | **D2/13** | **D3/0** | **D4/2** | **D5/5** | **D6/6** | **D7/7** | **D8/8** | **D9/9** | **Da/10** | **Db/11** | **Dc/12** | **Dd/1** | **De/4** |
|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|:-----:|
| **S0** | Tab CLR | | | | | | g| h| | | ' | ENTER | DOWN |  |
| **S1** |  | Tab SET |  | a | s | d | f| j | k| l | ; | Find | Reloc |  |
| **S2** |  | Mar Rel |  | $+/-$ |  |  | 5 | 6 | = |  | BS |  | Ppr Up |  |
| **S3** |  |  | . | 1 | 2 | 3 | 4 | 7 | 8 | 9 | 0 |  |  |  |
| **S4** | Left Mar | TAB| | q | w| e| r | u| i | o | p |  | Ppr Down |  |
| **S5** |  | R Mar| | | | | t| y| [| | $\frac14$ |  |  |  |
| **S6** |  | LOCK | R Shift| z| x | c | v| m | ,| . | $()^2$ | UP |  |  |
| **S7** | SPACE | DEL | L Shift | $\S$ |  |  | b | n | 6 | 7 | / | LEFT | RIGHT | CODE |
