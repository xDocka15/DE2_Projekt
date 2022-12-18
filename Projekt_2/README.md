# DE2_Project

### Team members

* Tomáš Dočkal (Code for driving servo motor, README file)
* Tran Minh Hieu (Code for driving servo motor, translating ADC value for servomotor)

## Hardware description

### Driving servomotors using PWM channels.

Allows you to control two servo motors using a potenciometer and a button. Pressing the button changes which motors is being cotrolled, rotating the potenciometer then changes angle of the motor.

### Components used:

Arduino uno

servo motors

potenciometer

button

### Schmeatic

 ![schematic](images/schema.png)

## Software description

### Stucture of the project
   ```c
   Project                        // PlatfomIO project
   ├── include                    // Included files
   │   └── timer.h                // Timer library for AVR-GCC includes definitions for Timers/Counters
   ├── lib                        // Libraries
   │   └── GPIO                   // GPIO library for AVR-GCC used for reading/writing digital pins
   |       ├── gpio.c             
   |       └── gpio.h                  
   ├── src                        // Source file(s)
   │   └── main.c
   └── platformio.ini             // Project Configuration File
   README.md                      // Report of this project
   images
   ```
   
 ### Flowcharts
   
#### [flowchart of Timer0 interupt](images/Flowchart_P2_T0.png)
 Responsible for switching between which servo is being controlled.
 
#### [flowchart of ADC interrupt](images/Flowchart_P2_ACD.png)
 Responsible for converting analog input into OCR2x value for servomotor

 
## Video

https://user-images.githubusercontent.com/99402754/206269182-13e209bd-6b5b-47b7-9bc2-aaa0024e8cbd.mov
## References


1. Inspiration for Encoder code from: https://howtomechatronics.com/tutorials/arduino/rotary-encoder-works-use-arduino/
2. Needed libraries from https://github.com/tomas-fryza/digital-electronics-2
