# VitaFlightRadar Releases

This page is the release index for VitaFlightRadar.

## Latest release

### v1.9.1

**Current / Recommended**

[Download VitaFlightRadar v1.9.1 VPK](releases/VitaFlightRadar-v1.9.1.vpk)

Highlights:

- Fixes the v1.9 black satellite-map regression seen on real PS Vita hardware
- Restores the exact proven v1.8 satellite-map renderer
- Keeps one-finger pan, touch-and-hold recenter, pinch zoom and automatic aircraft-radius behavior
- Keeps coordinate search, AUTO/MANUAL refresh, route information and aircraft filtering
- Safe Release compiler optimization with `-O3`
- No forced CPU/GPU clock settings or overclock requirement
- Public build contains no creator-specific startup coordinates

## Historical versions

Detailed patch notes are in [CHANGELOG.md](CHANGELOG.md).

| Version | Status | Major milestone |
| --- | --- | --- |
| **v1.9.1** | **Current / Recommended** | Satellite-map hotfix; restores proven v1.8 renderer with safe optimized build |
| **v1.9** | **Historical / Known map regression** | Experimental async performance engine; aircraft worked but satellite map could remain black |
| **v1.8** | Historical / Stable map base | Slippy satellite map, drag/pan, recenter, progressive cached tiles, sharper zoom |
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

Historical development/build branches remain in the repository so older implementations can still be inspected. Current public VPKs are kept directly in the repository for stable download links.

For the complete change list, see [CHANGELOG.md](CHANGELOG.md).
