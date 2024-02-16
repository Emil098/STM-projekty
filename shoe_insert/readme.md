Project of smart shoe insert.

The insert contains 5 piezoresistive sensors, a vibration motor and a device with an STM32L151 microcontroller and a BLE HM-10 module. The device sends data to the mobile application, sending information about the user's current position, the number of detected steps and the pressure of individual sensors. The user's position is recognized based on the sensor resistance measurement. After the application calibrates the correct standing position, the device will check whether the user is standing in the correct position. When a wrong position is detected for more than 2 seconds, the motor will vibrate, notifying the user to change their position. Additionally, to extend the operating time of the device, there is a standby mode. If the user does not use the shoe insert for a specified period of time, the device enters standby mode. The LSM6 gyroscope has an interrupt initiated on the GPIO pin when a step is detected, the pin is connected to the microprocessor and causes exit from standby mode.

STM32CubeIDE\Drivers\private_libs\ - contains custom libraries
\Core\Src\main.c - FreeRTOS tasks
\Core\Src\stm32l1xx_it.c - modified interrupts