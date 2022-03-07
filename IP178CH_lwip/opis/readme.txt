Opis:
Projekt zawiera prototyp strony internetowej opartej na bibliotece lwip. Na p³ytce pcb znajduj¹ siê: mikrokontroler STM107VCT oraz uk³ad IP178CH (zarz¹dzalny switch) który do komunikacji wykorzystuje interfesj RMII. W sk³ad strony wchodz¹: index.html (strona g³ówna), error.html (pojawia siê po wpisaniu b³êdnego adresu strony), leds.shtml (strona steruj¹ca 3 ledami) oraz login.shtml (strona w której wpisanie poprawnego loginu i has³a powduje przekierowanie do leds.shtml) Idê¹ projektu jest zaprezentowanie dzia³ania serwera http, z którego mikroprocesor mo¿e zarówno pobieraæ dane (przez zastosowanie CGI), jak równie¿ aktualizowaæ aktualizowaæ treœci na poszczególnych stronach (przy pomocy SSI). Strony internetowe napisane w htmlu znajduj¹ siê w folderze LWIP_v4\Middlewares\Third_Party\LwIP\src\apps\httpd\fs\

Wa¿ne informacje:
- W bibliotece lwipopts.h nale¿y zmieniæ wartoœæ makra #define HTTPD_USE_CUSTOM_FSDATA 0, katalog LWIP\Target.

- Przed zainicjowaniem rmii nale¿y zresetowaæ IP178CH poprzez podanie stanu niskiego na odpowiednie wyjœcie gpio w projekcie oznaczone jako RESET_IP.

- Procesor musi wystawiæ zegar na wyjœciu MCO f=25MHz, bez tego switch nie zacznie dzia³aæ.

- Do pliku fs.h (katalog Middlewares\Third_Party\LwIP\src\apps\httpd) nale¿y dodaæ makro #define FS_FILE_FLAGS_SSI 0x08 (w starej bibliotece LWIP go nie ma).

- Po dokonaniu zmian w pliakch .html i .shtml nale¿y uruchomiæ program makeFSdata.exe (w folderze d:\stm projekty\LWIP_v4\Middlewares\Third_Party\LwIP\src\apps\httpd\) nastêpnie przebudowaæ projekt.

- Za halndlery ssi i cgi odpowiada biblioteka http_ssi .