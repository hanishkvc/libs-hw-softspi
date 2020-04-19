# SoftSPI library

This allows one to emulate a SPI bus / line over GPIO lines

If one doesnt have a SPI controller on a given hardware platform
(i.e SOC or uC) or if the existing SPI lines are fully used up,
then one can use some spare GPIOs to emulate a SPI bus.

## Look in the source code for details, its pretty simple

One requires to define few constants and functions to adapt
this logic to work on ones given hardware platform.
