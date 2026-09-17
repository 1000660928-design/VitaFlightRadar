# VitaFlightRadar Changelog

This file tracks the major public iterations of VitaFlightRadar.

## v1.9.1

Satellite-map stability hotfix.

- Fixed the real-hardware regression from v1.9 where aircraft rendered correctly but the satellite map could remain completely black.
- Removed the experimental v1.9 asynchronous satellite-worker pipeline from the recommended build.
- Restored the exact proven v1.8 multi-tile satellite renderer and interaction model.
- Kept one-finger panning, touch-and-hold recentering, pinch zoom and automatic aircraft-radius behavior.
- Kept coordinate search, aircraft filtering, AUTO/MANUAL refresh, route information and touch aircraft selection.
- Kept safe Release compiler optimization with `-O3` and linker dead-section removal.
- Does not force CPU/GPU clock changes or require overclocking.
- Public build contains no creator-specific startup coordinates.

## v1.9

Performance-focused experimental release built on the v1.8 feature set.

- Moved satellite tile network I/O off the render/UI thread.
- Added two background satellite-tile worker threads.
- Added persistent HTTP sessions so map workers could reuse network connections rather than repeating setup for every tile.
- Added priority tile scheduling, neighboring/parent-tile prefetching and a larger in-memory cache.
- Moved disk map-cache work and aircraft/route refresh work away from the frame loop.
- Added stale-request invalidation and debounced configuration persistence.
- Added aggressive Release compiler optimization.
- Removed experimental forced CPU/GPU clock changes before release.

**Known issue:** real PS Vita testing showed that the experimental asynchronous map pipeline could fail to deliver satellite tiles to the renderer, producing a black map while aircraft icons continued to work. v1.9 is therefore preserved for development history but is not recommended. Use v1.9.1 or newer.

## v1.8

- Rebuilt the satellite renderer as a multi-tile slippy-map system instead of replacing one large map image after every view change.
- Added one-finger map dragging/panning.
- Releasing a drag makes the new map center the aircraft tracking center and refreshes the live aircraft feed around that point.
- Added stationary touch-and-hold recentering to the geographic point beneath the finger.
- Kept two-finger pinch as the unified map zoom + aircraft-radius control.
- Added horizontal world wrapping to remove the hard left/right map edge when panning around the globe.
- Added an in-memory tile cache and a persistent Vita storage tile cache.
- Added progressive tile loading so missing tiles arrive one at a time instead of blanking and rebuilding the entire map at once.
- Added cached parent/low-resolution tile fallback while sharper imagery is being loaded, reducing cut-off/empty map regions.
- Increased supported satellite zoom detail up to zoom level 19.
- Changed post-pinch rebasing to prefer sharper imagery that can be downscaled instead of stretching lower-resolution imagery upward.
- Added a short touch-gesture grace period before new satellite network requests begin, reducing interruptions while repeatedly dragging or pinching.
- Improved smaller-text rendering for cleaner readability.
- Kept the aircraft feed radius tied to the actual visible map area, up to the adsb.fi nearby-query limit.
- Kept coordinate search, tap-to-select, UP/DOWN selection, Square AUTO/MANUAL refresh and START manual refresh.
- Public builds use a neutral coordinate-search example and do not include the creator's private startup-location file.

## v1.7

- Removed the world-map mode entirely.
- Removed L/R hardware zoom controls.
- Removed LEFT/RIGHT aircraft-radius controls.
- Removed SELECT-to-exit; use the Vita system UI/PS button normally.
- Pinch zoom is now the single control for both map scale and aircraft search radius.
- Zooming in automatically reduces the live-aircraft radius to match the visible map area.
- Zooming out automatically expands the live aircraft search area.
- Added high-resolution satellite tile rebasing after pinch gestures so the map does not remain a stretched, blurry image.
- Kept tap-to-select aircraft.
- Kept UP/DOWN aircraft cycling.
- Kept Triangle coordinate search.
- Kept Square AUTO/MANUAL refresh.
- Kept START manual refresh.
- Kept Circle cancel/back behavior in dialogs.
- Coordinate jumps preserve the current map zoom style and recalculate the aircraft feed radius from the visible map.

## v1.6

- Added two-finger touch pinch zoom.
- Added touch aircraft selection while keeping UP/DOWN selection.
- Made visual pinch zoom immediate instead of requiring a new tile download for every movement.
- Fixed radius controls becoming unresponsive after world-map mode or hardware map zoom.
- Made X a world-map toggle that returned to the previous local map state.

## v1.5

- Removed the 2 km preset.
- Added 5 km / 10 km / 15 km / 20 km local aircraft radius presets.
- Added whole-world satellite-map mode on X.
- Added coordinate search on Triangle.
- Added Vita on-screen keyboard support for coordinate entry.
- Separated satellite-image zoom from aircraft radius.
- Added X as Enter/confirm inside coordinate entry.
- Added Circle as cancel/back.

## v1.4

- Replaced the radar-focused UI direction with a satellite-map-focused design.
- Added satellite imagery.
- Added 2 km / 5 km / 10 km radius modes.
- Added known helicopter/rotorcraft filtering.
- Expanded filtering for other known non-airplane ADS-B categories.
- Retained 4-second live refresh.

## v1.3

- Reworked geographic rendering after earlier map attempts were unreliable.
- Added vector-map experiments and richer geographic context.
- Redesigned the selected-flight panel into route/time/metric cards.
- Added route progress presentation.
- Improved origin/destination and timetable lookup fallbacks.
- Added clearer map status reporting for troubleshooting.

## v1.2.1

- Packaging repair release.
- Fixed corrupted/incompatible Vita icon packaging that caused VitaShell installation failure.
- Added stronger VPK/icon validation to the build process.

## v1.2

- Reduced the previously very large aircraft search area.
- Improved map rendering attempts and route lookup fallbacks.
- Enlarged and simplified on-screen text.
- Continued 4-second aircraft refresh.
- Improved flight-information layout.

Historical note: the original v1.2 package had an install-package/icon problem; v1.2.1 is the corrected build.

## v1.1

- Replaced simple arrow/triangle aircraft markers with top-down airplane-shaped icons.
- Increased font weight/readability.
- Changed auto refresh from 10 seconds to 4 seconds.
- Added date/time display.
- Added a compass.
- Began map/terrain integration.
- Redesigned selected-flight information around callsign, plane type, origin/destination and estimated timing.
- Added route-data lookup beyond raw ADS-B position information.

## v1.0.1

- Fixed the VPK install error caused by an incompatible Vita icon PNG format.
- Repacked the application using a Vita-compatible indexed icon.

## v1.0

- First installable VitaFlightRadar prototype.
- Live aircraft data from adsb.fi over Wi-Fi.
- Circular radar presentation.
- Manual location center for Vita models without GPS.
- Aircraft callsign, registration/type, altitude, speed, heading and distance.
- Touch aircraft selection.
- UP/DOWN selection.
- L/R range presets.
- START manual refresh.
- Square AUTO/MANUAL mode.
- Initial 10-second automatic refresh.

Historical note: the first package required a follow-up packaging fix, released as v1.0.1.

## Early proof of concept

Before the first VPK build, the project existed as a VitaSDK source prototype focused on proving that a Wi-Fi-only PS Vita could download public ADS-B data and render nearby aircraft without GPS or 3G hardware.
