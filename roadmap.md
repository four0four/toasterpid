## Interrupts
+ Every second
 + Update desired temp from EEPROM
 + Send current desired, current temp, and error over serial
+ Every 0.1 seconds
 + Update LCD
+ Every 1/60 seconds
 + Update SSR state according to PID results
+ So far: 60Hz tick, set bits checked in loop

