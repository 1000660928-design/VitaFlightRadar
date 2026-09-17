# VitaFlightRadar ✈️

**Live aircraft tracking on a PlayStation Vita.**

VitaFlightRadar is a PS Vita homebrew application that uses Wi-Fi to download live ADS-B aircraft data and plot nearby airplanes over satellite imagery. It works on homebrew-capable Vita systems, including Vita Slim models without GPS or 3G.

The project started as a circular radar experiment and evolved into a touch-controlled satellite flight map.

## Compatibility notice

> **PS Vita 1000 / FAT users:** v1.9.2 was developed and hardware-tested primarily on a PS Vita 2000 Slim. A first public hardware report indicates that some PS Vita 1000 systems may launch the app and accept coordinates correctly but then show a black satellite-map area with only the tracking-center marker visible. This issue is under active investigation.
>
> **PS Vita 2000 Slim users:** v1.9.2 is currently the confirmed working hardware target.
>
> A compatibility update is in development with the goal of supporting both PS Vita 1000 and PS Vita 2000 models reliably. If you are using a Vita 1000 and encounter this issue, please include your Vita model, storage setup (official memory card / SD2Vita), and whether aircraft icons appear when reporting it.

## Download

### **[Download VitaFlightRadar v1.9.2 VPK](releases/VitaFlightRadar-v1.9.2.vpk)**

**v1.9.2 is the current recommended build for PS Vita 2000 Slim hardware. PS Vita 1000 users should read the compatibility notice above.** See [RELEASES.md](RELEASES.md) and [CHANGELOG.md](CHANGELOG.md) for the complete version history.

## v1.9.2: High-Resolution Satellite Hotfix

v1.9.2 keeps the stable v1.9.1/v1.8 satellite-map renderer and targets the remaining close-zoom blur/pixelation issue without reintroducing the experimental v1.9 async map pipeline.

- Raises the app's satellite detail ceiling from zoom level 19 to zoom level 21.
- Adds a close-zoom **quality cushion**: after a pinch, the renderer requests one complete tile level sharper than the minimum required whenever possible.
- The extra-detail tile is then downsampled to the Vita screen instead of stretching a lower-resolution tile upward.
- Keeps lower-detail imagery visible as a fallback until the sharper tile arrives, so the map should not turn black while sharpening.
- Expands the in-memory map texture cache from 32 to 40 tiles so higher-resolution imagery can remain resident longer.
- Preserves the proven v1.8/v1.9.1 slippy-map architecture, one-finger pan, touch-and-hold recentering and pinch-controlled aircraft radius.
- Uses safe Release compiler optimization (`-O3`) with no forced CPU/GPU clocks or overclock requirement.

Satellite source imagery varies by location, so increasing tile zoom cannot invent detail that does not exist in the original imagery. v1.9.2 is designed to avoid unnecessary software-side stretching and use the sharpest available tile level more aggressively.

## Version notes

### v1.9.1

Stable satellite-map hotfix. It removed the experimental v1.9 asynchronous map pipeline and restored the proven v1.8 renderer after real-hardware testing showed that v1.9 could display planes over a black background.

### v1.9

**Not recommended.** Preserved for development history. The experimental async tile pipeline could fail to deliver satellite imagery to the renderer on real PS Vita hardware.

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

1. Download the latest `.vpk` using the link above.
2. Open **VitaShell** on the Vita.
3. Connect the Vita to your PC using VitaShell **USB** or **FTP** mode.
4. Copy the VPK to a convenient folder such as `ux0:/data/`.
5. In VitaShell, navigate to the VPK.
6. Press **X** on the file and choose **Install**.
7. Return to the Vita home screen and launch **VitaFlightRadar**.

If an older build refuses to update cleanly, delete the old VitaFlightRadar bubble and install the latest VPK fresh.

## How it works

The Vita itself is **not an ADS-B radio receiver**. VitaFlightRadar connects through Wi-Fi and requests live nearby aircraft data from the **adsb.fi** open-data API, then plots aircraft relative to the geographic map center currently being viewed.

Satellite imagery is rendered as cached 256x256 map tiles. v1.9.2 keeps the stable multi-tile renderer and requests higher-detail imagery more aggressively at close zoom levels to reduce software-side blur.

Flight route and timetable information is obtained separately because raw ADS-B position data does not reliably contain origin, destination or airline schedule fields. Public aviation data is incomplete, so private or unusual flights can still have missing route/timetable information.

Known helicopters, rotorcraft, drones, balloons, gliders and other known non-airplane ADS-B categories are filtered out.

## Version history

See [RELEASES.md](RELEASES.md) for the release index and [CHANGELOG.md](CHANGELOG.md) for detailed notes from the original proof of concept through v1.9.2.

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

Real PS Vita hardware testing has been a major part of the development process.

## Development

VitaFlightRadar is written in C for **VitaSDK** and uses Vita2D for rendering. GitHub Actions builds and validates the installable VPK.

The project includes Vita-native networking, HTTPS handling, map caching, static-library relocation fixes, Vita-safe icon/VPK packaging and LiveArea validation.

## Disclaimer

VitaFlightRadar is an unofficial hobby/homebrew project and is not affiliated with Sony, FlightRadar24, adsb.fi, Esri, OpenStreetMap, airlines, airports or aircraft manufacturers.

Do not use this application for navigation, air-traffic control, safety-critical decisions or operational aviation purposes.
