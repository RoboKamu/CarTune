##  Readme
Based on init-pskiva code. <br/>
The butt.c file is used for using init-pskiva branch main code (change time on display) but with buttons on a breadboard <br/>
first main.c commit is based on simply wiring the imu to the breadboard, same code as imu-pskiva branch

### Buttons
4 buttons on breadboard with pull up resistor(10KΩ) to 3.3v. <br/>
closest to MCU on closest pin (hence BUTTON1 on pin A4 and BUTTON4 on pin A7) <br/>
Button actions are respectively  ↑HOUR-UP : ↓HOUR-DOWN : ↑MINUTE-UP : ↓MINUTE-DOWN <br/>

### LCD
pins from MCU on breadboard to IO card: <br/>
- 3.3V
- GND 
- B13
- B15
- C13
- C15

### IMU
pins from MCU directly to IMU: <br/>
- 3.3v (red wire)
- GND (black/blue wire)
- SDA on B7 (white wire)
- SCL on B6 (yellow wire)