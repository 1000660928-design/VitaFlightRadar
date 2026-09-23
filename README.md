# VitaFlightRadar ✈️

**Live aircraft tracking on a PlayStation Vita.**

VitaFlightRadar is a PS Vita homebrew application that uses Wi-Fi to download live ADS-B aircraft data and plot nearby airplanes over satellite imagery. The project started as a circular radar experiment and evolved into a touch-controlled satellite flight map with panning, pinch zoom, coordinate search, aircraft selection and route information.

## Download — choose your PS Vita model

VitaFlightRadar now has **separate public builds for the two Vita hardware families**. Download the build that matches your console.

### PS Vita 2000 Slim — PCH-20xx

**Hardware-tested / recommended Slim build: v1.9.2**

### **[Download VitaFlightRadar v1.9.2 — PCH-2000 Slim](releases/VitaFlightRadar-v1.9.2.vpk)**

This is the build developed and repeatedly tested on the project creator's PS Vita 2000 Slim. It includes the stable slippy satellite map, high-resolution close zoom, touch controls and the complete current feature set.

### PS Vita 1000 FAT / OLED — PCH-10xx / PCH-11xx

**Dedicated FAT compatibility build: v1.9.3-PCH1000**

### **[Download VitaFlightRadar v1.9.3 — PCH-1000 FAT/OLED](releases/VitaFlightRadar-v1.9.3-PCH1000.vpk)**

This build keeps the same application, interface and feature set as v1.9.2, but replaces the network/map compatibility layer that caused a reported black satellite map on a PCH-1000.

PCH-1000-specific changes:

- Uses VitaSDK **libcurl with an mbedTLS backend** for HTTPS/TLS instead of relying on the console firmware's native HTTPS behavior.
- Explicitly creates the application and satellite-cache directory structure.
- Automatically falls back through lower satellite tile levels if an aggressive high-resolution tile is unavailable instead of leaving the map black.
- Retains the v1.9.2 high-resolution satellite system, including zoom levels up to 21 and the larger 40-tile texture cache.
- Built with the same stable v1.8-derived map renderer rather than the broken experimental v1.9 async renderer.
- Public package contains no creator-specific startup coordinates.

The PCH-1000 package has passed VitaSDK compilation, linking, VPK integrity and installer-asset validation. **Real PCH-1000 hardware confirmation is still requested**, because the project creator's own test console is a PCH-2000. If you test the FAT/OLED build, please report your exact Vita model, system software version, storage setup (official memory card / SD2Vita), whether satellite imagery appears after coordinate entry, and whether aircraft icons appear.

See [RELEASES.md](RELEASES.md) for the model-specific release index and [CHANGELOG.md](CHANGELOG.md) for the complete development history.

## Current feature set

- Live nearby aircraft positions over satellite imagery.
- One-finger map panning.
- Stationary touch-and-hold recentering.
- Two-finger pinch zoom.
- Aircraft search radius automatically follows the visible map area.
- Tap aircraft to select them.
- UP / DOWN aircraft cycling.
- Coordinate search using the Vita on-screen keyboard.
- AUTO / MANUAL live refresh modes.
- Manual refresh with START.
- Selected-flight information including altitude, speed, heading, distance and route/timetable data when public data is available.
- Known helicopters, rotorcraft, drones, balloons, gliders and other known non-airplane ADS-B categories are filtered out.
- Persistent satellite tile caching.
- High-resolution satellite sharpening at close zoom levels.

## Controls

| Control | Action |
| --- | --- |
| **One-finger drag** | Pan the satellite map; release to track aircraft around the new center |
| **Stationary touch-and-hold** | Recenter directly on the point under your finger |
| **Two-finger pinch** | Zoom the map and automatically change the live aircraft search radius |
| **Tap aircraft** | Select aircraft |
| **Triangle** | Search / enter coordinates |
| **X** | Confirm / Enter in supported dialogs |
| **Circle** | Cancel / Back in supported dialogs |
| **Square** | Toggle AUTO / MANUAL aircraft refresh |
| **UP / DOWN** | Previous / next aircraft |
| **START** | Manual aircraft refresh |
| **PS button** | Leave / suspend through the Vita system UI |

