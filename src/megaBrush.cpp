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
#include <EEPROM.h>
#include <SoftPWM.h>
#include <avr/interrupt.h>



//Settings for KingKong 12a (Also works for HobbyKing 30A and )
#define rcIN 2
#define hasLED false
#define redLED 0
#define greenLED 0
SOFTPWM_DEFINE_CHANNEL_INVERT(0, DDRD, PORTD, PORTD4);  //Arduino pin 2 ApFET
SOFTPWM_DEFINE_CHANNEL_INVERT(1, DDRC, PORTC, PORTC3);  //Arduino pin 4 CpFET
SOFTPWM_DEFINE_CHANNEL(2, DDRB, PORTB, PORTB0);  //Arduino pin 5   CnFET
SOFTPWM_DEFINE_CHANNEL(3, DDRD, PORTD, PORTD5);  //Arduino pin 13  AnFET

//Settings for Afro ESC
// #define rcIN 8  // PORTB0
// #define hasLED true
// #define redLED 16 // PORTC3
// #define greenLED 17 // PORTC2
// SOFTPWM_DEFINE_CHANNEL_INVERT(0, DDRD, PORTD, PORTD2);  //Arduino pin 2 ApFET
// SOFTPWM_DEFINE_CHANNEL_INVERT(1, DDRB, PORTB, PORTB1);  //Arduino pin 4 CpFET
// SOFTPWM_DEFINE_CHANNEL(2, DDRD, PORTD, PORTD5);  //Arduino pin 5   CnFET
// SOFTPWM_DEFINE_CHANNEL(3, DDRD, PORTD, PORTD3);  //Arduino pin 13  AnFET


#define PPM_MAX_LOC  32
#define PPM_MIN_LOC  48
#define PPM_NEVER_SET 40
#define PWM_LEVELS 127
#define PWM_HZ 1000
#define SERIAL_DEBUG false

#define RC_MIN_DEFAULT 445
#define RC_MAX_DEFAULT 800

#define DEADBAND 10
#define TIMEOUT 2000



SOFTPWM_DEFINE_OBJECT_WITH_PWM_LEVELS(4, PWM_LEVELS - 1);
SOFTPWM_DEFINE_EXTERN_OBJECT_WITH_PWM_LEVELS(4, PWM_LEVELS - 1);

int finalSpeed = 0; //-250 - 0 - 250
long pulse_time = 0;

int lastPulse = 100;
long lastUpdate = 0;

int rcMin = RC_MIN_DEFAULT;
int rcMax = RC_MAX_DEFAULT;

int smoothSpeed1 = 0;
int smoothSpeed2 = 0;
int smoothSpeed3 = 0;

int printUpdate = 1;

bool waitingForDeadBand = 1;


//This function sets the actual speed to the motor
void applySpeed(int speed){
  if (speed > PWM_LEVELS){
    speed = PWM_LEVELS;
  }
  else if (speed < PWM_LEVELS * -1){
    speed = PWM_LEVELS * -1;
  }
  if (speed < DEADBAND && speed > DEADBAND * -1){
    speed = 0;
  }

  if (speed > 0){
    speed = map(speed, DEADBAND, PWM_LEVELS, 0, PWM_LEVELS);
    Palatis::SoftPWM.set(0, speed); //CLOSE ApFET //Chop the high side
    Palatis::SoftPWM.set(1, 0); //OPEN CpFET
    Palatis::SoftPWM.set(2, PWM_LEVELS); //CLOSE CnFET
    Palatis::SoftPWM.set(3, 0); //OPEN  AnFET
  }
  else if (speed < 0){
    speed = map(speed * -1, DEADBAND, PWM_LEVELS, 0, PWM_LEVELS) * -1;
    Palatis::SoftPWM.set(0, 0); //OPEN th ApFET
    Palatis::SoftPWM.set(1, speed * -1); // CLOSECpFET //Chop the high side
    Palatis::SoftPWM.set(2, 0); //OPEN at CnFET
    Palatis::SoftPWM.set(3, PWM_LEVELS); //CLOSE AnFET
  }
  else {
    //Lets disconnect the motor
    Palatis::SoftPWM.set(0, 0); //OPEN ApFET
    Palatis::SoftPWM.set(1, 0); //OPEN CpFET
    Palatis::SoftPWM.set(2, 0); //OPEN CnFET
    Palatis::SoftPWM.set(3, 0); //OPEN AnFET
  }

}

void doBeep(int cMax){
  for (int cnt = 0; cnt < cMax; cnt++){
    applySpeed((PWM_LEVELS/4)*3);
    delayMicroseconds(250);
    applySpeed((PWM_LEVELS/4)*3 * -1);
    delayMicroseconds(250);
  }
  applySpeed(0);
  delay(100);
}

