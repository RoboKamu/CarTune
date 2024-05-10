# Projekt-grupp7
Programkod: TIELA1 <br />
Authors: - <br />
Examiner: -

## 🔍Project Details
This is a first year electrical engineering project. The project includes ....

### 🛠️Hardware
Mikrokontroll: GD32VF103 RISC-V 32-bit MCU <br />
Modules: .. <br />

#### Buttons
4 buttons on breadboard with pull up resistor(10KΩ) to 3.3v. <br/>
closest to MCU on closest pin (hence BUTTON1 on pin A4 and BUTTON4 on pin A7). <br/>
Button actions are respectively  ↑HOUR-UP : ↓HOUR-DOWN : ↑MINUTE-UP : ↓MINUTE-DOWN <br/>

#### LCD (TEMP)
pins from MCU on breadboard to IO card: <br/>
- 3.3V
- GND 
- B13
- B15
- C13
- C15

#### IMU
pins from MCU directly to IMU: <br/>
- 3.3v (red wire)
- GND (black/blue wire)
- SDA on B7 (white wire)
- SCL on B6 (yellow wire)

### </>💻Software
Drivers based on RISC-V ASM <br />
The "library" folder is all the given code before starting the project, not original code. <br /> 
NOTE: in practice to run the code the relevant files from "library" have to be in the same directory as the main.c file. The separation is for clarity.

### 📝Documentation
RISC-V ASM: [riscv-card.pdf](https://github.com/RoboKamu/Projekt-grupp7/files/14948728/riscv-card.pdf) <br />
MCU: [GD32VF103_Datasheet_Rev1.0-1.pdf](https://github.com/RoboKamu/Projekt-grupp7/files/14948731/GD32VF103_Datasheet_Rev1.0-1.pdf) <br />
User Guide: [GD32VF103_User_Manual_EN_V1.0.pdf](https://github.com/RoboKamu/Projekt-grupp7/files/14948745/GD32VF103_User_Manual_EN_V1.0.pdf) <br />
Firmware Guide: [GD32VF103_Firmware_Library_User_Guide_V1.0.pdf](https://github.com/RoboKamu/Projekt-grupp7/files/14948750/GD32VF103_Firmware_Library_User_Guide_V1.0.pdf) <br />
IO circuit: [IO-card_10-11-2020.pdf](https://github.com/RoboKamu/Projekt-grupp7/files/14948757/IO-card_10-11-2020.pdf) <br />
MCU circuit: [MCU-board.pdf](https://github.com/RoboKamu/Projekt-grupp7/files/14948764/MCU-board.pdf)
