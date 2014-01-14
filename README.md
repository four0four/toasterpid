# toasterpid

Firmware for my reflow oven PID controller (WIP!)

## Complete:
+ Simple serial driver
+ Thermocouple driver
+ LCD driver (4-bit, 8-bit)

## Work in progress:
+ PID algorithm/tuning
  + PID values don't differ much from 0 or 255, and given the oven's thermal momentum, they shouldn't...
+ Testing: Insulation in the oven

## Entirely untouched:
+ User interface

## Potential features:
+ USB instead of serial (final version will use an atmega16u4 or similar)
+ Serial (or USB) control
