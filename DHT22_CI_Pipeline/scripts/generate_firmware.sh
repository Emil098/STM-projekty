#!/bin/bash

# Ścieżki
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BIN_DIR="$SCRIPT_DIR/../STM32CubeIDE/Debug"
VER_DIR="$SCRIPT_DIR/../STM32CubeIDE/Drivers/libs"
VER_FILE="$VER_DIR/version.h"

# Szukamy pliku bin
BIN_FILE=$(find "$BIN_DIR" -maxdepth 1 -type f -name "*.bin" | head -n 1)

if [ -z "$BIN_FILE" ]; then
    echo "Nie znaleziono pliku .bin w $BIN_DIR"
    exit 1
fi

# Sprawdzamy plik version.h
if [ ! -f "$VER_FILE" ]; then
    echo "Brak pliku version.h w $VER_DIR"
    exit 1
fi

# Pobieramy wersję z version.h
VERSION=$(grep -E '^#define[[:space:]]+FIRMWARE_VERSION[[:space:]]+"' "$VER_FILE" \
     | sed -E 's/.*"([^"]+)".*/\1/')

if [ -z "$VERSION" ]; then
    echo "Nie udało się odczytać wersji z $VER_FILE"
    exit 1
fi

# Pobieramy nazwę urządzenia z version.h
DEV_NAME=$(grep -E '^#define[[:space:]]+BIN_DEST_NAME[[:space:]]+"' "$VER_FILE" \
     | sed -E 's/.*"([^"]+)".*/\1/')

if [ -z "$VERSION" ]; then
    echo "Nie udało się odczytać nazwy urządzenia z $VER_FILE"
    exit 1
fi

# Data
DATE=$(date +"%Y-%m-%d")

# Nazwa pliku wynikowego
# BASENAME=$(basename "$BIN_FILE" .bin)
BASENAME="Prod"
NEW_FILENAME="${BASENAME}_${DEV_NAME}_v${VERSION}.bin"
NEW_PATH="$SCRIPT_DIR/$NEW_FILENAME"

# Kopiowanie
cp "$BIN_FILE" "$NEW_PATH"

echo "✅ Skopiowano plik:"
echo "   źródło: $BIN_FILE"
echo "   cel:    $NEW_PATH"

exit 0