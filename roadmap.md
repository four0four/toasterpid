## Notes
  + 8MHz RC clock is probably bad for timing, may add external (temp. compensated) xtal in final
  + External clk doesn't help much - TODO/FIXME

## TODO
  + Implement v0.9 hardware changes (ethernet, sd)
  + Change order of args in some lcd functions?
  + Discard junk thermocouple reads (<0, 4096, etc)
  + Test insulation up to ~250C
    + If that works, pad oven w/insulation, test profiles again
