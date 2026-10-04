# VitaFlightRadar Changelog ✈️

This changelog covers the versions currently presented as public downloads.

## v2.5

Current recommended public release.

- Added persistence for the last map latitude, longitude and zoom so the app reopens at the user's previous view.
- Added the on-screen **TRACK FLIGHT** control for explicitly following a selected aircraft.
- Improved tracked-aircraft locking while zooming.
- Keeps the last known tracked aircraft visible when a live-data refresh temporarily misses the target.
- Prevents temporary route-data failures from immediately replacing previously valid route/timetable information.
- Changed the compass to show the selected aircraft's actual heading.
- Added more resilient multi-source live-flight search and equivalent identifier handling.
- Uses global lookup to find a requested aircraft, then returns to a focused local traffic view around the result.
- Moved slow flight/search operations away from normal interaction paths where possible.
- Improved satellite-map continuity during zoom transitions.
- Standardized satellite imagery to one consistent source for more consistent color and appearance.
- Added a fresh versioned map cache for the updated imagery behavior.
- Increased the in-memory map tile cache from 40 to 64 textures.
- Prioritizes proper sharp center tiles before lower-detail fallback imagery.
- Retains approximately 4-second automatic aircraft refresh.
- Passed VitaSDK compilation, VPK generation and package-integrity validation.
- Approved after real PS Vita hardware testing.

## v2.0

Stable fallback release.

- Added the Triangle Search Hub.
- Added coordinate search and flight-number/callsign search.
- Added front-touch and D-pad + X menu control.
- Added persistent storage for the three most recent coordinate searches.
- Successful flight search jumps to and selects the aircraft.
- Added automatic flight-follow behavior.
- Live aircraft refresh runs approximately every four seconds.
- Passed VitaSDK compilation and VPK integrity validation.
- Retained publicly as the recommended fallback if V2.5 has problems on a particular Vita.

## v1.9.3

Compatibility fallback.

- Added compatibility-focused networking changes using VitaSDK libcurl with an mbedTLS backend.
- Hardened application and map-cache directory creation.
- Added satellite parent/lower-zoom fallback behavior.
- Retains high-resolution satellite support and established touch-map controls.
- Can be used on either PCH-1000 FAT/OLED or PCH-2000 Slim systems.
- Retained publicly for systems that have trouble with newer releases.
