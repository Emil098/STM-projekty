Project of RGB LED controller and measurement of ambient conditions device based on the STM32F746G discovery board and BME680 sensor for measuring temperature, humidity and pressure. GUI design is made in TouchGFX Designer and uses the MVP (model - view - presenter) pattern to exchange information between the backend and the display. The TouchGFX\gui\ folder contains support for user interaction with the display (in C++). The private_libs folder contains libraries for handling the environmental parameters sensor and a library (gui_handle) exchanging data between the backend and the model (MVP). Errors related to communication with the sensor are displayed on the Default Screen. After certain period of time of not using touch screen, backlight is turned of and after detecting touch then it's turned on again (it's handled in FreeRTOS sensors task in main.c).

# STM32F746G_DISCO TBS Version 3.0.16.

The default IDE is set to STM32CubeIDE, to change IDE open the STM32F746G_DISCO.ioc with CubeMX and select from the supported IDEs (EWARM from version 8.50.9, MDK-ARM, and STM32CubeIDE). Supports flashing of the STM32F746G_DISCO board directly from TouchGFX Designer using GCC and STM32CubeProgrammer.Flashing the board requires STM32CubeProgrammer which can be downloaded from the ST webpage. 

This TBS is configured for 272 x 480 pixels 16bpp screen resolution.  

Performance testing can be done using the GPIO pins designated with the following signals: VSYNC_FREQ  - Pin PB4(D3), RENDER_TIME - Pin PC6(D1), FRAME_RATE  - Pin PC7(D0), MCU_ACTIVE  - Pin PG6(D2)
 