# Vita Flight Radar

A PS Vita homebrew live aircraft radar for Wi-Fi connected Vita systems, including the PS Vita Slim (PCH-2000).

It does not require GPS, 3G, cellular service, Bluetooth, or a companion computer.

## Features

- Live nearby aircraft from the adsb.fi open-data API.
- Current endpoint: `/api/v3/lat/{lat}/lon/{lon}/dist/{dist}`.
- Vita2D radar interface at 960x544.
- Callsign, registration, ICAO type, altitude, speed, heading, bearing, distance, ICAO hex and coordinates when present.
- Great-circle distance and initial bearing calculations.
- Aircraft sorted nearest-first.
- Touch an aircraft to select it.
- UP/DOWN selection with buttons.
- L/R radar range: 20, 40, 80, 120, 200 or 250 NM.
- Automatic refresh every 10 seconds by default.
- START forces a refresh and can retry networking after a startup failure.
- TRIANGLE opens the location editor.
- Configuration saved in `ux0:data/VitaFlightRadar/config.txt`.
- Graceful handling of missing aircraft fields and malformed API responses.
- No API key and no account required.

## Why location is manual

The PS Vita Slim has Wi-Fi but no GPS. This build therefore uses a saved latitude/longitude as the radar center. The first-run default is `32.0853, 34.7818` from the original prototype and can be changed inside the app.

## Controls

Main screen:
- START: refresh now
- UP / DOWN: previous / next aircraft
- L / R: radar range
- TRIANGLE: location editor
- SQUARE: automatic refresh on/off
- SELECT: exit
- Front touchscreen: tap an aircraft

Location editor:
- UP / DOWN: latitude
- LEFT / RIGHT: longitude
- L / R: adjustment step
- X: save
- CIRCLE: cancel

## Build dependencies

```bash
vdpm install libvita2d freetype jansson curl openssl libpng libjpeg-turbo zlib
```

## Local build

Linux/macOS/WSL:

```bash
export VITASDK=/path/to/vitasdk
export PATH="$VITASDK/bin:$PATH"
./build_vita.sh
```

Windows PowerShell:

```powershell
.\build_windows.ps1
```

Expected result:

```text
build/VitaFlightRadar.vpk
```

## GitHub Actions build

The project includes `.github/workflows/build.yml`, using the frozen official VitaSDK `2026.08-20260813` container, which includes the target package set. Push the project to GitHub, run **Build VitaFlightRadar VPK**, then download the `VitaFlightRadar-VPK` artifact.

## Install on Vita

1. Transfer `VitaFlightRadar.vpk` to the Vita with VitaShell USB or FTP.
2. In VitaShell, highlight the VPK and press X.
3. Confirm installation.
4. Launch **Vita Flight Radar** from LiveArea.
5. Press TRIANGLE to change the radar center if needed.

## Data source

Aircraft data comes from adsb.fi open data: https://adsb.fi/. Its nearby-aircraft API is for personal, non-commercial use, requires attribution, supports distances up to 250 NM, and is rate-limited to 1 request per second. This app defaults to one request every 10 seconds.

## TLS note

The current VitaSDK libcurl sample disables certificate verification for compatibility with Vita homebrew environments that do not have a maintained CA bundle. This project follows that approach only for a public, read-only aircraft-data request and sends no credentials or private data.

## Limitations

- This is homebrew software for a modified PS Vita.
- The Vita Slim still does not gain GPS; its center point is manual.
- ADS-B data does not reliably contain origin/destination, so this version does not fabricate route information.
- Coverage depends on contributing ADS-B receivers.
- Third-party APIs can change in the future.
