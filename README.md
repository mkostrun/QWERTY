# QWERTY (**Q**uery and **W**hisper to **E**lectronic **R**emote **TY**pewriter)

## Executive Summary

Given their place in development between electric typewriters and first generation of IBM personal computers (PC), all electronic typewriters from the 1980-90s feature micro-controller (MC) controlled keyboard matrix.

Recognizing commonalities in all MC controlled keyboard matrix, a generic emulator of typewriter keyboard matrix is proposed. The emulator purpose is to allow control of the typewriter through USB/serial interface of a PC to which the emulator is connected, where the level of control of the typewriter that is achieved mimics human operator.

In the following, we use IBM nomenclature.
As is known, in an electronic typewriter, the keyboard matrix consists of two sets of connections to the typewriter motherboard: drive pins and sense pins, where a key on the keyboard makes steady electrical connection between drive and sense pins.\
The novelty of presented emulator is that it uses only one or two drive signals from the entire drive signal set, to generate internally any of the other drive pulses. These internal drive pulses are then played on desired sense lines, to create an electric equivalent of pressing single key on a keyboard. \
Two ways of pressing simultaneously two keys (SHIFT + symbol, or CODE + symbol) found in the typewriters of the era are considered:
1. Fixed (Sense) Lines, e.g., Panasonic RK-T55, for which pressing the SHIFT or the CODE key is electrically equivalent to connecting a special sense line to the ground; and,
2. Multi Sense Lines, e.g., IBM WheelWriter, where the SHIFT and the CODE key have each its particular drive pulse shape played on a sense line, which need be combined with a drive/sense values corresponding to a key.

The emulator is connected parallel to the keyboard, so the keyboard maintains its function.
Besides emulating the keyboard, the solution allows one to explore the typewriter matrix and find values of the delays for drive pins. 
This is done through built-in command line interface over serial port.

## Technical Abstract
The project addresses keyboard matrix emulation. 
A solution is presented based on STM32F03-Nucleo64 micro-controller platform, which implements the following functional elements:
1. Typewriter drive signal detection and reconstruction: 1 or 2 Channel Input Capture, through TIM3, which uses interrupt routine to detect the leading and trailing edges of one or two drive pulses from the typewriter keyboard matrix drive signal set, and fires TIM14 with a delay (CCR) and for a period (ARR):
	- PAN/RKT55 uses 1 input capture channel and leading edge interrupts, because the pulse durations are not affected by operation;
	- IBM/WW1000 uses two input capture channels and trailing edge interrupts, as the pulse durations are altered to compensate for debouncing of the keys.

	The channels for input capture may be protected with a signal diode and a resistor from overvoltage.

2. Software timer TIM14 which, through interrupts on delay and on period overflow, creates output signals on selected sense pins by altering inactive and active output states. Here, a pin is inactive if it is configured as an analog input, while it is active when it is set to digital 0 output;
3. Command Line Interface (CLI) through Serial Port: the firmware runs in two modes, as CLI, where there is a small set of commands that can be used to probe the typewriter or type a symbol, and as TYPEWRITER, where all received data is typed on the typewriter. The latter recognizes some EPSON compatible escape sequences. The serial port uses interrupts to send messages, and DMA RX to receive messages.

STM32F030 (Arm Cortex M0, 8KB SRAM, 64KB FLASH) is used for realization of the solution, but this is done using generic HAL-infrastructure, so other MC's can be implemented, as well. It uses 4KB DMA RX buffer for serial port input, and 512 symbol output buffer for sending symbols to a typewriter.



## Things to consider

To successfully complete this project, one will have to learn a lot about 40-year old electronic typewriters.

Through synchronicity, this project was initially organized around a Panasonic RK-T55 typewriter, which is similar to other typewriters from RK, or T-series in that the printing element is the, so called, cupwheel:\
The cupwheel comprise pairs of letters and symbols on plastic bars attached to a gear. A plastic bar carries an elevated relief of the letter covered with thin metal layer. 
While developing the MC firmware, it was discovered that under hits from the typewriter hammer these letters would first decompose (metal separated from plastic resulting is a messy imprint of a symbol or letter) and then fall of (a piece would break off the plastic bar at the place of hammer impact). 

Secondly, if one wants to convert a typewriter to a printer, the questions to ask is, what are the features the typewriter should have to be "worthy" of conversion. 
Some of the features that can be considered are 
- if the text justification is supported (left, full, center, right margin flush-RMF); 
- font pitch, 10, 12, 15, and PS; 
- line spacing (1, 1-1/2, 2); 
- micro 4-directional spacing;
- support for additional keyboard characters '<' , '>', '|', '~'
- typing speed (characters per second, CPS), and whether the printout is bi-directional. One should be warned that typing in full justification is extremely slow (keyboard input is transferred to internal typewriter memory, and from there printed margin to margin, on Panasonic the speed of this is some 4 CPS).
- most importantly, are the above features available through standard keyboard keys (e.g., MODE+'c' for 'center', e.g., IBM WW1000) or through additional fixed sense lines (e.g., Panasonic KX typewriters)

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
5. [Cadetwriter](https://github.com/IBM-1620/Cadetwriter) in their presentation discuss signal shapes that IBM is using to communicate drive to sense line. The IBM/WW1000 QWERTY firmware uses different timings from what they claim: In fact, using their timings (see durations of ARR-CCR in our firmware, in main.c, which according to Cadetwriter's authors should be some 3.8ms) results in typing repeated symbols. Issue of timings is averted altogether in QWERTY firmware by firing Drive line timings off the trailing pulse edge (this is particularly relevant for simultaneous typing of two keys: e.g., SHIFT + symbol).
6. US PTO 4,888,600, IBM, "Keyboard Arrangement with Ghost Key Condition Detection. "
7. Mayo R.D., "System Control for a printwheel typewriter," IBM J. Res. Devel. Vol 29, No. 5,  1985.
8. IBM, "Wheelwriter Service Manual," 1995.
9. Useful is service manual of another Panasonic thermal typewriter, which gives an idea of how many and what kind of signals to expect between drive and sense channels in their **keyboard matrix.**

