# MegaBrush
MegaBrush is a firmware for common atmel based brushless ESCs.
This library makes it possible easily convert a brushless ESC into a brushed ESC.

## Features
* 1khz PWM with 128 levels per motor direction.
* Forward and Reverse Operation
* Runtime configurable Min and Max PPM inputs
* Simple PPM Smoothing
* Deadband for throttle center
* Easy to install when using SimonK Bootloader
* Failsafe on signal loss
* Throttle must pass through deadband before ESC will spin motor

## Installation Instructions (For SimonK ESCs)
1. Look for the .hex file for your ESC in the firmware directory.
1. Download the [BLHeli flashing tool](https://blhelisuite.wordpress.com/) and flash the .hex file
1. Use the "Flash Other" Option to flash the hex file.

## Tested firmware
* SKGeneric
  * KingKong 12a - https://hobbyking.com/en_us/2-4s-12a-esc-opto.html
  * T-Motor S35A - https://www.amazon.com/gp/product/B00JWTDYEE/ref=oh_aui_search_detailpage?ie=UTF8&psc=1

## ESC Performance
![Oscilloscope](/docs/scope.png)


## Operating Instructions
* Wire the ESC to your brushed motor
  * MegaBrush uses phases A + C from the brushless motor controller.
  * Phase B is not used.
* Program Min/Max for your RC Transmitter
  * Power on the ESC with the transmitter transmitting the MAX value
  * You should hear 3 beeps from the motor indicating you are in programming mode
  * After 2 seconds you will here 1 beep, Move the transmitter so you are transmitting RC Min
  * After 2 seconds you should here 3 beeps indicating successful programming of Min/Max
  * Return the RC transmitter to the neutral setting
* If the motor is making a fast quiet beep it's likely because your transmitter sticks are not centered. Center your sticks at power up to enable the ESC.
