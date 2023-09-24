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
### Firmware update over the CAN bus using STM32
This project provides a simple framework for performing firmware updates for STM32F103C8T6 microcontroller over the CAN bus.<br />
A custom bootloader is responsible for switching between the 2 firmware versions using 2 buttons.
#### Project Description
##### Objective
Update the initial firmaware running on the STM32 that toggles the yellow LED every 1 sec.<br />
to the new firmware that toggles the yellow LED every 0.5 sec.
##### Steps
1. Convert updated firmaware (([LED_BLINK](https://github.com/HossamElwahsh/firmware_flash_using_can_with_custom_bootloader/tree/main/LED_BLINK))) [.bin](https://github.com/HossamElwahsh/firmware_flash_using_can_with_custom_bootloader/blob/main/LED_BLINK/Debug/LED_BLINK.bin)  file  to an array 
using this tool: https://notisrac.github.io/FileToCArray/.

