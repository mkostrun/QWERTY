# QWERTY (Query and Whisper to Electronic Remote TYpewriter)

## Executive Summary

All electronic typewriters from the 1980 onwards feature micro-controller unit (MCU) 
controlled keyboard matrix. 
Keyboard matrix is a way of connecting a typewriter keyboard to a MCU, so that MCU can detect which keyboard key or their combination  has been pressed by a human operator. 
Keyboard matrix features, in IBM parlance, drive and sense lines, where a keyboard key electrically connects one drive line to one sense line. 
Different typewriter manufacturers of the era  differ in way how drive lines are used, and how they affect sense lines.

The QWERTY project delivers firmware for STM32F030 (Arm Cortex M0, 8KB SRAM, 64KB FLASH), where the MCU  connects in parallel  to the typewriter keyboard. 
This way of connection allows one to use MCU or the typewriter's own keyboard to control the typewriter. 
Compared to other open source projects on the internet, 
QWERTY requires minimal number of input drive lines from the typewriter: 
1 for Panasonic, 
2 or 3 for IBM Wheelwriter 1000. 
Internally, based on the baseline drive line input the MCU computes output signals that are presented to the sense lines of a typewriter to achieve desired action. 
The MCU itself communicates to host PC through a serial over USB, and so allow QWERTY to act as a quirky(*)  ASCII printer driver. 
The difference is, however, that QWERTY accepts a large number of EPSON Escape Codes, including font pitch  control, and font appearance and justification and so expand on what would be
available were the owner to purchase and install, in IBM parlance, printer option for the typewriter.

While we present solutions that we have developed specifically for two typewriters, 
Panasonic RK T-55 and IBM WheelWriter 1000, the QWERTY firmware has large number of
configuration parameters that allows it with proper wiring to control, we believe, 
any electronic typewriter of the era (Smith Corona, or Olympia, to mention a few).

-----
(*) Quirky here refers to a feature of STM32 USB programming interface stlink: On Linux to reset the MCU and start  executing its firmware, one needs to send BREAK command to the serial port upon opening it (Minicom: ALT+A,Z,F sequence). This allows typewriter to be simultaneously controlled through printer driver, minicom, and typewriter keyboard.

## Technical Abstract

A firmware for keyboard matrix emulator based on STM32F03-Nucleo64 micro-controller platform, is presented. The firmware implements the following functional elements:
1. TTL to 3.3V conversion: All tested typewriters use 5V positive logic. STM32 MCU operates at 3.3V.
    - The typewriter sense lines are driven by STM32 through usage of two output states: Active State at 0V, and Inactive State as Analog Input. This is because in the typewriter, all sense lines have pull-up resistors. 
    - The typewriter drive line(s) feeding into input capture channels on STM32 MCU may be protected with a signal diode (from STM to TY) and a 3.3V pull-up resistor.

1. Typewriter drive signal detection and reconstruction: 1 or 2 Channel Input Capture, through TIM3, which uses interrupt service routine (IRS) to detect the leading and trailing edges of one or two drive lines:
	- PAN/RKT55 uses 1 input capture channel connected to single drive line from the typewriter, which triggers on leading edge interrupts. As the typewriter utilizes fixed sense lines for keys SHIFT, CODE,  and CAPS LOCK, the firmware provides two fixed lines too;
	- IBM/WW1000 uses two input capture channels and trailing edge interrupts. The typewriter utilizes single and two simultaneous pulsed drive lines, where the length of the pulses is altered so to compensate for debouncing of the keys. For that reason the firmware uses as input the drive lines for keys SHIFT and CODE, and from them computes how to drive sense channels for other keys. In adition, for brave in heart, third input channel is available through GPIO/EXTI should one decide to use "PITCH SWITCH" hack.  The hack allows WW 1000 limited control through software of the font pitch, similar to what, e.g., IBM ActionWriter 1, has.

2. Software timer TIM14 which, through interrupts on delay and on period overflow, creates output signals on selected sense pins, and so acts as a GPIO multiplexer. 
3. Command Line Interface (CLI) through Serial Port: the firmware runs in two modes, as CLI, where there is a small set of commands that can be used to probe the typewriter or type a symbol, and as TYPEWRITER, where all received data is typed on the typewriter. The latter recognizes some EPSON compatible escape sequences. The serial port uses interrupts to send messages, and DMA RX to receive messages.


STM32F030 (Arm Cortex M0, 8KB SRAM, 64KB FLASH) is used for realization of the solution, but this is done using generic HAL-infrastructure, so other MC's can be implemented, as well. It uses 4KB DMA RX buffer for serial port input, and 512 symbol output buffer for sending symbols to the typewriter.



## Things to consider before starting this project

To successfully complete this project, one will have to learn a lot about 40-year old electronic typewriters.

Hardware work involves isolating drive and sense connectors, using an oscilloscope to identify drive lines used for input capture, and soldering all wires to the sense connector, and one-two wires to the driver connector. Additional wiring is needed for built-in switches, which also require separation switch.

