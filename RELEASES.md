# VitaFlightRadar Releases

## PS Vita 2000 Slim — PCH-20xx

**Current stable release: v2.0 Slim**

### [Download VitaFlightRadar v2.0 Slim](releases/VitaFlightRadar-v2.0-Slim.vpk)

V2.0 is the current stable public release for the **PS Vita Slim / PCH-2000**. It is the last build confirmed through real Vita testing to provide the stable, responsive baseline we want. V2.1–V2.4 have been withdrawn from public download while the next release is rebuilt and tested.

Highlights:

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

V2.1, V2.2, V2.3 and V2.4 are retained in project history for development/reference only. Their public VPK downloads have been withdrawn because real Vita testing exposed regressions. A later version will become public only after it is tested and ready.

## PS Vita 1000 FAT / OLED — PCH-10xx / PCH-11xx

**Current compatibility build: v1.9.3-PCH1000**

### [Download VitaFlightRadar v1.9.3 PCH-1000](releases/VitaFlightRadar-v1.9.3-PCH1000.vpk)

The PCH-1000 edition is currently paused while V2 development focuses on the Slim. The plan is to port the completed Slim feature set back to the PCH-1000 after the Slim application reaches its final form.

## Historical versions

Detailed notes are in [CHANGELOG.md](CHANGELOG.md).

| Version | Status | Major milestone |
| --- | --- | --- |
| **v2.4 Slim** | Development / withdrawn | Experimental global search and map-continuity work |
| **v2.3 Slim** | Development / withdrawn | Experimental commercial resolver and background-worker work |
| **v2.2 Slim** | Development / withdrawn | Experimental multi-feed, route/time and tracking work |
| **v2.1 Slim** | Development / withdrawn | Experimental performance and search changes |
| **v2.0 Slim** | **Current stable PCH-2000 release** | Search Hub, saved coordinate history, flight search, continuous follow, always-on 4-second refresh |
| **v1.9.3-PCH1000** | PCH-1000 compatibility build | mbedTLS networking, cache-directory hardening, satellite fallback |
| **v1.9.2** | Previous PCH-2000 release | Higher-detail satellite zoom and larger texture cache |
| **v1.9.1** | Historical | Stable map hotfix |
| **v1.9** | Historical / Known map regression | Experimental async performance engine |
| **v1.8** | Historical | Slippy satellite map, pan and progressive cached tiles |
| **v1.7 and earlier** | Historical | Earlier VitaFlightRadar development |

Historical development/build branches remain in the repository.
