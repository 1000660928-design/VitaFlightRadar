# VitaFlightRadar ✈️

**Live aircraft tracking on PlayStation Vita.**

VitaFlightRadar is a homebrew flight-tracking application for PS Vita. It uses Wi-Fi to retrieve live aircraft data and displays flights over an interactive satellite map with touch controls, search, route information and live tracking.

> [!NOTE]
> **PCH-1000 FAT/OLED and PCH-2000 Slim use the same VitaFlightRadar application.** The downloads below are different software versions, not model-exclusive builds.

## 📥 Downloads

| Version | Status | Recommended use |
| --- | --- | --- |
| **V2.5** | 🟢 **Latest / Recommended** | Start here. This is the newest real-hardware-tested public release. |
| **V2.0** | 🛟 **Stable fallback** | Use this if V2.5 causes problems on your Vita. |
| **V1.9.3** | 🧰 **Compatibility fallback** | Try this if newer versions still have compatibility trouble. |

### 🟢 [Download VitaFlightRadar V2.5](releases/VitaFlightRadar-v2.5.vpk)

### 🛟 [Download VitaFlightRadar V2.0](releases/VitaFlightRadar-v2.0.vpk)

### 🧰 [Download VitaFlightRadar V1.9.3 Compatibility Build](releases/VitaFlightRadar-v1.9.3-Compatibility.vpk)

> [!IMPORTANT]
> **Install V2.5 first.** If V2.5 does not behave correctly on your Vita, use **V2.0**. If you still have compatibility problems, try **V1.9.3**.
>
> V2.0 remains public intentionally so users always have a proven older version to return to.

## ✨ V2.5 highlights

- **Remembers your last map position and zoom** between launches.
- **TRACK FLIGHT** button for explicitly following a selected aircraft.
- Tracking stays locked while zooming instead of dropping the selected aircraft.
- A temporary missed aircraft refresh keeps the **last known tracked position** visible while retrying.
- Previously valid route/timetable information is protected from temporary failed refreshes.
- The compass now shows the selected aircraft's **real heading**.
- More resilient flight search using multiple public live-data paths and equivalent identifier forms.
- After a global flight match, the app focuses on that aircraft and loads a **small nearby traffic area** instead of trying to render worldwide traffic.
- Slow search/network work is kept away from the main interaction path wherever possible.
- Improved satellite-map loading, cache behavior, color consistency and zoom continuity.
- Larger in-memory map tile cache and sharper center-tile priority.
- Live aircraft refresh remains approximately every **4 seconds**.

## 🗺️ Main features

- Live aircraft over satellite imagery.
- One-finger map panning.
- Two-finger pinch zoom.
- Touch-and-hold recentering.
- Tap an aircraft to select it.
- UP / DOWN aircraft cycling.
- **Triangle Search Hub**.
- Coordinate search with the **three most recent searches saved**.
- Flight-number / callsign search.
- Live flight tracking.
- Altitude, speed, heading and distance.
- Origin, destination and timetable data when public data is available.
- Automatic filtering of known helicopters, drones, balloons, gliders and other non-airplane ADS-B categories.
- Persistent satellite tile caching.
- Last-view persistence across app restarts.

## 🎮 Controls

| Control | Action |
| --- | --- |
| **One-finger drag** | Pan the map |
| **Stationary touch-and-hold** | Recenter on that point |
| **Two-finger pinch** | Zoom in / out |
| **Tap aircraft** | Select aircraft |
| **TRACK FLIGHT button** | Start / stop following the selected aircraft |
| **Triangle** | Open Search Hub |
| **UP / DOWN** | Navigate menus or cycle aircraft |
| **X** | Confirm / Enter |
| **Circle** | Cancel / Back |
| **PS button** | Suspend / leave through the Vita system UI |

## 🔎 Search

Press **Triangle** to open the Search Hub.

### Coordinate search

Enter decimal coordinates such as:

```text
40.7128,-74.0060
```

The app jumps to the selected location and remembers your three most recent coordinate searches.

