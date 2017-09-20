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

## Installation Instructions (For SimonK ESCs)
1. Look for the .hex file for your ESC in the firmware directory.
1. Download the [BLHeli flashing tool](https://blhelisuite.wordpress.com/) and flash the .hex file
1. Use the "Flash Other" Option to flash the hex file.


##Tested firmware
* SKGeneric
  * KingKong 12a
  * T-Motor S35A
