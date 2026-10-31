# Bootstrap.bas loader for the RPi Pico

This quick-and-dirty program emulates a keyboard and enters bootstrap.bas into the device it's connected to. Based off of the tinyusb example hid_boot_interface.

# Building
1) `cmake -B build .`
2) `cd build && make`
3) Flash uf2 to Pico

# Usage
1) Get to the text input box where you want the code
2) Plug in the Pico
3) It will wait 2s before entering the code

# Updating bootstrap.bas
Run this command: `xxd -i bootstrap.bas > bootstrap.bas.h`
And then re-build the image
