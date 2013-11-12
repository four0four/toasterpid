# toasterpid

Firmware for my reflow oven PID controller (Heavily WIP!)

## Complete:
+ Simple serial driver
+ Thermocouple driver

## Work in progress:
+ LCD driver
 + Next up: Proper newline/wrapping options
+ Thermocouple output handling (negative temps, errors)

## Entirely untouched:
+ PID algorithm
 + PID tuning
+ User interface


## Potential features:
+ USB instead of serial (final version will use an atmega16u4 or similar)
+ Serial (or USB) control
