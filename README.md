# VitaFlightRadar ✈️

**Live aircraft tracking on a PlayStation Vita.**

VitaFlightRadar is a PS Vita homebrew application that uses the Vita's Wi-Fi connection to download live ADS-B aircraft data and plot nearby airplanes over satellite imagery. It is designed for the PS Vita Slim and other homebrew-capable Vita systems, including systems without GPS or 3G.

The project started as a simple circular radar experiment and has evolved into a touch-controlled satellite flight map.

## Download

### **[Download VitaFlightRadar v1.7 VPK](releases/VitaFlightRadar-v1.7.vpk)**

v1.7 is the current recommended build. See **[RELEASES.md](RELEASES.md)** and **[CHANGELOG.md](CHANGELOG.md)** for the complete version history and patch notes.

## Current version

**v1.7**

The latest version focuses on a cleaner mobile-style map experience:

- Live aircraft positions refreshed every 4 seconds in AUTO mode
- Satellite imagery
- Two-finger pinch zoom only
- Zoom level and aircraft search radius are now one unified system
- Zooming in shows only aircraft inside the tighter visible area
- Zooming out expands the live aircraft search area
- High-resolution satellite tiles are automatically reloaded after a pinch so the map does not stay blurry/pixelated
- Tap an aircraft to inspect it
- UP / DOWN can also cycle through aircraft
- Coordinate search lets you jump to another location anywhere in the world
- Known helicopters, rotorcraft, drones, balloons, gliders and other non-airplane ADS-B categories are filtered out
- Flight detail panel can show callsign, aircraft type, origin, destination, schedule/estimate information, altitude, speed, distance and heading when public data is available

## Controls

| Control | Action |
| --- | --- |
| **Touch: two-finger pinch** | Zoom satellite map and automatically change the live aircraft search radius |
| **Touch: tap aircraft** | Select aircraft |
| **Triangle** | Search / enter coordinates |
| **X** | Confirm / Enter inside supported dialogs |
| **Circle** | Cancel / Back inside supported dialogs |
| **Square** | Toggle AUTO / MANUAL aircraft refresh |
| **UP / DOWN** | Previous / next aircraft |
| **START** | Refresh aircraft manually |
| **PS button** | Leave / suspend the app normally through the Vita system UI |

There is intentionally **no separate radius button and no separate hardware zoom button in v1.7**. The visible map and live-aircraft radius are controlled together through pinch zoom.

## AUTO vs MANUAL

**AUTO** refreshes live aircraft data approximately every **4 seconds**.

**MANUAL** freezes automatic aircraft-data refreshes. Press **START** whenever you want a fresh aircraft update.

## Coordinate search

Press **Triangle** and enter coordinates in decimal format. For example, JFK Airport:

```text
40.6413,-73.7781
```

Then confirm with **X**. VitaFlightRadar jumps to that location, loads its satellite imagery and searches for nearby aircraft based on the current visible map area.

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

The Vita itself is **not an ADS-B radio receiver**. VitaFlightRadar connects through Wi-Fi and requests live nearby aircraft information from the **adsb.fi open-data API**. Aircraft positions are then plotted relative to the coordinates currently being viewed.

Satellite imagery is loaded as map tiles and rendered directly by the Vita. Flight-route and schedule information is obtained separately because raw ADS-B position data does not reliably contain origin, destination, scheduled departure or arrival information.

Public aviation data is incomplete by nature. Private flights, unusual callsigns, military aircraft and flights without a public schedule may legitimately have missing route or timetable fields.

## Version history

See **[RELEASES.md](RELEASES.md)** for the release index and **[CHANGELOG.md](CHANGELOG.md)** for the detailed history from the first prototype through v1.7.

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

The source/build history contains several compatibility fixes specific to PS Vita homebrew, including Vita-native networking, HTTPS handling, static-library relocation fixes, Vita-safe PNG packaging and LiveArea/VPK validation.

## Disclaimer

VitaFlightRadar is an unofficial hobby/homebrew project and is not affiliated with Sony, FlightRadar24, adsb.fi, Esri, OpenStreetMap, airlines, airports or aircraft manufacturers.

Do not use this application for navigation, air-traffic control, safety-critical decisions or operational aviation purposes.
