# VitaFlightRadar ✈️

**Live aircraft tracking on a PlayStation Vita.**

VitaFlightRadar is a PS Vita homebrew application that uses Wi-Fi to download live ADS-B aircraft data and plot nearby airplanes over satellite imagery. The project started as a circular radar experiment and evolved into a touch-controlled satellite flight map with panning, pinch zoom, coordinate search, aircraft selection and route information.

## Download

### PS Vita 2000 Slim — PCH-20xx

**Current feature release: VitaFlightRadar v2.2 Slim**

### **[Download VitaFlightRadar v2.2 — PCH-2000 Slim](releases/VitaFlightRadar-v2.2-Slim.vpk)**

V2.2 is the current PS Vita Slim / PCH-2000 build. It expands live-flight coverage across multiple ADS-B feeds, accepts equivalent flight-number/callsign formats, improves route and time enrichment with multiple fallbacks, and adds an explicit on-screen TRACK FLIGHT button so following is controlled by the user.

### PS Vita 1000 FAT / OLED — PCH-10xx / PCH-11xx

**Legacy compatibility build: v1.9.3-PCH1000**

### **[Download VitaFlightRadar v1.9.3 — PCH-1000 FAT/OLED](releases/VitaFlightRadar-v1.9.3-PCH1000.vpk)**

The FAT/OLED edition is currently frozen at v1.9.3 while feature development focuses exclusively on the PCH-2000 Slim. After the Slim feature set is complete and thoroughly tested, the finished application can be ported back to the PCH-1000 compatibility path.

See [RELEASES.md](RELEASES.md) for the release index and [CHANGELOG.md](CHANGELOG.md) for the complete development history.

## V2.2 live coverage, route data and tracking

- Live radar data is merged from adsb.fi and ADSB.lol instead of relying on one feed.
- Flight search tries multiple live providers and equivalent callsign forms, including leading-zero variants.
- Tracked flights receive a direct callsign recovery attempt if they temporarily disappear from the nearby feed.
- Route information is merged from multiple public aviation sources instead of being erased when one source fails.
- Schedule lookup retries using the IATA flight number learned from route data.
- HexDB is available as an additional route fallback.
- Incomplete route/schedule enrichment retries automatically.
- Added an on-screen **TRACK FLIGHT** target button.
- Tap the button once to follow the selected aircraft and again to stop.
- While tracking is on, the map recenters on the aircraft as fresh positions arrive.
- If a public scheduled time is unavailable but a route is known, clearly labeled UTC live estimates can fill the time panel instead of leaving every field blank.

## V2.1 performance update

- Faster perceived startup: the interface appears before live aircraft and route requests finish.
- Reuses network/DNS/TLS state between requests.
- Shorter network timeouts so unavailable services fail quickly instead of freezing the app.
- Satellite tiles prioritize the visible area and avoid long fallback-download chains.
- Flight-number search shows the aircraft before optional route/timetable details finish.
- Flight search requires an exact normalized callsign match.
- Every fresh launch resets to the default 5 km view.
- Found flights automatically focus to a 10 km view while keeping nearby traffic visible.

## Current feature set

- Live nearby aircraft positions over satellite imagery.
- One-finger map panning, touch-and-hold recentering and two-finger pinch zoom.
- Tap aircraft to select them, or use UP / DOWN to cycle through aircraft.
- **Triangle Search Hub** with coordinate search and flight-number search.
- Search menus work with the **front touchscreen** or **D-pad + X**.
- Coordinate search remembers the **three most recent coordinates** across launches.
- Flight-number/callsign search accepts entries such as `ELY5230`, tries equivalent callsign formats across multiple live feeds, finds the aircraft and moves the map to it.
- **TRACK FLIGHT button** explicitly turns map following on or off for the selected aircraft.
- **FOLLOW mode** keeps the map centered on the tracked aircraft as new live positions arrive.
- Aircraft data refresh is **always automatic every 4 seconds**.
- Selected-flight information includes altitude, speed, heading, distance and route/timetable data when public data is available.
- Known helicopters, rotorcraft, drones, balloons, gliders and other known non-airplane ADS-B categories are filtered out.
- Persistent satellite tile caching and high-resolution satellite sharpening at close zoom levels.

## Controls

| Control | Action |
| --- | --- |
| **One-finger drag** | Pan the satellite map; release to track aircraft around the new center |
| **Stationary touch-and-hold** | Recenter directly on the point under your finger |
| **Two-finger pinch** | Zoom the map and automatically change the live aircraft search radius |
| **Tap aircraft** | Select aircraft and leave flight-follow mode |
| **Triangle** | Open the Search Hub |
| **UP / DOWN** | Navigate menus or cycle aircraft |
| **X** | Confirm / Enter |
| **Circle** | Cancel / Back |
| **Square** | Unused in v2.2 |
| **START** | Unused in v2.2 |
| **PS button** | Leave / suspend through the Vita system UI |

There is intentionally no separate hardware zoom or aircraft-radius control. Map zoom and aircraft search radius work together through the touch screen.

## Moving around the map

Drag with one finger to move the map. When the drag ends, the center of the visible map becomes the new aircraft tracking point. Hold one finger still to recenter directly on that geographic point. Pinch with two fingers to zoom in or out.

The map wraps horizontally around the Earth. The public nearby-aircraft API still has a finite point/radius search limit, so a very zoomed-out view does not mean the app can request every aircraft on Earth simultaneously.

## Automatic refresh and flight following

V2.2 has no AUTO/MANUAL modes. Live aircraft refresh is always enabled and runs approximately every **4 seconds**.

When a flight is found through flight-number search, VitaFlightRadar enters **FOLLOW mode**. Each live refresh updates the aircraft and recenters the map on its newest reported position while its signal remains available.

## Search Hub

Press **Triangle** to open the Search Hub.

### Coordinate search

Choose **Enter Coordinates** and type decimal coordinates such as:

```text
40.7128,-74.0060
```

The app jumps to that location and saves it in the recent-search list. The latest **three** coordinate searches appear in the coordinate menu and can be selected without typing them again.

### Flight-number search

Choose **Search Flight Number** and enter a live callsign/flight number such as:

```text
ELY5230
```

Spaces and letter case are normalized automatically. If the flight is currently present in the live ADS-B feed, the map jumps to the aircraft, selects it and begins following its live position.

Both search menus can be controlled with the **front touchscreen** or **D-pad + X**. Use **Circle** to go back.

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

V2 feature development currently targets the **PCH-2000 Slim only**.

- **PCH-2000 Slim:** v2.2 is the current feature release.
- **PCH-1000 FAT/OLED:** v1.9.3-PCH1000 remains available as the existing compatibility build.

The PCH-1000 version will be revisited after the Slim application reaches the final feature set, so the finished Slim software can be carried over as one complete port rather than maintaining two moving targets during active development.

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

Current feature development is maintained on the `v200-build` branch for the PCH-2000 Slim. The PCH-1000 compatibility branch is intentionally paused until the Slim feature set is complete.

## Disclaimer

VitaFlightRadar is an unofficial hobby/homebrew project and is not affiliated with Sony, FlightRadar24, adsb.fi, Esri, OpenStreetMap, airlines, airports or aircraft manufacturers.

Do not use this application for navigation, air-traffic control, safety-critical decisions or operational aviation purposes.
