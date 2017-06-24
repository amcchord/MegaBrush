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

#define rcIN 12
#define ApFET 2
#define AnFET 13 //PWM

#define BpFET 3 //PWM
#define BnFET 7

#define CpFET 4
#define CnFET 5 //PWM


int x = 0;
bool up = true;

// SOFTPWM_DEFINE_CHANNEL(2, DDRD, PORTD, PORTD2);  //Arduino pin 2
// SOFTPWM_DEFINE_CHANNEL_INVERT(3, DDRD, PORTD, PORTD3);  //Arduino pin 3
// SOFTPWM_DEFINE_CHANNEL(4, DDRD, PORTD, PORTD4);  //Arduino pin 4
SOFTPWM_DEFINE_CHANNEL(5, DDRD, PORTD, PORTD5);  //Arduino pin 5
// SOFTPWM_DEFINE_CHANNEL(7, DDRD, PORTD, PORTD7);  //Arduino pin 7
// SOFTPWM_DEFINE_CHANNEL(13, DDRB, PORTB, PORTB5);  //Arduino pin 13
 SOFTPWM_DEFINE_OBJECT_WITH_PWM_LEVELS(6, 250);
 SOFTPWM_DEFINE_EXTERN_OBJECT_WITH_PWM_LEVELS(6, 250);


// the setup function runs once when you press reset or power the board
void setup() {


  Palatis::SoftPWM.begin(400);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(ApFET, OUTPUT);
  pinMode(AnFET, OUTPUT);
  pinMode(BpFET, OUTPUT);
  pinMode(BnFET, OUTPUT);
  pinMode(CpFET, OUTPUT);
  pinMode(CnFET, OUTPUT);

  digitalWrite(ApFET, HIGH); //Shutdown the highside
  digitalWrite(BpFET, HIGH);
  digitalWrite(CpFET, HIGH);

  delay(1000);
  digitalWrite(BpFET,LOW);
  digitalWrite(CnFET, HIGH);

  delay(1000);
  digitalWrite(BpFET,HIGH);
  digitalWrite(CnFET, LOW);

  delay(1000);


}

// the loop function runs over and over again forever
void loop() {

    digitalWrite(BpFET,LOW);
    Palatis::SoftPWM.set(5,x);
    //digitalWrite(CnFET, HIGH);

    if (x > 254 && up == true){
      up = false;
      x--;
    }
    if (x < 1 && up == false){
      up = true;
      x++;
    }

    if (up){
      x++;
    } else {
      x--;
    }

    delay(10);
   // tone(7,400,100);


//  digitalWrite(4, HIGH);  //Cp OFF
//  digitalWrite(7, LOW);   //Bn OFF
//
//
//  digitalWrite(3, LOW);   //Bp ON;
//  digitalWrite(5, HIGH);  //Cn ON;
//
//  delay(250);
//  digitalWrite(4, LOW);  //Cp ON
//  digitalWrite(7, HIGH); //Bn ON
//
//  digitalWrite(3, HIGH); //Bp OFF;
//  digitalWrite(5, LOW); //Cn ON;
                                    // turn the LED off by making the voltage LOW
 // delay(250);                       // wait for a second
}
