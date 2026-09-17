# VitaFlightRadar Releases

This page is the release index for VitaFlightRadar.

## Latest release

### v1.9

**Current / Recommended**

[Download VitaFlightRadar v1.9 VPK](releases/VitaFlightRadar-v1.9.vpk)

Highlights:

- Maximum-performance release built on the v1.8 map experience
- Two background satellite-tile download workers
- Persistent HTTP connection reuse for map traffic
- Center-first tile priority scheduling
- Neighbor/parent tile prefetching
- 48-tile in-memory texture cache plus persistent Vita storage cache
- Disk/network map work moved off the render thread
- Controlled texture decode/upload budget to reduce frame spikes
- Background aircraft and route-data refresh worker
- Stale map requests dropped after fast pans/zooms
- Debounced settings writes during gestures
- Release compiler optimization with `-O3` and linker dead-section removal
- No required overclock or forced CPU/GPU clock settings
- All v1.8 touch-map features and controls retained

## Historical versions

Detailed patch notes are in [CHANGELOG.md](CHANGELOG.md).

| Version | Status | Major milestone |
| --- | --- | --- |
| **v1.9** | **Current / Recommended** | Async performance engine, parallel tiles, HTTP reuse, larger cache, non-blocking data refresh |
| **v1.8** | Historical | Slippy satellite map, drag/pan, recenter, progressive cached tiles, sharper zoom |
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
