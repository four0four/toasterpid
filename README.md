# toasterpid

Firmware for my reflow oven PID controller (Heavily WIP!)

## Complete:
+ Simple serial driver
+ Thermocouple driver
+ LCD driver

## Work in progress:
+ Thermocouple output handling (negative temps, errors)
+ LCD EN line ringing?
 + Test on not-breadboard 1st

## Entirely untouched:
+ PID algorithm
 + PID tuning
+ User interface

## Potential features:
+ USB instead of serial (final version will use an atmega16u4 or similar)
+ Serial (or USB) control
