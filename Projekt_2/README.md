# DE2_Project

### Team members

* Tomáš Dočkal (Code for driving servo motor, README file)
* Tran Minh Hieu (Code for driving servo motor, translating ADC value for servomotor)

## Hardware description

### Stopwatch and alarm clock.

Enables you to choose between stopwatch which counts up each second up to 100 hours and alarm clock (timer) which lets you set a time period after which a LED will turn on.

Turning the encoder switches between Stopwatch and Timer in the main menu pressing it switches to selected mode or backs up to main menu. Pressing the joystick starts/stops Stopwatch or Timer depending on which is selected, when Timer is selected, moving the joystick left or right switches between hours, minutes and seconds and moving it up or down increments or subtracts from selected time interval. 

### Components used:

Arduino uno

Display HD44780U LCD

Encoder

Joystick HW-504

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
   
#### [flowchart of Timer1 interupt](images/Flowchart_P2_T0.png)
 Responsible for i
 
#### [flowchart of Timer2 interrupt](images/Flowchart_P2_ACD.png)
 Responsible for 

 
## Video

https://user-images.githubusercontent.com/99402754/206269182-13e209bd-6b5b-47b7-9bc2-aaa0024e8cbd.mov
## References


1. Inspiration for Encoder code from: https://howtomechatronics.com/tutorials/arduino/rotary-encoder-works-use-arduino/
2. Needed libraries from https://github.com/tomas-fryza/digital-electronics-2
