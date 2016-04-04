/*
  wiring.h - Partial implementation of the Wiring API for the ATmega8.
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2005-2006 David A. Mellis

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA

  $Id: wiring.h 164 2006-08-26 09:56:40Z mellis $
*/

#ifndef Wiring_h
#define Wiring_h

#include <avr/io.h>

#ifdef __cplusplus
extern "C"{
#endif

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1

#define true 0x1
#define false 0x0

#ifndef PI
#define PI 3.14159265
#endif
#define HALF_PI 1.57079
#define TWO_PI 6.283185

#define SERIAL  0x0
#define DISPLAY 0x1

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
//#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

typedef uint8_t boolean;
typedef uint8_t byte;

void pinMode(int, int);
void digitalWrite(int, int);
int digitalRead(int);
int analogRead(int);
void analogWrite(int, int);

void beginSerial(long);
void serialWrite(unsigned char);
int serialAvailable(void);
int serialRead(void);
void printMode(int);
void printByte(unsigned char c);
void printNewline(void);
void printString(char *s);
void printInteger(long n);
void printHex(unsigned long n);
void printOctal(unsigned long n);
void printBinary(unsigned long n);

unsigned long millis(void);
void delay(unsigned long);
void delayMicroseconds(unsigned int us);
unsigned long pulseIn(int pin, int state);

void setup(void);
void loop(void);

// XXX: internal definitions, should be moved to another header file
#define NOT_A_PIN 0
#define NOT_A_PORT -1

#define NOT_ON_TIMER -1
#define TIMER0A 0
#define TIMER0B 1
#define TIMER1A 2
#define TIMER1B 3
#define TIMER2  4
#define TIMER2A 5
#define TIMER2B 6

typedef struct {
	int port;
	int bit;
} pin_t;

extern int port_to_mode[];
extern int port_to_input[];
extern int port_to_output[];
extern pin_t *digital_pin_to_port;
extern pin_t *analog_in_pin_to_port;
extern int *analog_out_pin_to_timer;

#ifdef __cplusplus
} // extern "C"
#endif

#endif
