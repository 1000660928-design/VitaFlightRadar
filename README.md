# VitaFlightRadar ✈️

**Live aircraft tracking on a PlayStation Vita.**

VitaFlightRadar is a PS Vita homebrew application that uses Wi-Fi to download live ADS-B aircraft data and plot nearby airplanes over satellite imagery. The project started as a circular radar experiment and evolved into a touch-controlled satellite flight map with panning, pinch zoom, coordinate search, aircraft selection and route information.

## Download

**Current stable release: VitaFlightRadar v2.0**

### **[Download VitaFlightRadar v2.0](releases/VitaFlightRadar-v2.0-Slim.vpk)**

V2.0 is the current recommended public build and the last version confirmed through real Vita testing to have the stable, responsive behavior we want.

### **[Download VitaFlightRadar v1.9.3 compatibility build](releases/VitaFlightRadar-v1.9.3-PCH1000.vpk)**

> [!IMPORTANT]
> **Both V2.0 and V1.9.3 can be used on PS Vita PCH-1000 FAT/OLED and PCH-2000 Slim systems.**
>
> **Start with V2.0. If V2.0 gives you problems on your Vita, especially on a PCH-1000 FAT/OLED system, try the V1.9.3 compatibility build.** V1.9.3 is kept available specifically as the safer fallback for systems that have trouble with the newer build.

The two downloads are not separate "Slim-only" and "FAT-only" applications. They are different software versions of VitaFlightRadar. V1.9.3 contains compatibility-focused networking and map changes and remains available for users who need them.

See [RELEASES.md](RELEASES.md) for the release index and [CHANGELOG.md](CHANGELOG.md) for the complete development history.

## Current stable feature set

- Live nearby aircraft positions over satellite imagery.
- One-finger map panning, touch-and-hold recentering and two-finger pinch zoom.
- Tap aircraft to select them, or use UP / DOWN to cycle through aircraft.
- **Triangle Search Hub** with coordinate search and flight-number search.
- Search menus work with the **front touchscreen** or **D-pad + X**.
- Coordinate search remembers the **three most recent coordinates** across launches.
- Flight-number/callsign search accepts normalized entries such as `ELY5230`, jumps to a live aircraft when found and begins FOLLOW mode.
- **FOLLOW mode** keeps the map centered on the searched aircraft as new live positions arrive.
- Aircraft data refresh is **always automatic every 4 seconds**.
- Selected-flight information includes aircraft metrics plus route/timetable data when public data is available.
- Known helicopters, rotorcraft, drones, balloons, gliders and other known non-airplane ADS-B categories are filtered out.
- Persistent satellite tile caching and the existing satellite-map renderer.

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
| **Square** | Unused in v2.0 |
| **START** | Unused in v2.0 |
| **PS button** | Leave / suspend through the Vita system UI |

There is intentionally no separate hardware zoom or aircraft-radius control. Map zoom and aircraft search radius work together through the touch screen.

## Moving around the map

Drag with one finger to move the map. When the drag ends, the center of the visible map becomes the new aircraft tracking point. Hold one finger still to recenter directly on that geographic point. Pinch with two fingers to zoom in or out.

The map wraps horizontally around the Earth. The public nearby-aircraft API still has a finite point/radius search limit, so a very zoomed-out view does not mean the app can request every aircraft on Earth simultaneously.

## Automatic refresh and flight following

V2.0 has no AUTO/MANUAL modes. Live aircraft refresh is always enabled and runs approximately every **4 seconds**.

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

Spaces and letter case are normalized automatically. If the flight is found in the live aircraft feed, the map jumps to the aircraft, selects it and enters FOLLOW mode.

Both search menus can be controlled with the **front touchscreen** or **D-pad + X**. Use **Circle** to go back.

## Installing on a PS Vita

You need a homebrew-enabled PS Vita with **VitaShell** installed.

1. Start with the V2.0 VPK. If it gives your system compatibility problems, try the V1.9.3 compatibility build.
2. Open **VitaShell** on the Vita.
3. Connect the Vita to your PC using VitaShell **USB** or **FTP** mode.
4. Copy the VPK to a convenient folder such as `ux0:/data/`.
5. In VitaShell, navigate to the VPK.
6. Press **X** on the file and choose **Install**.
7. Return to the Vita home screen and launch **VitaFlightRadar**.

If an older build refuses to update cleanly, delete the old VitaFlightRadar bubble and install the chosen VPK fresh.

## How it works

The Vita itself is **not an ADS-B radio receiver**. VitaFlightRadar connects through Wi-Fi and requests live aircraft data from public ADS-B services, then plots aircraft relative to the geographic map center being viewed.

Satellite imagery is rendered as cached 256x256 map tiles. The current renderer aggressively requests higher-detail imagery at close zoom levels to reduce software-side blur while keeping lower-detail tiles available as a fallback.

Flight route and timetable information is obtained separately because raw ADS-B position data does not reliably contain origin, destination or airline schedule fields. Public aviation data is incomplete, so private or unusual flights can still have missing route/timetable information.

## Compatibility

VitaFlightRadar uses one application codebase for PS Vita PCH-1000 FAT/OLED and PCH-2000 Slim systems.

- **V2.0:** current recommended stable public build for both PCH-1000 and PCH-2000.
- **V1.9.3:** compatibility fallback for both models, especially useful if V2.0 has trouble on a PCH-1000 FAT/OLED system.

If V2.0 works correctly on your Vita, use it. If it does not, install V1.9.3 and report the issue so it can be investigated in the next release.

## Version history

See [RELEASES.md](RELEASES.md) for the release index and [CHANGELOG.md](CHANGELOG.md) for detailed notes from the original proof of concept through the current builds.

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

Only tested public releases are documented on this page. V2.0 remains the recommended release until a newer version is ready for public use.

## Disclaimer

VitaFlightRadar is an unofficial hobby/homebrew project and is not affiliated with Sony, FlightRadar24, adsb.fi, Esri, OpenStreetMap, airlines, airports or aircraft manufacturers.

Do not use this application for navigation, air-traffic control, safety-critical decisions or operational aviation purposes.
