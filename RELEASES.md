# VitaFlightRadar Releases

## Current stable release

**VitaFlightRadar v2.0**

### [Download VitaFlightRadar v2.0](releases/VitaFlightRadar-v2.0-Slim.vpk)

V2.0 is the current recommended stable public build for **both PS Vita PCH-1000 FAT/OLED and PCH-2000 Slim systems**.

### Compatibility fallback

### [Download VitaFlightRadar v1.9.3 compatibility build](releases/VitaFlightRadar-v1.9.3-PCH1000.vpk)

> [!IMPORTANT]
> **Both downloads can be used on either Vita model.**
>
> **Try V2.0 first. If V2.0 does not work correctly on your system, especially on a PCH-1000 FAT/OLED Vita, try V1.9.3.** The V1.9.3 build is kept available as a compatibility fallback.

V1.9.3 includes compatibility-focused networking and map changes. It is not a different application for a different Vita model.

Highlights of V2.0:

- Triangle Search Hub
- Coordinate search with three recent saved coordinate searches
- Live flight-number/callsign search
- Automatic jump to a searched aircraft
- FOLLOW mode
- Permanent 4-second aircraft refresh
- Touchscreen or D-pad + X menu control
- Satellite map, panning, pinch zoom and aircraft selection
- Route/timetable information when public data is available
- Square and START intentionally unused
- Passed VitaSDK compilation and VPK integrity validation
- Confirmed as the current real-hardware stable baseline

### Development builds

V2.1, V2.2, V2.3 and V2.4 are retained in project history for development/reference only. Their public VPK downloads remain withdrawn because real Vita testing exposed regressions. **V2.4 is the active development line.** A newer version will become the public download only after real-hardware testing confirms it is ready.

## Historical versions

Detailed notes are in [CHANGELOG.md](CHANGELOG.md).

| Version | Status | Major milestone |
| --- | --- | --- |
| **v2.4** | Development / withdrawn | Experimental global search and map-continuity work |
| **v2.3** | Development / withdrawn | Experimental commercial resolver and background-worker work |
| **v2.2** | Development / withdrawn | Experimental multi-feed, route/time and tracking work |
| **v2.1** | Development / withdrawn | Experimental performance and search changes |
| **v2.0** | **Current stable release** | Recommended for PCH-1000 and PCH-2000; Search Hub, saved coordinate history, flight search, continuous follow, always-on 4-second refresh |
| **v1.9.3** | Compatibility fallback | Available for PCH-1000 and PCH-2000; mbedTLS networking, cache-directory hardening, satellite fallback |
| **v1.9.2** | Previous release | Higher-detail satellite zoom and larger texture cache |
| **v1.9.1** | Historical | Stable map hotfix |
| **v1.9** | Historical / Known map regression | Experimental async performance engine |
| **v1.8** | Historical | Slippy satellite map, pan and progressive cached tiles |
| **v1.7 and earlier** | Historical | Earlier VitaFlightRadar development |

Historical development/build branches remain in the repository.
