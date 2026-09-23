# Vita Flight Radar

PS Vita homebrew live aircraft radar for **PCH-1000 (FAT)** and **PCH-2000 (Slim)**.

## v2.0 development branch

Branch: `v200-build`

This branch starts from the v1.9.3 dual-model compatibility code and is focused on the new unified Search experience.

### Planned v2.0 search flow

Press **TRIANGLE** on the radar to open **Search**.

1. **Search coordinates**
   - Enter latitude and longitude for a precise location.
   - The last **3 coordinate searches** are saved and shown below the coordinate entry option.
   - Recent coordinates can be selected with the touchscreen or with **D-pad UP/DOWN + X**.
   - New searches move to the top of the history automatically.

2. **Search flight number**
   - Enter a live flight/callsign such as `ELY5230`.
   - Spaces and letter case are normalized before lookup.
   - When the aircraft is currently visible to the ADS-B network, the radar centers on its live position.
   - The matching aircraft is selected immediately so its live details can be inspected.

### v2.0 controls

Main radar:
- **TRIANGLE**: Search menu
- **CIRCLE**: Back / Cancel
- **X**: Confirm / Enter
- **D-pad UP/DOWN**: Menu or aircraft selection
- **L / R**: Satellite-map zoom/range controls already present in v1.9.x
- **SQUARE**: unused in v2.0
- **START**: unused in v2.0
- **SELECT**: Exit
- **Front touchscreen**: select aircraft and choose Search menu items

Search menu:
- Touch an item directly, or use **D-pad UP/DOWN**
- **X**: confirm highlighted item
- **CIRCLE**: return to radar

### Refresh behavior

Aircraft refresh is **always automatic every 4 seconds**.

The old SQUARE AUTO/MANUAL toggle is removed. There is no manual aircraft-refresh mode in v2.0.

### Data sources

- Nearby aircraft: adsb.fi geographic endpoint
- Flight/callsign lookup: adsb.fi callsign endpoint

No API key or account is required for the public personal-use endpoints used by the app.

### Compatibility target

v2.0 keeps the v1.9.3 compatibility work for:
- PS Vita PCH-1000 / FAT
- PS Vita PCH-2000 / Slim
- firmware-independent TLS path used by the v1.9.3 build
- satellite tile fallback behavior from v1.9.3

## Development status

The v2.0 branch has been created from `v193-build`. The UI/search implementation and VitaSDK package build are being developed on this branch before a public release is tagged.