Software work involves identifying pins attached to drive and to sense and fixed lines, and examining PAN firmware on how to handle fixed lines, and IBM firmware on how to handle simultaneous driving of two sense inputs.

Choosing a typewriter is critical. One should look for those in which the various formating features are available as keyboard strokes. For IBM that is definitely the ActionWriter 1. Wait, this one IBM outsourced to, used to be West, German Adler.
For PAN these are KT-R30 or 55, and perhaps KX-E508. Olympia de Luxe seems to be fine candidate too.
If typewriter has additional built-in switches (e.g., font pitch selector in PAN KX-R typewriters), these complicate the wiring: One first needs to install a separation switch to disconnect the built-in switch(es) from the typewriter, and then bring from MCU a number of fixed lines that control the switch output. E.g., if typewriter has a 1-in-3 selector for 10, 12 and 15pt font pitch, this requires three fixed lines from the MCU connected to font pitch sense lines. 
- What makes things easier is that all built-in switches can be controlled through single separation switch that connects/disconnects them to 0V.
- What makes things harder, is that prior to activation of separation switch, the MCU lines have to be inactive. This suggests that the MCU may have to control the separation switch in non-trivial way: if the MCU is off the built-in switches drive line has to be pulled to 0V, while if the MCU is controlling the typewriter the drive line has to be disconnected or inactive.

Some of the important keyboard-accessible features to consider when choosing a typewriter for conversion, 
are:
- text justification: left, full, center, right margin flush-RMF; 
- font pitch: 10, 12, and 15, and proportional spacing (PS); 
- new keyboard characters '<' , '>', '|', '~'
- typing speed (characters per second, CPS). One should be warned that typing in full justification is painfully slow (input from keyboard is transferred to typewriter internal memory, and from there printed margin to margin, on Panasonic the speed of this is some 4 CPS);
- micro 1- or 4-directional spacing: this can be used for brute-force PITCH SWITCH: IBM WW 1000 has 1-directional micro (back) space equal in size to 1/5 font pitch. With 12pt native printwheel, 15pt font pitch can be simulated through pressing back space after single character, which works OK in regards to typing speed.  To get 10pt, one would press space after each character followed by 4 back spaces. To watch WW 1000 do this drunken dance could be very painful.
- we found no typewriters which printout is bi-directional, irrespectively what their advertisement says;
- availability of replacement parts (printwheels/cupwheels) and consumables (ribbon and correction tape):\
In developing QWERTY,  PAN RK-T55 typewriter was used the most. This exposed its weakness. PAN RK, or T-series uses as a printing element, so called, cupwheel: 
The cupwheel comprise pairs of letters and symbols on plastic bars attached to a round gear. A plastic bar carries an elevated relief of the letter covered with thin metal layer. 
During testing it was noticed that under hits from the typewriter hammer these letters would first decompose (metal separated from plastic resulting is a messy imprint of a symbol or letter) and then fall of (a piece would break off the plastic bar at the place of hammer impact). 
This project consumed a total of three PAN typewriters with cupwheels, which were obtained from e-Bay. The typewriters ended up, or perhaps down, in the land fill somewhere, their cupwheels too.

## Next steps

1. Extending the QWERTY firmware with sub-variants for other electronic typewriters of the era may be interesting. 
Some detriments are their typing speed and keyboard control.
Besides IBM at around 20 CPS, all the other examined typewriters have around 12 CPS speed, at least for consumer level typewriters (Smith Corona, or Olympia).
2. Replacing soldering of sense wires with ribbon cable connector might be interesting to streamline conversion.
3. Simulating a human operator of a typewriter: 
    - Through QWERTY, 'DELETE' key is accessible with all arrow keys. If the typewriter has correction tape installed, this deletes the typed character under the hammer. To prove that a certain document was written on a typewriter by a human, a random number of errors and their corrections could be inserted in the document as it is being printed. Actions that can be simulated are deleting a character and correcting it, or deleting an entire word then typing a corrected word in the available space using micro-spacing. Interestingly, the typewriters do not allow crossing out a mistyped word by x's. I didn't have time to figure out how to override.
    - Time delays between keys can be inserted, so to mimic a human operator of various typing skills sitting behind a closed door.

## References

There is considerable amount of work posted on how to connect the electronic typewriters from the period 1980-90s to modern computers.
Most of the work aims at reverse engineering the communication protocol of various serial-look alike protocols using inexpensive micro-controller prototyping boards.
We do not recommend this approach as through such interface one has limited access to the typewriter's features.
Two of the projects listed below, however, recognized the presence of keyboard matrix inside the typewriter, and provided rudimentary signal plots of various drive lines:
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
9. Useful is service manual of another Panasonic thermal typewriter, which gives an idea of how many and what kind of signals to expect between drive and sense channels in their keyboard matrix.