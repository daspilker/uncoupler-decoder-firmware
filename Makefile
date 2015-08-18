#
#   Copyright 2012 Daniel A. Spilker
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#

DEVICE     = attiny2313
CLOCK      = 8000000
BAUD       = 9600
FUSE_L     = E4
FUSE_H     = D9
PROGRAMMER = avrisp2
PART       = t2313

SOURCES    = main.c
OBJECTS    = $(SOURCES:.c=.o)

CFLAGS     = -Wall -O3 -mmcu=$(DEVICE) -std=c99
CPPFLAGS   = -DF_CPU=$(CLOCK) -DBAUD=$(BAUD)
CC         = avr-gcc

ifeq ($(OS), Windows_NT)
	SHELL = C:/Windows/System32/cmd.exe
endif

.PHONY: all
all: main.hex

-include $(SOURCES:.c=.d)

.PHONY: flash
flash: main.hex
	avrdude -c $(PROGRAMMER) -p $(PART) -U flash:w:main.hex -v

.PHONY: fuse
fuse:
	avrdude -c $(PROGRAMMER) -p $(PART) -U lfuse:w:$(FUSE_L):m -U hfuse:w:$(FUSE_H):m

.PHONY: clean
clean:
	rm -f main.hex main.elf $(OBJECTS) $(SOURCES:.c=.d)

main.elf: $(OBJECTS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o main.elf $(OBJECTS)

main.hex: main.elf
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex

%.d: %.c
	@set -e; $(CC) -MM $(CPPFLAGS) $< -o $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' $@.$$$$ > $@; \
	rm -f $@.$$$$
