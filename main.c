/*
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
*/

#include "stdbool.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

#define PIN_LED PB3
#define PIN_OUT PB4

#define LED_ON()  PORTB |=  _BV(PIN_LED)
#define LED_OFF() PORTB &= ~_BV(PIN_LED)
#define OUT_ON()  PORTB |=  _BV(PIN_OUT)
#define OUT_OFF() PORTB &= ~_BV(PIN_OUT)

volatile uint8_t count;

volatile uint8_t address;
volatile uint8_t function;
volatile uint8_t data;
volatile uint8_t address2;
volatile uint8_t function2;
volatile uint8_t data2;

volatile bool data_ready;

static void clear() {
  count = 0;
  address = 0;
  function = 0;
  data = 0;
}

static void process(uint8_t bit) {
  count++;
  if (count < 9) {
    address <<= 1;
    address |= bit;
  } else if (count < 11) {
    function <<= 1;
    function |= bit;
  } else {
    data <<= 1;
    data |= bit;
    if (count == 18) {
      address2 = address;
      function2 = function;
      data2 = data;
      count = 0;
      address = 0;
      function = 0;
      data = 0;
      data_ready = true;
    }
  }
}

static void activate() {
  OUT_ON();
  LED_ON();

  TCNT1H = 0x00;
  TCNT1L = 0x00;
  TCCR1B |= _BV(CS11) | _BV(CS10);
  TIMSK |= _BV(TOIE1);
}

static void deactivate() {
  OUT_OFF();
  LED_OFF();

  TIMSK &= ~_BV(TOIE1);
  TCCR1B &= ~_BV(CS11) & ~_BV(CS10);
}

ISR(INT0_vect) {
  uint8_t timer = TCNT0;
  TCNT0 = 0x01;
  if (bit_is_clear(PIND, PD2)) {
    if (timer < 40) {
      process(0);
    } else if (timer > 70 && timer < 130) {
      process(1);
    }
  }
}

ISR(TIMER0_OVF_vect) {
  clear();
}

ISR(TIMER1_OVF_vect) {
  deactivate();
}

static void read_address(uint8_t* address, uint8_t* data) {
  uint8_t address_in = (~PIND & 0x03) | ((~PIND >> 1) & 0x3C);
  uint8_t address_result = 0;
  uint8_t trit = 0;
  for (uint8_t i = 0; i < 4; i++) {
    address_result <<= 2;
    trit = address_in % 3;
    if (trit == 0x01) {
      address_result += 0x03;
    } else if (trit == 0x02) {
      address_result += 0x02;
    }
    address_in /= 3;
  }
  *address = address_result;

  uint8_t data_in = ~PINB;
  uint8_t data_result = 0;
  uint8_t bit = 0;
  for (uint8_t i = 0; i < 3; i++) {
    bit = (data_in & 0x01) << 7;
    data_result |= bit;
    data_result >>= 1; 
    data_result |= bit;
    data_result >>= 1; 
    data_in >>= 1;
  }
  *data = data_result << 1;
}

int main(void) {
  uint8_t last_address = 0;
  uint8_t last_function = 0;
  uint8_t last_data = 0;
  bool data_valid = false;

  uint8_t my_address = 0;
  uint8_t my_data = 0;

  DDRB |= _BV(PIN_OUT) | _BV(PIN_LED);
  PORTB |= _BV(PB0) | _BV(PB1) | _BV(PB2);
  PORTD |= _BV(PD0) | _BV(PD1) | _BV(PD3) | _BV(PD4) | _BV(PD5) | _BV(PD6);

  for (uint8_t i=0; i<5; i++) {
    LED_ON();
    _delay_ms(50);
    LED_OFF();
    _delay_ms(50);
  }

  read_address(&my_address, &my_data);
  my_data |= 0x03;

  MCUCR |= _BV(ISC00);
  GIMSK |= _BV(INT0);

  TCCR0B |= _BV(CS01);
  TIMSK |= _BV(TOIE0);

  sei();

  for (;;) {
    if (data_ready) {
      data_ready = false;
      if (last_address == address2 && last_function == function2 && last_data == data2) {
	data_valid = true;
      } else {
	last_address = address2;
	last_function = function2;
	last_data = data2;
      }
    }
    if (data_valid) {
      if (last_address == my_address && last_function == 0x00) {
	if (last_data == my_data) {
	  activate();
	} else if ((last_data & 0x03) == 0x00) {
	  deactivate();
	}
      }
      data_valid = false;
    }
  }

  return 0;
}
