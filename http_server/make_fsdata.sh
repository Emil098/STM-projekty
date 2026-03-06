#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

MAKEFSDATA_BIN="$SCRIPT_DIR/makefsdata/makefsdata"
HTML_DIR="$SCRIPT_DIR/html"
OUT_C="$SCRIPT_DIR/http_server/Middlewares/Third_Party/LwIP/src/apps/http/fsdata_custom.c"
HTTPD_SRC="$SCRIPT_DIR/http_server/libs/httpd.c"
HTTPD_DST="$SCRIPT_DIR/http_server/Middlewares/Third_Party/LwIP/src/apps/http/"

echo "Running makefsdata:"
echo "  bin : $MAKEFSDATA_BIN"
echo "  html: $HTML_DIR"
echo "  out : $OUT_C"

command -v perl >/dev/null 2>&1 || { echo "ERROR: perl not installed"; exit 127; }
command -v find >/dev/null 2>&1 || { echo "ERROR: find not installed"; exit 127; }

[[ -f "$MAKEFSDATA_BIN" ]] || { echo "ERROR: $MAKEFSDATA_BIN not found"; exit 127; }
chmod +x "$MAKEFSDATA_BIN"

cp -rf $HTTPD_SRC $HTTPD_DST
echo "Copied httpd.c to - $HTTPD_DST"

tmpdir="$(mktemp -d)"
trap 'rm -rf "$tmpdir"' EXIT

mkdir -p "$tmpdir/fs"
cp -a "$HTML_DIR/." "$tmpdir/fs/"

# uruchom w katalogu tmp, bo perl generuje ./fsdata.c i wymaga ./fs/
(
  cd "$tmpdir"
  "$MAKEFSDATA_BIN"
)

mkdir -p "$(dirname "$OUT_C")"
cp -f "$tmpdir/fsdata.c" "$OUT_C"

echo "OK: generated $OUT_C"