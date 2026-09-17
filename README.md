# VitaFlightRadar ✈️

**Live aircraft tracking on a PlayStation Vita.**

VitaFlightRadar is a PS Vita homebrew application that uses the Vita's Wi-Fi connection to download live ADS-B aircraft data and plot nearby airplanes over satellite imagery. It is designed for the PS Vita Slim and other homebrew-capable Vita systems, including systems without GPS or 3G.

The project started as a simple circular radar experiment and has evolved into a touch-controlled satellite flight map.

## Download

### **[Download VitaFlightRadar v1.8 VPK](releases/VitaFlightRadar-v1.8.vpk)**

v1.8 is the current recommended build. See **[RELEASES.md](RELEASES.md)** and **[CHANGELOG.md](CHANGELOG.md)** for the complete version history and patch notes.

## Current version

**v1.8**

v1.8 upgrades the satellite renderer into a much more map-like experience:

- Live aircraft positions refreshed approximately every 4 seconds in AUTO mode
- Satellite imagery rendered as a multi-tile slippy map instead of one large replaceable image
- One-finger drag/pan across the map
- Releasing a drag makes the new map center the live aircraft tracking center
- Stationary touch-and-hold recenters directly on the geographic point under your finger
- Two-finger pinch zoom controls both map scale and the live aircraft search radius
- Zooming in narrows the aircraft area to what is visible
- Zooming out expands the aircraft area
- Horizontal world wrapping prevents a hard left/right map edge
- Cached lower-resolution imagery remains visible while sharper tiles arrive, reducing blank/cut-off regions
- Missing map tiles are streamed progressively instead of clearing and rebuilding the whole map at once
- Satellite tiles are cached on Vita storage so revisited areas can load faster
- Higher map zoom levels are used when zooming in so the map can sharpen again instead of permanently stretching a blurry image
- Touch gestures get a short priority/grace period before new map downloads begin, improving responsiveness during repeated panning or pinching
- Smaller UI text was adjusted for cleaner readability
- Tap an aircraft to inspect it
- UP / DOWN can also cycle through aircraft
- Coordinate search lets you jump directly to another location
- Known helicopters, rotorcraft, drones, balloons, gliders and other non-airplane ADS-B categories are filtered out
- Flight detail panel can show callsign, aircraft type, origin, destination, schedule/estimate information, altitude, speed, distance and heading when public data is available

## Controls

| Control | Action |
| --- | --- |
| **Touch: one-finger drag** | Pan the satellite map; release to track aircraft around the new map center |
| **Touch: stationary hold** | Recenter the map/tracking point on the location under your finger |
| **Touch: two-finger pinch** | Zoom the satellite map and automatically change the live aircraft search radius |
| **Touch: tap aircraft** | Select aircraft |
| **Triangle** | Search / enter coordinates |
| **X** | Confirm / Enter inside supported dialogs |
| **Circle** | Cancel / Back inside supported dialogs |
| **Square** | Toggle AUTO / MANUAL aircraft refresh |
| **UP / DOWN** | Previous / next aircraft |
| **START** | Refresh aircraft manually |
| **PS button** | Leave / suspend the app normally through the Vita system UI |

There is intentionally **no separate aircraft-radius button and no hardware zoom button**. The visible map and live-aircraft radius work together through touch gestures.

## Moving around the map

VitaFlightRadar v1.8 behaves more like a mobile map:

- Drag with one finger to move the map.
- Release after dragging and the map center becomes the new live tracking point.
- Hold one finger still on a point to recenter directly there.
- Pinch with two fingers to zoom in or out.
- Aircraft data automatically follows the geographic area you are viewing.

The map wraps horizontally around the Earth. Very wide views are still limited by the public ADS-B point/radius API: adsb.fi allows a maximum nearby-aircraft query radius of roughly **250 nautical miles / 460 km**. A very zoomed-out map therefore does not mean the app can download every aircraft on Earth simultaneously.

## AUTO vs MANUAL

**AUTO** refreshes live aircraft data approximately every **4 seconds**.

**MANUAL** freezes automatic aircraft-data refreshes. Press **START** whenever you want a fresh aircraft update.

## Coordinate search

Press **Triangle** and enter coordinates in decimal format. For example, New York City:

```text
40.7128,-74.0060
```

Then confirm with **X**. VitaFlightRadar jumps to that location, begins loading its satellite imagery and searches for nearby aircraft based on the current visible map area.

## Installing on a PS Vita

You need a homebrew-enabled PS Vita with **VitaShell** installed.

1. Download the latest `.vpk` using the download link above.
2. Open **VitaShell** on the Vita.
3. Connect VitaShell to your PC using **USB** or **FTP**.
4. Copy the `.vpk` to a convenient Vita folder such as `ux0:/data/`.
5. Disconnect USB/FTP if needed.
6. In VitaShell, navigate to the `.vpk`.
7. Press **X** on the file.
8. Choose **Install**.
9. Return to the Vita home screen and launch **VitaFlightRadar**.

If an older build refuses to update cleanly, delete the old VitaFlightRadar bubble and install the new VPK fresh.

## How flight tracking works

The Vita itself is **not an ADS-B radio receiver**. VitaFlightRadar connects through Wi-Fi and requests live nearby aircraft information from the **adsb.fi open-data API**. Aircraft positions are then plotted relative to the geographic map center currently being viewed.

Satellite imagery is loaded as map tiles and rendered directly by the Vita. v1.8 keeps an in-memory tile set plus a persistent tile cache under the app's Vita data folder, allowing cached imagery to be reused while missing/sharper tiles are loaded progressively.

Flight-route and schedule information is obtained separately because raw ADS-B position data does not reliably contain origin, destination, scheduled departure or arrival information.

Public aviation data is incomplete by nature. Private flights, unusual callsigns, military aircraft and flights without a public schedule may legitimately have missing route or timetable fields.

## Version history

See **[RELEASES.md](RELEASES.md)** for the release index and **[CHANGELOG.md](CHANGELOG.md)** for the detailed history from the first prototype through v1.8.

The repository also keeps historical build branches so development and older implementations remain inspectable.

## Project credits

### Creator

**George Ultra**  
Created by George Ultra with help from **ChatGPT / OpenAI** for software development, debugging, VitaSDK integration, UI iteration and build automation.

### Technical / Emotional Support

- **Ventiz**
- **YahliKap**

## Feedback, ideas and contact

Have an idea, bug report, review or feature request?

Contact **George Ultra** on Reddit:  
https://www.reddit.com/user/Diligent_Peace_1618/

Feedback from real PS Vita hardware testing has been a major part of how the project has developed.

## Development

VitaFlightRadar is written in C for **VitaSDK** and uses Vita2D for rendering. GitHub Actions builds the project inside a VitaSDK container and produces the installable VPK.

The source/build history contains several compatibility fixes specific to PS Vita homebrew, including Vita-native networking, HTTPS handling, static-library relocation fixes, Vita-safe PNG packaging, map-tile caching and LiveArea/VPK validation.

## Disclaimer

VitaFlightRadar is an unofficial hobby/homebrew project and is not affiliated with Sony, FlightRadar24, adsb.fi, Esri, OpenStreetMap, airlines, airports or aircraft manufacturers.

Do not use this application for navigation, air-traffic control, safety-critical decisions or operational aviation purposes.
