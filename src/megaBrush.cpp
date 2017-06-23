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

#define rcIN 12
#define ApFET 2
#define AnFET 13 //PWM

#define BpFET 3 //PWM
#define BnFET 7

#define CpFET 4
#define CnFET 5 //PWM




// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(ApFET, OUTPUT);
  pinMode(AnFET, OUTPUT);
  pinMode(BpFET, OUTPUT);
  pinMode(BnFET, OUTPUT);
  pinMode(CpFET, OUTPUT);
  pinMode(CnFET, OUTPUT);

  digitalWrite(ApFET, HIGH);
  digitalWrite(BpFET, HIGH);
  digitalWrite(CpFET, HIGH);

  delay(4000);


}

// the loop function runs over and over again forever
void loop() {
    digitalWrite(4,HIGH);
    digitalWrite(7,LOW);

    delay(1);

    digitalWrite(4,LOW);
    digitalWrite(7,HIGH);

    delay(1);
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
