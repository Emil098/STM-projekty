Opis:
Projekt zawiera prototyp strony internetowej opartej na bibliotece lwip. Na płytce pcb znajdują się: mikrokontroler STM107VCT oraz układ IP178CH (zarządzalny switch) który do komunikacji wykorzystuje interfesj RMII. W skład strony wchodzą: index.html (strona główna), error.html (pojawia się po wpisaniu błędnego adresu strony), leds.shtml (strona sterująca 3 ledami) oraz login.shtml (strona w której wpisanie poprawnego loginu i hasła powduje przekierowanie do leds.shtml) Idęą projektu jest zaprezentowanie działania serwera http, z którego mikroprocesor może zarówno pobierać dane (przez zastosowanie CGI), jak również aktualizować aktualizować treści na poszczególnych stronach (przy pomocy SSI). Strony internetowe napisane w htmlu znajdują się w folderze LWIP_v4\Middlewares\Third_Party\LwIP\src\apps\httpd\fs\

Ważne informacje:
- W bibliotece lwipopts.h należy zmienić wartość makra #define HTTPD_USE_CUSTOM_FSDATA 0, katalog LWIP\Target.

- Przed zainicjowaniem rmii należy zresetować IP178CH poprzez podanie stanu niskiego na odpowiednie wyjście gpio w projekcie oznaczone jako RESET_IP.

- Procesor musi wystawić zegar na wyjściu MCO f=25MHz, bez tego switch nie zacznie działać.

- Do pliku fs.h (katalog Middlewares\Third_Party\LwIP\src\apps\httpd) należy dodać makro #define FS_FILE_FLAGS_SSI 0x08 (w starej bibliotece LWIP go nie ma).

- Po dokonaniu zmian w pliakch .html i .shtml należy uruchomić program makeFSdata.exe (w folderze \Middlewares\Third_Party\LwIP\src\apps\httpd\) następnie przebudować projekt.

- Za halndlery ssi i cgi odpowiada biblioteka http_ssi .
