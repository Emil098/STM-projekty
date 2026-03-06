%~dp0makefsdata\makefsdata.exe %~dp0html -f:%~dp0http_server\Middlewares\Third_Party\LwIP\src\apps\http\fsdata_custom.c
if errorlevel 1 pause

copy "%~dp0http_server\libs\httpd.c" "%~dp0http_server\Middlewares\Third_Party\LwIP\src\apps\http\" /Y