There is intentionally no separate hardware zoom or aircraft-radius control. Map zoom and aircraft search radius work together through the touch screen.

## Moving around the map

Drag with one finger to move the map. When the drag ends, the center of the visible map becomes the new aircraft tracking point. Hold one finger still to recenter directly on that geographic point. Pinch with two fingers to zoom in or out.

The map wraps horizontally around the Earth. The public nearby-aircraft API still has a finite point/radius search limit, so a very zoomed-out view does not mean the app can request every aircraft on Earth simultaneously.

## AUTO vs MANUAL

**AUTO** refreshes live aircraft data approximately every 4 seconds.

**MANUAL** pauses automatic aircraft-data refreshes. Press **START** whenever you want a fresh update.

## Coordinate search

Press **Triangle** and enter decimal coordinates, for example:

```text
40.7128,-74.0060
```

Confirm with **X**. The app jumps to that point, loads satellite imagery and retrieves aircraft around the visible area.

## Installing on a PS Vita

You need a homebrew-enabled PS Vita with **VitaShell** installed.

1. Download the VPK for your Vita model from the model selector above.
2. Open **VitaShell** on the Vita.
3. Connect the Vita to your PC using VitaShell **USB** or **FTP** mode.
4. Copy the VPK to a convenient folder such as `ux0:/data/`.
5. In VitaShell, navigate to the VPK.
6. Press **X** on the file and choose **Install**.
7. Return to the Vita home screen and launch **VitaFlightRadar**.

If an older build refuses to update cleanly, delete the old VitaFlightRadar bubble and install the correct model-specific VPK fresh.

## How it works

The Vita itself is **not an ADS-B radio receiver**. VitaFlightRadar connects through Wi-Fi and requests live nearby aircraft data from the **adsb.fi** open-data API, then plots aircraft relative to the geographic map center currently being viewed.

Satellite imagery is rendered as cached 256x256 map tiles. The current renderer aggressively requests higher-detail imagery at close zoom levels to reduce software-side blur while keeping lower-detail tiles available as a fallback.

Flight route and timetable information is obtained separately because raw ADS-B position data does not reliably contain origin, destination or airline schedule fields. Public aviation data is incomplete, so private or unusual flights can still have missing route/timetable information.

## Model-specific builds

The PCH-1000 and PCH-2000 editions are the **same VitaFlightRadar application**. The split exists so compatibility changes for one hardware/firmware/storage environment do not risk breaking the proven build for the other.

- **PCH-2000:** v1.9.2 remains the hardware-tested Slim build.
- **PCH-1000:** v1.9.3-PCH1000 uses the dedicated TLS/cache/satellite-fallback compatibility path.

Future feature updates can be carried to both editions while preserving separate model-specific packages when necessary.

## Version history

See [RELEASES.md](RELEASES.md) for the release index and [CHANGELOG.md](CHANGELOG.md) for detailed notes from the original proof of concept through the current model-specific builds.

Historical development/build branches remain in the repository.

## Credits

### Creator

**George Ultra**  
Created by George Ultra with help from **ChatGPT / OpenAI** for software development, debugging, VitaSDK integration, UI iteration, research and build automation.

### Technical / Emotional Support

- **Ventiz**
- **YahliKap**

## Feedback, ideas and contact

Contact **George Ultra** on Reddit for ideas, feedback, reviews or bug reports:  
https://www.reddit.com/user/Diligent_Peace_1618/

Real PS Vita hardware testing and community reports are a major part of the development process.

## Development

VitaFlightRadar is written in C for **VitaSDK** and uses Vita2D for rendering. GitHub Actions builds and validates the installable VPKs.

The project includes Vita networking, HTTPS/TLS handling, map caching, static-library relocation/link fixes, Vita-safe icon/VPK packaging and LiveArea validation.

The dedicated PCH-1000 build is maintained on the `pch1000-build` development branch and is built separately from the proven PCH-2000 release.

## Disclaimer

VitaFlightRadar is an unofficial hobby/homebrew project and is not affiliated with Sony, FlightRadar24, adsb.fi, Esri, OpenStreetMap, airlines, airports or aircraft manufacturers.

Do not use this application for navigation, air-traffic control, safety-critical decisions or operational aviation purposes.
