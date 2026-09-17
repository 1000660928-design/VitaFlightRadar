# VitaFlightRadar Releases

This page is the release index for VitaFlightRadar.

## Latest release

### v1.8

**Recommended version**

[Download VitaFlightRadar v1.8 VPK](releases/VitaFlightRadar-v1.8.vpk)

Highlights:

- New slippy-map satellite renderer
- One-finger map drag/pan
- Release after dragging to make the new map center the live aircraft tracking center
- Stationary touch-and-hold to recenter directly on a geographic point
- Pinch zoom remains the unified map zoom + aircraft search-radius control
- Progressive multi-tile loading instead of clearing/rebuilding the whole map
- In-memory + persistent Vita tile caching
- Cached lower-detail tile fallback while sharper imagery arrives
- Higher satellite detail levels for less blurry zoomed-in imagery
- Horizontal world wrapping to remove hard left/right map edges
- Improved touch responsiveness with a short grace period before tile network loads begin
- Cleaner small-text rendering
- Coordinate search, AUTO/MANUAL refresh, START refresh, tap-to-select and UP/DOWN aircraft selection retained

## Historical versions

Detailed patch notes for every version are available in [CHANGELOG.md](CHANGELOG.md).

| Version | Status | Major milestone |
| --- | --- | --- |
| **v1.8** | Current / Recommended | Slippy satellite map, drag/pan, recenter, progressive cached tiles, sharper zoom |
| **v1.7** | Historical | Unified touch pinch zoom + aircraft radius; higher-resolution tile rebasing |
| **v1.6** | Historical | Touch pinch zoom and world-view return improvements |
| **v1.5** | Historical | Coordinate search, Vita keyboard, world map, 5/10/15/20 km presets |
| **v1.4** | Historical | Satellite-map UI and airplane-only filtering direction |
| **v1.3** | Historical | Rich flight cards and geographic/route-data redesign |
| **v1.2.1** | Historical / Fixed installer | Corrected v1.2 package/icon issue |
| **v1.2** | Historical / Install issue | Map and route redesign; original package had an install problem |
| **v1.1** | Historical | Plane icons, 4-second refresh, compass, date/time and route panel |
| **v1.0.1** | Historical / Fixed installer | Corrected v1.0 icon/package issue |
| **v1.0** | Historical / First VPK | First installable live ADS-B radar prototype |
| **Early proof of concept** | Development history | Proved Wi-Fi-only PS Vita live ADS-B tracking was feasible |

Historical development/build branches remain in the repository so older implementations can still be inspected. The latest public VPK is kept directly in the repository for a stable download link.

For the complete change list, see [CHANGELOG.md](CHANGELOG.md).
