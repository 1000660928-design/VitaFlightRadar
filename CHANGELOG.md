# VitaFlightRadar Changelog

This file tracks the major public iterations of VitaFlightRadar.

## v2.1 Slim

Performance, flight-search correctness and map-focus update for the PS Vita Slim / PCH-2000.

- Shows the UI before live aircraft and route network work, reducing startup waiting.
- Loads the Vita search keyboard only when a search is opened.
- Reuses the libcurl connection/DNS/TLS state between requests.
- Uses shorter aircraft, map and route network timeouts.
- Prioritizes visible satellite tiles and removes long multi-level fallback download chains.
- Defers route/timetable lookup until after the searched aircraft is visible.
- Uses strict normalized callsign matching and removes the unrelated-first-result fallback.
- Fresh launches reset to the default 5 km view.
- Successful flight searches focus to a 10 km view.
- Retains 4-second automatic aircraft refresh and FOLLOW mode.
- Passed VitaSDK compilation, VPK generation and package-integrity validation.

## v2.0 Slim

Major search and live-flight-following update for the PS Vita Slim / PCH-2000.

- V2 development now targets the PCH-2000 Slim exclusively; the PCH-1000 compatibility edition is paused at v1.9.3 until the Slim feature set is complete.
- Triangle now opens a Search Hub instead of jumping directly to coordinate entry.
- Added **Enter Coordinates** and **Search Flight Number** choices.
- Search menus support front-touch selection and D-pad UP/DOWN + X.
- Added persistent storage for the three most recent coordinate searches.
- Added live flight-number/callsign search.
- A successful flight search centers the map on the aircraft, selects it and displays its data.
- Added FOLLOW mode so the map recenters on the searched aircraft as new live positions arrive.
- Removed the Square AUTO/MANUAL refresh toggle.
- START is intentionally unused in v2.0.
- Aircraft refresh is permanently automatic at approximately four seconds.
- Built successfully with VitaSDK and passed VPK integrity validation.

## v1.9.3-PCH1000

Dedicated PS Vita 1000 FAT/OLED compatibility build based on the v1.9.2 feature set.

- Created a separate public PCH-1000 release instead of modifying the proven PCH-2000 Slim package.
- Replaced the compatibility build's HTTPS path with VitaSDK libcurl using an mbedTLS backend and explicit TLS 1.2 behavior.
- Avoids depending on firmware-native HTTPS behavior for satellite/network requests.
- Explicitly creates the application and map-cache directory hierarchy before cache use.
- Added automatic satellite parent/lower-zoom fallback so an unavailable high-resolution tile does not leave the map permanently black.
- Retains the v1.9.2 high-resolution satellite changes: maximum zoom level 21, close-zoom extra-detail requests and 40 cached textures.
- Retains the stable v1.8-derived map renderer, one-finger panning, touch-and-hold recentering, pinch zoom and automatic aircraft-radius behavior.
- Retains coordinate search, aircraft filtering, AUTO/MANUAL refresh, route information and touch aircraft selection.
- Links the Vita pthread implementation needed by the mbedTLS static libraries.
- Passed VitaSDK compilation, linking, VPK generation, package integrity and Vita icon validation.
- Public build contains no creator-specific startup coordinates.
- Real PCH-1000 hardware confirmation is still requested before calling the compatibility issue universally resolved.

## v1.9.2

High-resolution satellite hotfix built on the stable v1.9.1/v1.8 renderer.

- Raised the satellite detail ceiling from zoom level 19 to zoom level 21.
- Added a close-zoom quality cushion: at street-level zooms, the renderer requests one complete tile level sharper than the mathematical minimum whenever possible.
- The sharper tile is downsampled to the Vita display instead of enlarging a lower-resolution tile, reducing software-side blur and pixelation.
- Keeps the previous lower-detail imagery visible while sharper tiles arrive, preserving the stable non-black fallback behavior.
- Increased the in-memory texture cache from 32 to 40 map tiles so higher-resolution imagery can remain resident longer.
- Kept the proven v1.8/v1.9.1 slippy-map renderer, one-finger panning, touch-and-hold recentering, pinch zoom and automatic aircraft-radius behavior.
- Kept coordinate search, aircraft filtering, AUTO/MANUAL refresh, route information and touch aircraft selection.
- Kept safe Release compiler optimization with `-O3` and no forced CPU/GPU clock settings.
- Public build contains no creator-specific startup coordinates.

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
