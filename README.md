# EME - Egypt Makes Electronics

## Class 3 SEITech Solutions

### Team Members
- Hossam Elwahsh - [![GitHub](https://img.shields.io/badge/github-%23121011.svg?style=flat&logo=github&logoColor=white)](https://github.com/HossamElwahsh) [![LinkedIn](https://img.shields.io/badge/linkedin-%230077B5.svg?style=flat&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/hossam-elwahsh/)
- Ahmed Sakr - [![GitHub](https://img.shields.io/badge/github-%23121011.svg?style=flat&logo=github&logoColor=white)](https://github.com/Ahmeddsakrrr)
- Ahmed Hisham - [![GitHub](https://img.shields.io/badge/github-%23121011.svg?style=flat&logo=github&logoColor=white)](https://github.com/ahmedhish)
- Nada Salloum - [![GitHub](https://img.shields.io/badge/github-%23121011.svg?style=flat&logo=github&logoColor=white)](https://github.com/nadasalloum)
- Salma Faragalla - [![GitHub](https://img.shields.io/badge/github-%23121011.svg?style=flat&logo=github&logoColor=white)](https://github.com/SalmaFaragalla)
- 
### ARM Project
### Firmware update over the CAN bus 
This project provides a simple framework for performing firmware updates for STM32F103C8T6 microcontroller over the CAN bus.<br />
A custom bootloader is responsible for switching between the 2 firmware versions using 2 buttons.

### Dist Files

| ECU | APP               | Build File                           | Description                                                                                                                                                                            |
|-----|-------------------|--------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 1   | Custom Bootloader | [hex](dist/custom_bootloader.hex)    | Handles switching between updates, Supports override mode by holding SW (1) to force boot into "main application" skipping loading updated app which is the default behavior if found. |
| 1   | Main Application  | [hex](dist/firmware_application.hex) | Main firmware application handles CAN communication, checking for updates, OLED display step by step, Toggles an LED every 2 seconds in the background.                                |
| 2   | Update Server     | [hex](dist/update_server.hex)        | Runs on a separate ECU, Communicates over CAN, serves new updates to requesting ECUs                                                                                                   |
| -   | Blink Update      | [hex](dist/LED_BLINK.hex)            | Updated application that blinks LED every 1 second                                                                                                                                     |

#### Project Description
##### Objective
Update the initial firmware running on the STM32 that toggles the blue LED every 2 sec.<br />
to a new firmware that toggles the blue LED every 1 sec.
##### Steps
1. Convert updated firmaware (([LED_BLINK](https://github.com/HossamElwahsh/firmware_flash_using_can_with_custom_bootloader/tree/main/LED_BLINK))) [.bin](https://github.com/HossamElwahsh/firmware_flash_using_can_with_custom_bootloader/blob/main/LED_BLINK/Debug/LED_BLINK.bin) file to an array 
using this tool: https://notisrac.github.io/FileToCArray/.

