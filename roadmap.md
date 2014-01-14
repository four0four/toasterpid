## Interrupts
  + 100Hz
    + Update SSR counter (8 bit value, allowed to overflow)
    + Compare to PID value (10Hz update?), set accordingly
  + Every 10 cycles of above (@10Hz):
    + Update LCD?
    + Check timestamp of current target, update as needed
    + Dump state (PID value, error, temperature, timestamp) to serial (CSV)
## Notes
  + 8MHz RC clock is probably bad for timing, may add external (temp. compensated) xtal in final
## TODO
  + Change order of args in some lcd functions?
  + Discard junk thermocouple reads (<0, 4096, etc)
  + Test insulation up to ~250C
    + If that works, pad oven w/insulation, test profiles again
