#!/usr/bin/env bash
set -euo pipefail

if [[ -z "${VITASDK:-}" ]]; then
  echo "VITASDK is not set."
  echo "Install VitaSDK first, then: export VITASDK=/path/to/vitasdk"
  exit 1
fi

export PATH="$VITASDK/bin:$PATH"

vdpm install libvita2d freetype jansson curl openssl libpng libjpeg-turbo zlib

rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel

echo
echo "SUCCESS: build/VitaFlightRadar.vpk"
