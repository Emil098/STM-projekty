Description:
This project is a prototype of a website based on the lwip library. The PCB contains: the STM107VCT microcontroller and the IP178CH system (managed switch) which uses the RMII interface for communication. The website includes: index.html (home page), error.html (appears after entering the wrong website address), leds.shtml (page controlling 3 LEDs) and login.shtml (page where entering the correct login and password causes redirection to leds.shtml) This project present the capabilities of the lwip library which can both download data (using CGI) and update the content on individual pages (using SSI). Websites written in HTML are located in the Middlewares\Third_Party\LwIP\src\apps\httpd\fs\

Tips:
- Change the macro value in the lwipopts.h library #define HTTPD_USE_CUSTOM_FSDATA 0, (folder - LWIP\Target)

- Before initializing rmii, you must reset IP178CH by applying a low state to the appropriate GPIO output in the project marked as RESET_IP.

- The processor must set a clock at the MCO output of f=25MHz, otherwise the switch will not start working.

- The #define FS_FILE_FLAGS_SSI 0x08 macro should be added to the fs.h file Middlewares\Third_Party\LwIP\src\apps\httpd (it is not included in the old LWIP library).

- After making changes to the .html and .shtml files, run the makeFSdata.exe program (in the \Middlewares\Third_Party\LwIP\src\apps\httpd\ folder) and then rebuild the project.

- SSI and CGI handlers can be found at http_ssi.c
