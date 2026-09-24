# VitaFlightRadar Releases

## PS Vita 2000 Slim — PCH-20xx

**Current release: v2.2 Slim**

### [Download VitaFlightRadar v2.2 Slim](releases/VitaFlightRadar-v2.2-Slim.vpk)

V2.2 is the current main release for the **PS Vita Slim / PCH-2000**, focused on broader live-flight coverage, stronger route/time data and explicit user-controlled flight tracking.

Highlights:

- Multi-feed live aircraft coverage
- More tolerant flight-number/callsign matching
- Route + schedule fallback merging
- HexDB route fallback
- Explicit on-screen TRACK FLIGHT toggle
- Tracked-aircraft recovery if a feed temporarily loses the plane
- Faster startup and satellite loading
- Faster flight search path
- Strict exact callsign matching
- Fresh-launch default zoom reset
- Automatic 10 km flight focus
- Triangle Search Hub
- Coordinate search plus three recent saved coordinate searches
- Live flight-number/callsign search
- Automatic jump to a searched aircraft
- Continuous FOLLOW mode
- Touchscreen or D-pad + X menu control
- Permanent 4-second aircraft refresh
- Square and START intentionally unused
- Existing satellite map, pan, pinch zoom, aircraft selection, route data, filtering and tile caching retained
- Passed VitaSDK compilation and VPK integrity validation

## PS Vita 1000 FAT / OLED — PCH-10xx / PCH-11xx

**Current compatibility build: v1.9.3-PCH1000**

### [Download VitaFlightRadar v1.9.3 PCH-1000](releases/VitaFlightRadar-v1.9.3-PCH1000.vpk)

The PCH-1000 edition is currently paused while V2 development focuses on the Slim. The plan is to port the completed Slim feature set back to the PCH-1000 after the Slim application reaches its final form.

## Historical versions

Detailed notes are in [CHANGELOG.md](CHANGELOG.md).

| Version | Status | Major milestone |
| --- | --- | --- |
| **v2.2 Slim** | **Current PCH-2000 release** | Multi-feed flight coverage, route/time fixes, explicit tracking button |
| **v2.1 Slim** | Previous PCH-2000 release | Performance update, exact flight matching, launch zoom reset, focused flight search |
| **v2.0 Slim** | Previous PCH-2000 release | Search Hub, saved coordinate history, flight search, continuous follow, always-on 4-second refresh |
| **v1.9.3-PCH1000** | PCH-1000 compatibility build | mbedTLS networking, cache-directory hardening, satellite fallback |
| **v1.9.2** | Previous PCH-2000 release | Higher-detail satellite zoom and larger texture cache |
| **v1.9.1** | Historical | Stable map hotfix |
| **v1.9** | Historical / Known map regression | Experimental async performance engine |
| **v1.8** | Historical | Slippy satellite map, pan and progressive cached tiles |
| **v1.7 and earlier** | Historical | Earlier VitaFlightRadar development |

Historical development/build branches remain in the repository.
