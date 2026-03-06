# STM32 Embedded HTTP Server (LwIP)

Lightweight embedded HTTP server running on **STM32 + LwIP**, exposing a browser-based interface for device monitoring and configuration.

# The project demonstrates:

* integration of **LwIP**
* custom request handling (files + POST endpoints)
* session handling using **HTTP cookies**
* web UI (HTML/CSS/JS) served directly from embedded flash (LwIP fsdata)
* **real-time sensor monitoring** via REST endpoint (`/api/sensor`)
* periodic acquisition of **DHT22 temperature and humidity data**
* runtime network configuration (IP / subnet mask / gateway)
* persistent configuration storage in the **last sector of internal flash memory**
* REST API endpoints (`/api/login`, `/api/sensor`, `/api/net`, `/api/auth`)
* **USB UART console** is also available for runtime diagnostics.

# Features

* Embedded HTTP server based on **LwIP HTTPD**
* Web interface written in **HTML / CSS / JavaScript**
* **HTTP POST handling** for device configuration
* **Cookie-based authentication/session handling**
* **Custom filesystem integration (`makefsdata`)**
* Automatic patching of `httpd.c` used by the LwIP HTTPD module
* **Real-time sensor data API (`/api/sensor`)**
* Temperature and humidity acquisition from **DHT22 sensor**
* Sensor values updated every second and exposed via REST endpoint
* **Settings page for runtime configuration**
* Ability to change:
  * device IP address
  * subnet mask
  * gateway
  * login and password
* Configuration stored persistently in the **last sector of MCU flash**
* CI build on GitHub Actions + automatic **GitHub Release** publishing
* Firmware versioning via `version.h`

# Development Environment

The base project configuration was generated using STM32CubeMX and then extended with custom networking, HTTP server logic and persistent configuration handling. The project was generated and developed using the STM32 ecosystem tools:

| Tool | Version |
|------|---------|
| STM32CubeMX | 6.13.0 |
| STM32CubeIDE | 1.18.0 |
| LwIP | 2.1.2 |

# REST API

The firmware exposes several REST endpoints used by the web interface.

## Login
`POST /api/login`

Authenticates the user and creates a **session cookie** used by subsequent requests. If authentication succeeds, the server returns a **session cookie** and redirects the user to dashboard.

Example request:
{
  "user": "admin",
  "pwd":  "admin01"
}

## Sensor data
`GET /api/sensor`

Returns current **temperature and humidity values** from the **DHT22 sensor**. After successful authentication, the web dashboard queries this endpoint every second to update sensor readings.

Example response:
{
  "temp": 24.6,
  "hum": 51.2
}

## Network configuration
`GET /api/net`  
`POST /api/net`

Provides access to device network configuration (IP address, subnet mask and default gateway). Used by settings web page to initialize network related fields.
These settings are stored persistently in the **last sector of MCU flash memory**. After a valid IP address update, the JavaScript client redirects the browser to the login page of the new address.

Example request:
{
  "ip": "192.168.1.50",
  "mask": "255.255.255.0",
  "gw": "192.168.1.1"
}

## Authentication configuration
`GET /api/auth`  
`POST /api/auth`

Allows changing authentication credentials used to access the web interface.
Credentials are stored in the **persistent configuration area located in the last flash sector**.
The "pass" key (password) is present only in POST, when the user submits new credentials.

Example request:
{
  "user": "admin",
  "pass": "admin01"
}

# Web UI + fsdata generation

The web UI is embedded into firmware using `makefsdata`, which converts the `html/` directory into a C source file used by LwIP HTTPD.

# STM32CubeIDE Debug build (automatic)

In STM32CubeIDE the fsdata generation and `httpd.c` patching are executed automatically **as a pre-build step**.

The pre-build script is `make_fsdata.bat` (Windows), this ensures that every local build always uses:

* freshly generated `fsdata_custom.c`
* the patched `httpd.c` required by the project

No manual step is required before pressing **Build** in CubeIDE.

# LwIP HTTPD configuration

The project enables the following LwIP HTTPD features (in `lwipopts.h`):

