# VitaFlightRadar ✈️

**Live aircraft tracking on a PlayStation Vita.**

VitaFlightRadar is a PS Vita homebrew application that uses Wi-Fi to download live ADS-B aircraft data and plot nearby airplanes over satellite imagery. It works on homebrew-capable Vita systems, including Vita Slim models without GPS or 3G.

The project started as a circular radar experiment and evolved into a touch-controlled satellite flight map.

## Download

### **[Download VitaFlightRadar v1.9.1 VPK](releases/VitaFlightRadar-v1.9.1.vpk)**

**v1.9.1 is the current recommended build.** See [RELEASES.md](RELEASES.md) and [CHANGELOG.md](CHANGELOG.md) for the complete history.

## v1.9.1: Satellite Map Hotfix

v1.9.1 fixes a regression introduced in v1.9 where live aircraft still appeared but the satellite imagery could remain completely black.

To prioritize reliability, v1.9.1 restores the exact satellite-map renderer used by v1.8, which was confirmed working on real PS Vita hardware, while retaining safe Release compiler optimization.

- Restores the proven v1.8 multi-tile satellite map engine.
- Removes the experimental v1.9 asynchronous satellite-worker pipeline that caused the black-map regression.
- Keeps one-finger panning, touch-and-hold recentering, pinch zoom and automatic aircraft-radius behavior.
- Keeps coordinate search, aircraft filtering, AUTO/MANUAL refresh, route information and touch aircraft selection.
- Uses optimized Release compilation (`-O3`) without forced CPU/GPU clock changes.
- Keeps the public build privacy-safe with no creator-specific startup coordinates.

### v1.9 known issue

The original v1.9 performance release is preserved for development history, but it is **not recommended**. On real hardware it could display aircraft over a black background because the experimental asynchronous map pipeline failed to deliver satellite tiles to the renderer correctly.

## Map controls

| Control | Action |
| --- | --- |
| **One-finger drag** | Pan the satellite map; release to track aircraft around the new center |
| **Stationary touch-and-hold** | Recenter directly on the point under your finger |
| **Two-finger pinch** | Zoom map and automatically change the live aircraft search radius |
| **Tap aircraft** | Select aircraft |
| **Triangle** | Search / enter coordinates |
| **X** | Confirm / Enter in supported dialogs |
| **Circle** | Cancel / Back in supported dialogs |
| **Square** | Toggle AUTO / MANUAL aircraft refresh |
| **UP / DOWN** | Previous / next aircraft |
| **START** | Manual aircraft refresh |
| **PS button** | Leave / suspend through the Vita system UI |

There is intentionally no separate hardware map-zoom or aircraft-radius control. The visible map and live-aircraft radius move together with touch zoom.

## Moving around the world

Drag with one finger to move the map. When the drag ends, the center of the visible map becomes the new aircraft tracking point. Hold one finger still to recenter directly on that geographic point. Pinch with two fingers to zoom.

The map wraps horizontally around the Earth. The public adsb.fi nearby-aircraft endpoint has a finite point/radius search limit, so a very zoomed-out map does not mean VitaFlightRadar can request every aircraft on Earth at once.

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

1. Download the latest `.vpk` above.
2. Open VitaShell.
3. Connect the Vita to your PC using VitaShell USB or FTP mode.
4. Copy the VPK to a convenient folder such as `ux0:/data/`.
5. Navigate to the VPK in VitaShell.
6. Press **X** and choose **Install**.
7. Return to the Vita home screen and launch VitaFlightRadar.

If an older build refuses to update cleanly, delete the old VitaFlightRadar bubble and install the new VPK fresh.

## How it works

The Vita itself is **not an ADS-B radio receiver**. VitaFlightRadar uses Wi-Fi to request live nearby aircraft data from the **adsb.fi open-data API**, then plots aircraft relative to the map view.

Satellite imagery is rendered as cached map tiles. v1.9.1 uses the proven v1.8 slippy-map renderer after the experimental v1.9 asynchronous tile pipeline caused a real-hardware black-map regression.

Flight route and timetable information is obtained separately because raw ADS-B position data does not reliably contain origin, destination or airline schedule fields. Public aviation data is incomplete, so private/unusual flights can still have missing route or timetable information.

Known helicopters, rotorcraft, drones, balloons, gliders and other known non-airplane ADS-B categories are filtered out.

## Version history

See [RELEASES.md](RELEASES.md) for the release index and [CHANGELOG.md](CHANGELOG.md) for detailed notes from the original prototype through v1.9.1.

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