### Flight search

Enter a live callsign or flight number, for example:

```text
ELY5230
AIZ994
NBT50K
```

Spaces and letter case are normalized. VitaFlightRadar tries the available public live-flight sources and moves the map to the aircraft when a live match is found.

> [!WARNING]
> No public flight-tracking source guarantees coverage of every aircraft at every moment. A flight may exist on a commercial tracker while temporarily being absent from the public sources available to VitaFlightRadar.

## 🧭 Tracking

When **TRACK FLIGHT** is enabled:

- The selected aircraft remains the tracking target.
- The map follows its newest reported position.
- You can zoom while tracking.
- Temporary missed refreshes keep the last known target visible while the app retries.
- Existing route/timetable information stays visible instead of immediately changing to unavailable.
- The compass displays the aircraft's heading.

## 📡 How it works

The PS Vita is **not an ADS-B receiver**. VitaFlightRadar connects over Wi-Fi to public aircraft-data services and renders the aircraft positions they provide.

Satellite imagery is downloaded as map tiles and cached locally. First-time loading in a new area may take longer than revisiting a cached area.

Route and timetable information comes from separate public aviation sources. Those sources can be incomplete or temporarily unavailable, so some flights may show partial information even when their live position is visible.

## 🎮 Compatibility

VitaFlightRadar is intended for both:

- **PS Vita PCH-1000 FAT/OLED**
- **PS Vita PCH-2000 Slim**

There is no requirement to use a separate application solely because of your Vita model.

### Which version should I use?

1. **V2.5** - recommended for everyone.
2. **V2.0** - stable fallback if V2.5 has issues.
3. **V1.9.3** - compatibility fallback if newer versions still do not behave correctly.

## 🛠️ Installation

You need a homebrew-enabled PS Vita with **VitaShell**.

1. Download the VPK you want to use.
2. Transfer it to the Vita using VitaShell USB or FTP.
3. Navigate to the VPK in VitaShell.
4. Press **X** and install it.
5. Return to LiveArea and launch VitaFlightRadar.

If an update behaves strangely, remove the old VitaFlightRadar bubble and perform a clean installation.

## 🆘 Troubleshooting

**V2.5 does not work correctly**  
Try V2.0. If the issue remains, try V1.9.3 and report what happened.

**A flight cannot be found**  
Try the callsign or commercial flight number shown by your flight-tracking source. Live availability still depends on at least one accessible public source reporting the aircraft.

**Route / departure / arrival is unavailable**  
Position data and route/schedule data come from different sources. A plane can be visible while its public route or timetable data is missing.

**The map takes time to sharpen**  
New satellite tiles must be downloaded over Wi-Fi. Already-cached areas should load faster on later visits.

## 📋 Release policy

Only builds considered ready for normal users are promoted on this page. New releases are added after build validation and real-hardware testing.

See [RELEASES.md](RELEASES.md) for the download summary and [CHANGELOG.md](CHANGELOG.md) for public release notes.

## 🙌 Credits

### Creator

**George Ultra**

Created by George Ultra with help from **ChatGPT / OpenAI** for software development, debugging, VitaSDK integration, UI iteration, research and build automation.

### Technical / Emotional Support

- **Ventiz**
- **YahliKap**

## 💬 Feedback and bug reports

Contact **George Ultra** on Reddit:

https://www.reddit.com/user/Diligent_Peace_1618/

When reporting a bug, please include the Vita model, VitaFlightRadar version, searched flight/callsign and what happened.

## ⚠️ Disclaimer

VitaFlightRadar is an unofficial hobby/homebrew project and is not affiliated with Sony, FlightRadar24, adsb.fi, OpenSky, ADSB.lol, ADSB One, Esri, airlines, airports or aircraft manufacturers.

Live aircraft positions, routes, times and satellite imagery may be delayed, incomplete, unavailable or inaccurate.

**Do not use VitaFlightRadar for navigation, air-traffic control, flight safety or any other safety-critical aviation purpose.**