* `LWIP_HTTPD_SUPPORT_POST` — POST request handling
* `LWIP_HTTPD_CUSTOM_FILES` — custom file open (`fs_handler`)
* `LWIP_HTTPD_FILE_STATE` — per-file state (`fs_state_init/free`)
* `HTTPD_USE_CUSTOM_FSDATA` — use generated (`fsdata_custom.c`)

Note: **SSI is not used**

# Session handling (cookies)

Session handling is implemented using HTTP cookies.

Main modules:

* `session.c` — session generation/validation
* `fs_handler.c` — cookie extraction on file requests
* `post_handler.c` — session verification for POST endpoints

This keeps the HTTP callbacks lightweight while maintaining access control.

# CI pipeline (GitHub Actions)

A GitHub Actions workflow builds the firmware, generates fsdata, compiles the project, and publishes a versioned binary.

Workflow name: **HTTP SERVER CI**

Triggers:

* `push`
* `pull_request`
* `workflow_dispatch`

## Build job

Steps:

1. Checkout repository
2. Prepare `makefsdata` tools
3. Run STM32CubeIDE build (`Debug_CI`)
4. Build Docker image
5. Run firmware packaging script
6. Upload `.bin` artifact

## Release job

When pushing to the default branch:

1. Download build artifacts
2. Read firmware version from `version.h`
3. Read device name from `version.h`
4. Create GitHub Release
5. Upload compiled `.bin`

# Linux fsdata generation (CI)

For CI builds a Linux helper script is used make_fsdata.sh amd used as pre-build command in Debug_CI configuration

Responsibilities:

* run `makefsdata`
* copy patched `httpd.c`
* generate `fsdata_custom.c` inside the LwIP HTTPD directory

This mirrors the behavior of the Windows pre-build script used in CubeIDE.

# Debug_CI configuration

A dedicated CubeIDE build configuration is used for CI allows CI builds docker image without interfering with the developer's local build configuration.

# Firmware versioning

Firmware version and release naming are defined in:
http_server/http_server/libs/version.h

# Firmware packaging script

The script: scripts/create_firmware.py
automatically prepares the final firmware artifact.

It performs:

1. locating the compiled `.bin`
2. reading version information from `version.h`
3. generating a versioned filename
4. copying the firmware with the new name to it's own location

# Project structure
```bash
http_server/
│
├─ http_server/                    # STM32CubeIDE project
│  ├─ Core/
│  ├─ Middlewares/
│  ├─ libs/                        # directory of 'my libraries'
│  │  ├─ httpd.c                   # patched HTTPD source
│  │  └─ version.h                 # firmware version definitions
│  └─ Debug_CI/                    # CI build output
│
├─ html/                           # web interface (html / css / js)
│
├─ makefsdata/                     # fsdata generators
│
├─ make_fsdata.bat                 # Windows pre-build script
├─ make_fsdata.sh                  # Linux CI helper
│
├─ Dockerfile                      # container used for CI packaging
│                                  # in create_firmware.py
└─ scripts/
   └─ create_firmware.py           # firmware packaging tool
```

# UART Console

A **UART debug** console is available for runtime diagnostics.

* Baudrate: 115200
* Parity: UART_PARITY_NONE
* Stop bits: UART_STOPBITS_1
* Word length: UART_WORDLENGTH_8B

# LwIP HTTPD modification

The project requires a patched version of httpd.c. STM32CubeMX regenerates middleware sources when the project configuration changes. This process overwrites the httpd.c, therefore the project maintains a custom patched copy. 

To ensure the correct version is always used:

* the patched httpd.c is stored inside the repository
* it is copied automatically before every build
* The modification introduces the callback `httpd_preview_headers()`

This callback allows the firmware to inspect HTTP headers before httpd_post_begin() is called.
It is used to detect authentication cookies early during request processing.

# Notes

* Web assets are automatically embedded into firmware during build.
* CI produces deterministic firmware builds and attaches them to GitHub Releases.
* The project separates networking, HTTP handling, session logic, and UI layers for maintainability.
* The linker script was modified so that the application firmware does not overwrite configuration data.
