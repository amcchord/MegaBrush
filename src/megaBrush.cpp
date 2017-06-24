/*MegaBush... A Brushed motor controller firmware for BLHELI compatible ESCs

Copyright 2017 Austin McChord

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#include <Arduino.h>


#include <SoftPWM.h>

/* pins_arduino.h defines the pin-port/bit mapping as PROGMEM so
   you have to read them with pgm_read_xxx(). That's generally okay
   for ordinary use, but really bad when you're writing super fast
   codes because the compiler doesn't treat them as constants and
   cannot optimize them away with sbi/cbi instructions.

   Therefore we have to tell the compiler the PORT and BIT here.
   Hope someday we can find a way to workaround this.

   Check the manual of your MCU for port/bit mapping.

   The following example demonstrates setting channels for all pins
   on the ATmega328P or ATmega168 used on Arduino Uno, Pro Mini,
   Nano and other boards. */

#define rcIN 8
#define ApFET 2
#define AnFET 13 //PWM

#define BpFET 3 //PWM
#define BnFET 7

#define CpFET 4
#define CnFET 5 //PWM

#define PPM_MAX_LOC = 128;
#define PPM_MIN_LOC = 64;

SOFTPWM_DEFINE_CHANNEL_INVERT(1, DDRD, PORTD, PORTD2);  //Arduino pin 2 ApFET
SOFTPWM_DEFINE_CHANNEL_INVERT(2, DDRD, PORTD, PORTD3);  //Arduino pin 3 BpFET
SOFTPWM_DEFINE_CHANNEL_INVERT(3, DDRD, PORTD, PORTD4);  //Arduino pin 4 CpFET

SOFTPWM_DEFINE_CHANNEL(4, DDRD, PORTD, PORTD5);  //Arduino pin 5   CnFET
SOFTPWM_DEFINE_CHANNEL(5, DDRD, PORTD, PORTD7);  //Arduino pin 7   BnFET
SOFTPWM_DEFINE_CHANNEL(6, DDRB, PORTB, PORTB1);  //Arduino pin 13  AnFET

SOFTPWM_DEFINE_OBJECT_WITH_PWM_LEVELS(7, 250);
SOFTPWM_DEFINE_EXTERN_OBJECT_WITH_PWM_LEVELS(7, 250);

int finalSpeed = 0; //-250 - 0 - 250
long pulse_time = 0;


//This function sets the actual speed to the motor
void applySpeed(int speed){
  if (speed > 250){
    speed = 250;
  }
  else if (speed < -250){
    speed = -250;
  }

  if (speed > 0){
    Palatis::SoftPWM.set(2, 0); //ApFET //Chop the high side
    Palatis::SoftPWM.set(6, 0); //Stop at AnFET
    Palatis::SoftPWM.set(1,speed); //ApFET //Chop the high side
    Palatis::SoftPWM.set(5,250); //BnFET
  }
  else if (speed < 0){
    Palatis::SoftPWM.set(1, 0); //Stop th ApFET
    Palatis::SoftPWM.set(5, 0); //Stop at BnFET
    Palatis::SoftPWM.set(2,speed * -1); //BpFET //Chop the high side
    Palatis::SoftPWM.set(6,250); //AnFET
  }
  else {
    Palatis::SoftPWM.set(1, 0); //Stop th ApFET
    Palatis::SoftPWM.set(5, 255); //Stop at BnFET
    Palatis::SoftPWM.set(2, 0); //BpFET //Chop the high side
    Palatis::SoftPWM.set(6, 255); //AnFET
  }

}


// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(rcIN, INPUT);
  Palatis::SoftPWM.begin(600);
  Palatis::SoftPWM.allOff();
  delay(2000);

  applySpeed(0);

}

int lastPulse = 100;
long lastUpdate = 0;
// the loop function runs over and over again forever
void loop() {
    pulse_time = pulseIn(rcIN, HIGH, 25000);
    if (pulse_time != lastPulse){
      lastPulse = pulse_time;
      lastUpdate = millis();
    }

    if (millis() - lastUpdate > 2000 || pulse_time == 0){
      applySpeed(0);
    }
    else {
      finalSpeed = map(pulse_time, 125, 300, -250, 250);
      applySpeed(finalSpeed);
    }

  delay(10);
}