void EEPROMWritelong(int address, long value){
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

long EEPROMReadlong(long address) {
  //Read the 4 bytes from the eeprom memory.
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

bool programMinMax(){
  if (SERIAL_DEBUG){Serial.println("Programming Min Max");}
  doBeep(100);
  doBeep(100);
  doBeep(100);
  long max = 0;
  long min = 10000;
  lastUpdate = millis();
  while(millis() - lastUpdate < TIMEOUT){
    pulse_time = pulseIn(rcIN, HIGH, 25000);
    if (pulse_time > max){
      max = pulse_time;
      lastUpdate = millis();
    }
  }
  if (SERIAL_DEBUG){
    Serial.print("Max Set: ");
    Serial.println(max);
  }
  doBeep(200);
  lastUpdate = millis();
  while(millis() - lastUpdate < TIMEOUT){
    pulse_time = pulseIn(rcIN, HIGH, 25000);
    if (pulse_time < min){
      min = pulse_time;
      lastUpdate = millis();
    }
  }
  if (SERIAL_DEBUG){
    Serial.print("Min Set: ");
    Serial.println(min);
  }


  if (max - min < 20){
    //Not enough different between max and min;
    if (SERIAL_DEBUG){Serial.print("FAIL: Points too close");}
    doBeep(500);
    delay(1000);
    return false;

  }
  if (SERIAL_DEBUG){Serial.print("SUCCESS:");}
  doBeep(50);
  doBeep(100);
  doBeep(50);
  doBeep(100);
  EEPROMWritelong(PPM_MIN_LOC, min);
  EEPROMWritelong(PPM_MAX_LOC, max);
  delay(1000);
  return true;

}


// the setup function runs once when you press reset or power the board
void setup() {
  if (SERIAL_DEBUG){Serial.begin(9600);}
  pinMode(rcIN, INPUT);
  if (hasLED){
    pinMode(redLED, OUTPUT);
    pinMode(greenLED, OUTPUT);
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, HIGH);
  }
  Palatis::SoftPWM.begin(PWM_HZ);
  Palatis::SoftPWM.allOff();
  doBeep(50);
  doBeep(50);
  delay(2000);

  while(pulseIn(rcIN, HIGH, 25000) == 0){
    delay(3000);
    if (hasLED){
      digitalWrite(redLED, LOW);
      digitalWrite(greenLED, LOW);
    }
    doBeep(50);
    if (hasLED){
      digitalWrite(redLED, LOW);
      digitalWrite(greenLED, LOW);
    }
    Serial.println("No Pulse Found");
  }


  //If the EEPROM is empty lets load some sain defaults into the system.
  if (EEPROM.read(PPM_NEVER_SET) == 255){
      EEPROM.write(PPM_NEVER_SET, 32);
      EEPROMWritelong(PPM_MIN_LOC, RC_MIN_DEFAULT);
      EEPROMWritelong(PPM_MAX_LOC, RC_MAX_DEFAULT);
  }

  pulse_time = pulseIn(rcIN, HIGH, 25000);
  if (EEPROMReadlong(PPM_MIN_LOC) > 0){
    rcMin = EEPROMReadlong(PPM_MIN_LOC);
  }
  if (EEPROMReadlong(PPM_MAX_LOC) > rcMin){
    rcMax = EEPROMReadlong(PPM_MAX_LOC);
  }

  if (pulse_time > rcMax * 0.9){
    if (hasLED){
      digitalWrite(redLED, HIGH);
      digitalWrite(greenLED, LOW);
    }
    while (!programMinMax()){ //If min max failed... try again forever
      delay (500);
    }
  }
  applySpeed(0);
  rcMin = EEPROMReadlong(PPM_MIN_LOC);
  rcMax = EEPROMReadlong(PPM_MAX_LOC);
  if (hasLED){
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);
  }
}


// the loop function runs over and over again forever
void loop() {

    pulse_time = pulseIn(rcIN, HIGH, 25000);
    if (pulse_time != lastPulse){
      lastPulse = pulse_time;
      lastUpdate = millis();
    }
    if (millis() - lastUpdate > TIMEOUT || pulse_time == 0){
      //In the event the ESC is running for 31 days an the varible flips over. Lets not thow an error :)
      if (millis() < 2000 && lastUpdate > 2000 ){
        lastUpdate = millis();
      }
      if (SERIAL_DEBUG){
        Serial.write(0xFE);
        Serial.write(0x58);
        Serial.println("Signal Lost.");
      }
      applySpeed(0);
    }
    else {

      finalSpeed = pulse_time;
      smoothSpeed3 = smoothSpeed2;
      smoothSpeed2 = smoothSpeed1;
      smoothSpeed1 = finalSpeed;

      //lets send the median
      if ((smoothSpeed1 <= smoothSpeed2 && smoothSpeed1 >= smoothSpeed3) || (smoothSpeed1 >= smoothSpeed2 && smoothSpeed1 <= smoothSpeed3)){
        finalSpeed = smoothSpeed1;
      }
      else if ((smoothSpeed2 <= smoothSpeed1 && smoothSpeed1 >= smoothSpeed3) || (smoothSpeed1 >= smoothSpeed1 && smoothSpeed1 <= smoothSpeed3)){
        finalSpeed = smoothSpeed2;
      }
      else if ((smoothSpeed3 <= smoothSpeed1 && smoothSpeed1 >= smoothSpeed2) || (smoothSpeed3 >= smoothSpeed1 && smoothSpeed1 <= smoothSpeed2)){
        finalSpeed = smoothSpeed3;
      }

      finalSpeed = map(finalSpeed, rcMin, rcMax, PWM_LEVELS * -1, PWM_LEVELS);


      if (!waitingForDeadBand){
        applySpeed(finalSpeed);
      } else {
        if (finalSpeed < DEADBAND && finalSpeed > DEADBAND * -1){
          waitingForDeadBand = 0;
        }
        delay(50);
        doBeep(10);
      }
    }
    if (SERIAL_DEBUG){
      delay(8);
      if (printUpdate > 10){
        Serial.write(0xFE);
        Serial.write(0x58);
        Serial.print("Pls: ");
        Serial.print(pulse_time);
        Serial.print(" Spd: ");
        Serial.print(finalSpeed);
        printUpdate = 1;
      } else {
        delay(2);
      }
      printUpdate++;
    }
    else {
      delay(10);
    }
}
