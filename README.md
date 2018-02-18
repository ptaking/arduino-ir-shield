# IR SHIELD
A very simple programmable infrared shield for Arduino

### Parts list:
* CD4021 x1
* Button x8
* VS1838 IR Sensor x1
* IR LED x1
* Prototype PCB x1

### Schematic:
![Schematic](https://github.com/ptaking/arduino-ir-shield/raw/master/schematic.png)
Here is a layout example:
![Example](https://github.com/ptaking/arduino-ir-shield/raw/master/example.jpeg)

### Usage:
You can define your default codes in `setup()`.

Press program button to enter program mode, the LED will blink slowly.

Press the button you want to program, the LED will blink faster.

Point your remote toward the shield, press the button you want to assign, the LED will stop blinking.

### TO-DO:
* Save data to EPROM.
* Press program button again to exit program mode.
