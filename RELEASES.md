# VitaFlightRadar Releases

This page is the release index for VitaFlightRadar.

## Latest release

### v1.9.2

**Current / Recommended**

[Download VitaFlightRadar v1.9.2 VPK](releases/VitaFlightRadar-v1.9.2.vpk)

Highlights:

- High-resolution satellite hotfix built on the stable v1.9.1/v1.8 map renderer
- Satellite detail ceiling increased from zoom level 19 to zoom level 21
- Close-zoom quality cushion requests one complete tile level sharper than the minimum whenever possible
- Sharper imagery is downsampled instead of stretching lower-resolution imagery upward
- Previous lower-detail tiles remain visible while sharper tiles load
- In-memory map texture cache increased from 32 to 40 tiles
- One-finger pan, touch-and-hold recentering, pinch zoom and automatic aircraft-radius behavior retained
- Coordinate search, AUTO/MANUAL refresh, route information and aircraft filtering retained
- Safe Release compiler optimization with no forced CPU/GPU clock settings
- Public build contains no creator-specific startup coordinates

## Historical versions

Detailed patch notes are in [CHANGELOG.md](CHANGELOG.md).

| Version | Status | Major milestone |
| --- | --- | --- |
| **v1.9.2** | **Current / Recommended** | Higher-detail satellite zoom, close-zoom supersampling/quality cushion, larger texture cache |
| **v1.9.1** | Historical / Stable map hotfix | Restored proven v1.8 renderer after v1.9 black-map regression |
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
