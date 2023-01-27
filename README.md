# midi2gate
This project contains firmware, schematics and gerber file for MIDI to GATE converter based on ATMEGA328p.

This is a DIY project, suitable for beginners, although a general knowledge of electronics, basic tools and skills are required to build this converter. The schematics presented here feature:

- midi input
- 6 programmable buffered gate outputs
- midi channel setting dip switch

The main feature of the project is the support of midi polyphony, which means that all outputs can send a signal at once. Another feature is that you can assign any midi notes to the individual outputs using two buttons and thus not limit yourself to a predetermined range of notes.

## Usage
My original intention was to send a signal to the drum module in my modular synth, which only has analog inputs. But the possibilities are endless.

## How to
Want to change midi note on output? Just press Button A. After LED blinking sequence you can choose desired output with Button B (signaled by LED) and press note on your midi controler. If you are done just press Button A and it will save it to EEEPROM.

Want to set midi channel? In power off state set dip switch to corresponding binary value.
