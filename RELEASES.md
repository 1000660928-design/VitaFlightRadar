# VitaFlightRadar Releases

This page is the release index for VitaFlightRadar.

## Current model-specific builds

Choose the build that matches your PS Vita.

### PS Vita 2000 Slim — PCH-20xx

**Current hardware-tested Slim build: v1.9.2**

[Download VitaFlightRadar v1.9.2 — PCH-2000 Slim](releases/VitaFlightRadar-v1.9.2.vpk)

Highlights:

- Confirmed through repeated real-hardware testing on a PCH-2000 Slim
- Stable v1.8-derived slippy satellite renderer
- Satellite detail ceiling up to zoom level 21
- Close-zoom higher-detail tile cushion and downsampling
- 40-tile in-memory texture cache
- One-finger pan, touch-and-hold recentering and pinch-controlled aircraft radius
- Coordinate search, AUTO/MANUAL refresh, route information and aircraft filtering
- No forced CPU/GPU clock settings

### PS Vita 1000 FAT / OLED — PCH-10xx / PCH-11xx

**Dedicated compatibility build: v1.9.3-PCH1000**

[Download VitaFlightRadar v1.9.3 — PCH-1000 FAT/OLED](releases/VitaFlightRadar-v1.9.3-PCH1000.vpk)

Highlights:

- Same VitaFlightRadar UI and feature set as v1.9.2
- VitaSDK libcurl with mbedTLS-backed HTTPS/TLS for firmware-independent networking
- Explicit application/map-cache directory creation
- Automatic lower-resolution satellite fallback when an HQ tile is unavailable
- Retains v1.9.2 zoom-21 satellite sharpening and 40-tile texture cache
- Uses the stable renderer path, not the v1.9 experimental async map pipeline
- Passed VitaSDK compile, link, VPK and package-integrity validation
- Public build contains no creator-specific startup coordinates

**Testing status:** this is the dedicated PCH-1000 build produced in response to the first FAT/OLED black-map report. Wider real PCH-1000 hardware confirmation is still requested.

## Historical versions

Detailed patch notes are in [CHANGELOG.md](CHANGELOG.md).

| Version | Status | Major milestone |
| --- | --- | --- |
| **v1.9.3-PCH1000** | **Current PCH-1000 compatibility build** | mbedTLS networking, cache-directory hardening, satellite fallback for FAT/OLED |
| **v1.9.2** | **Current PCH-2000 Slim build** | Higher-detail satellite zoom, close-zoom quality cushion, larger texture cache |
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
