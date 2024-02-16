Opis:
Projekt zawiera prototyp strony internetowej opartej na bibliotece lwip. Na p�ytce pcb znajduj� si�: mikrokontroler STM107VCT oraz uk�ad IP178CH (zarz�dzalny switch) kt�ry do komunikacji wykorzystuje interfesj RMII. W sk�ad strony wchodz�: index.html (strona g��wna), error.html (pojawia si� po wpisaniu b��dnego adresu strony), leds.shtml (strona steruj�ca 3 ledami) oraz login.shtml (strona w kt�rej wpisanie poprawnego loginu i has�a powduje przekierowanie do leds.shtml) Id� projektu jest zaprezentowanie dzia�ania serwera http, z kt�rego mikroprocesor mo�e zar�wno pobiera� dane (przez zastosowanie CGI), jak r�wnie� aktualizowa� aktualizowa� tre�ci na poszczeg�lnych stronach (przy pomocy SSI). Strony internetowe napisane w htmlu znajduj� si� w folderze LWIP_v4\Middlewares\Third_Party\LwIP\src\apps\httpd\fs\

Wa�ne informacje:
- W bibliotece lwipopts.h nale�y zmieni� warto�� makra #define HTTPD_USE_CUSTOM_FSDATA 0, katalog LWIP\Target.

- Przed zainicjowaniem rmii nale�y zresetowa� IP178CH poprzez podanie stanu niskiego na odpowiednie wyj�cie gpio w projekcie oznaczone jako RESET_IP.

- Procesor musi wystawi� zegar na wyj�ciu MCO f=25MHz, bez tego switch nie zacznie dzia�a�.

- Do pliku fs.h (katalog Middlewares\Third_Party\LwIP\src\apps\httpd) nale�y doda� makro #define FS_FILE_FLAGS_SSI 0x08 (w starej bibliotece LWIP go nie ma).

- Po dokonaniu zmian w pliakch .html i .shtml nale�y uruchomi� program makeFSdata.exe (w folderze d:\stm projekty\LWIP_v4\Middlewares\Third_Party\LwIP\src\apps\httpd\) nast�pnie przebudowa� projekt.

- Za halndlery ssi i cgi odpowiada biblioteka http_ssi .