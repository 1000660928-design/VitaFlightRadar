# VitaFlightRadar ✈️

**Live aircraft tracking on a PlayStation Vita.**

VitaFlightRadar is a PS Vita homebrew application that uses Wi-Fi to download live ADS-B aircraft data and plot nearby airplanes over satellite imagery. It works on homebrew-capable Vita systems, including Vita Slim models without GPS or 3G.

The project started as a circular radar experiment and evolved into a touch-controlled satellite flight map.

## Download

### **[Download VitaFlightRadar v1.9 VPK](releases/VitaFlightRadar-v1.9.vpk)**

**v1.9 is the current recommended build.** See [RELEASES.md](RELEASES.md) and [CHANGELOG.md](CHANGELOG.md) for the complete history.

## v1.9: Maximum Performance

v1.9 keeps the v1.8 interface and focuses almost entirely on responsiveness, loading speed and reducing freezes:

- Satellite networking moved off the render/UI thread.
- Two background map workers download tiles in parallel.
- Persistent HTTP sessions reuse network connections instead of repeating connection setup for every tile.
- Center and visible tiles are prioritized before surrounding tiles.
- Neighboring/parent tiles are prefetched to make upcoming pans and zooms more likely to hit cache.
- In-memory texture cache increased to 48 map tiles with LRU-style reuse.
- Disk cache reads/writes happen away from the frame loop.
- Old/cached imagery remains visible while sharper imagery arrives.
- Texture decoding/upload work is budgeted so the renderer is not flooded by many new tiles in one frame.
- Aircraft and route-data refreshes run in a separate background worker instead of blocking touch/map rendering.
- Fast repeated gestures can invalidate stale map requests so the app does not waste time loading an area you already left.
- Config writes are debounced so dragging/pinching does not constantly write settings to storage.
- Touch gestures receive priority before background tile work resumes.
- Release build uses aggressive compiler optimization (`-O3`) and dead-code/data section removal.
- No forced CPU/GPU clock changes or overclock dependency. The performance work is architectural and is intended to preserve stability and battery/thermal behavior.

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

Satellite imagery is rendered as cached map tiles. v1.9 adds parallel background downloading, connection reuse, priority scheduling and a larger in-memory texture cache on top of the v1.8 slippy-map engine.

Flight route and timetable information is obtained separately because raw ADS-B position data does not reliably contain origin, destination or airline schedule fields. Public aviation data is incomplete, so private/unusual flights can still have missing route or timetable information.

Known helicopters, rotorcraft, drones, balloons, gliders and other known non-airplane ADS-B categories are filtered out.

## Version history

See [RELEASES.md](RELEASES.md) for the release index and [CHANGELOG.md](CHANGELOG.md) for detailed notes from the original prototype through v1.9.

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

The project includes Vita-native networking, HTTPS handling, map caching, asynchronous worker threads, static-library relocation fixes, Vita-safe icon/VPK packaging and LiveArea validation.

## Disclaimer

VitaFlightRadar is an unofficial hobby/homebrew project and is not affiliated with Sony, FlightRadar24, adsb.fi, Esri, OpenStreetMap, airlines, airports or aircraft manufacturers.

Do not use this application for navigation, air-traffic control, safety-critical decisions or operational aviation purposes.
