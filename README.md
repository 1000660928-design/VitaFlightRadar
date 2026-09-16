# Vita Flight Radar

PS Vita homebrew live aircraft radar for Wi-Fi connected Vita systems, including the PS Vita Slim (PCH-2000).

This repository contains the complete VitaFlightRadar v1.0 source archive and an automated VitaSDK GitHub Actions build. The workflow extracts the source, cross-compiles it with VitaSDK, packages `VitaFlightRadar.vpk`, and uploads the VPK as a downloadable Actions artifact.

## Build

Open the **Actions** tab and run **Build VitaFlightRadar VPK**, or push to `main`. A successful run produces an artifact named `VitaFlightRadar-VPK` containing the installable `.vpk`.

## What the app does

- Uses the Vita's Wi-Fi connection
- Queries live nearby aircraft data from adsb.fi
- Displays aircraft on a radar-style Vita2D interface
- Shows callsign, registration, altitude, speed, heading, bearing and distance when available
- Supports touch selection and Vita controls
- Supports 20, 40, 80, 120, 200 and 250 NM radar ranges
- Refreshes automatically every 10 seconds
- Saves a manually configured latitude/longitude because the Vita Slim has no GPS

## Install

After downloading the VPK artifact, transfer `VitaFlightRadar.vpk` to your modified PS Vita with VitaShell USB or FTP, then install it from VitaShell.

The source archive is also available directly in this repository as `VitaFlightRadar-v1.0-source.zip`.
