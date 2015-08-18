Uncoupler Decoder
=================

This repository contains the source code of the firmware for an decoder for the Märklin Digital system. The decoder has only one output and is well suited for uncoupler tracks.

The Atmel AVR Toolchain for [Windows][1] or [Linux][2] is needed for compiling and [AVRDUDE][3] for flashing.


Setting the Decoder Address
---------------------------

A DIP switch with 9 positions must be used to set the decoder address and select the output.

The firmware expects the DIP switch to be connected with the following mapping:

    1 <-> PD0
    2 <-> PD1
    3 <-> PD3
    4 <-> PD4
    5 <-> PD5
    6 <-> PD6
    7 <-> PB0
    8 <-> PB1
    9 <-> PB2

The first 8 positions are used to set the decoder address in binary format, which allows 255 different addresses (1 to 255). Position 1 sets the least significant and position 8 the most significant bit. Since each address has two outputs (red/turn and green/straight), position 9 is used to select the output, ON for red (or turn) and OFF for green (or straight).

To set the address to 11 (1011 in binary) and the output to red/turn, the following switches must be set:

    1 - ON
    2 - ON
    3 - OFF
    4 - ON
    5 - OFF
    6 - OFF
    7 - OFF
    8 - OFF
    9 - ON

The firmware reads the address only after reset or power on. To set a new address, power must be switched off and on again or the decoder must be disconnected and then reconnected.


Schematics and PCB Layout
-------------------------

Schematics and a PCB layout for the uncoupler decoder can be found in other [repository][4].


License
-------

Copyright 2012 Daniel A. Spilker

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.


[1]: http://www.atmel.com/tools/atmelavrtoolchainforwindows.aspx
[2]: http://www.atmel.com/tools/atmelavrtoolchainforlinux.aspx
[3]: http://www.nongnu.org/avrdude/
[4]: https://github.com/daspilker/uncoupler-decoder-schematics
