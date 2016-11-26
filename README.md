# ece445_HWcode
ece445 related code (except the App)
Please switch to branch `PID-test`
The final Arduino code is in `flow_test/flow_test.ino`. All other files are actually for testing seperate units and contribute nothing to final code.

## explain to subfolders
1. `./` and general
  * `Tutorial1-8.jpg` is for the Arduino to ATMega pin reference. 
  * `learn_arduino_fritzing.jpg` is for connecting the LCD
  * https://www.arduino.cc/en/Tutorial/ArduinoToBreadboard is for connect the ATMega chip to Arduino board and program. Need to connect Reset, RX, TX and unplug the chip on Arduino board
  * https://learn.adafruit.com/introducing-bluefruit-ez-link is reference for bluetooth
2. `./led_test`
  Test for the LCD (hello world and the elapsed time in second)
3. `./library`
  libraries used in Arduino program. Maybe need to import
4. `./flow_test`
  all things related to the flow sensor and the **final program**
5. `./interrupt_test`
  testing program for the interrupts for flow sensor
6. `./FlowSensorPID`
  The initial algorithm related to flow sensor. Writing and tested in c++
7. `./temp`
  irrelevant temporary tests
