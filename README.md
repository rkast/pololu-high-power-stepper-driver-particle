# High-Power Stepper Motor Driver library for Particle

Version: 0.0.1<br/>
Release date: 2019-11-11<br/>
[www.pololu.com](https://www.pololu.com/)

## Summary

This is a library for the Particle ecosystem that helps interface with a [Pololu
High-Power Stepper Motor Driver 36v4][hpsd-36v4].  It uses the existing Particle SPI capabilities library to communicate with the SPI interface (SCS, SCLK, SDATI, and
SDATO) of the driver. This library was ported over from an existing Arduino library provided by Pololu so all credit goes to them. I just needed this capability in the particle ecosystem and thought I'd publish it in case others did too. To Pololu, if I didn't get crediting or licensing correct, please reach out and I'd be happy to update this.

## Supported platforms

This library is designed to work with the Particle ecosystem. I personally still use the Particle Desktop IDE (atom) and will be testing with a GEN 2 Photon running at firmware version 1.4.2
I may get around to testing with other .

## Getting started

### Hardware

The HighPowerStepperDriver library supports Pololu's [High-Power Stepper Motor
Driver 36v4][hpsd-36v4].  Before continuing, careful reading of its product
page is recommended.

You will need to connect your motor, motor power, and IOREF as described on the
product page.  You should also make the following connections between the
Particle and the driver:

| Particle     | High-Power Stepper Motor Driver |
|--------------|---------------------------------|
| D0           | DIR                             |
| D1           | STEP                            |
| A2 (SS)      | SCS                             |
| A3 (SCK)     | SCLK                            |
| A4 (MISO)    | SDATO                           |
| A5 (MOSI)    | SDATI                           |
| GND          | GND                             |

The SDATO pin is only needed if you want to read information back from the
stepper driver.  Since the motor can be stepped and its direction changed using
the SPI interface, it is possible to use the driver without connecting the STEP
and DIR pins, and they are not used in every example.

## Examples

Several example sketches are available that show how to use the library. You
can access them under the examples folder, which is subdivided by whether you intend to use SPI or Step/Dir based control.

## Documentation

For complete documentation of this library, including many features that were
not mentioned here, see [the high-power-stepper-driver-arduino
documentation][doc].  If you are already on that page, see the
HighPowerStepperDriver class reference.

## Version history

* 1.0.0 (2019-06-04): Original release.

[particle]: https://docs.particle.io/reference/device-os/firmware/photon/
[github]: https://github.com/rkast/pololu-high-power-stepper-driver-particle
[Original Arduino github]: https://github.com/pololu/high-power-stepper-driver-arduino/releases
[hpsd-36v4]: https://www.pololu.com/product/3730
[spi]: https://docs.particle.io/reference/device-os/firmware/photon/#spi
