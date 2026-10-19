# NOHRD Bike Controller — Projektkontext

Zephyr/nRF Connect SDK Firmware (nRF52832, aktuell auf einem D52Q-Modul /
Dev-Kit-Breadboard) für den NOHRD-Bike-Displaycontroller. Treibt das
serielle Display, liest Trittfrequenz/Widerstand, exponiert BLE FTMS
(Indoor Bike Data) und relayt Daten von BLE-Herzfrequenzgurten.

Ursprünglich von einer ATmega/AVR-Firmware portiert. Referenzcode davon
liegt noch im Repo (siehe unten).

## Build & Flashen

- Aktives Hardware-Profil: **D52Q Dev-Kit** (siehe [HARDWARE_PROFILES.md](HARDWARE_PROFILES.md)).
  Das frühere `proto_pcb_v1`-Profil ist retired.
- Normaler App-Build/Flash-Zyklus: `tools/build-profile.ps1 -Target app -Pristine`
  → Build-Verzeichnis `build_devkit_spi`.
- DFU/MCUboot-Variante: `tools/build-profile.ps1 -Target dfu -Pristine`
  → `build_devkit_spi_dfu`.
- **Debuggen/Flashen direkt aus VS Code**: Run-and-Debug-Panel →
  „Launch nohrd_bike/build_devkit_spi" (siehe `.vscode/launch.json`).
  NICHT „Launch the active build configuration" nehmen — `nrf-connect.applications`
  ist aktuell leer, es gibt keine registrierte aktive Config.
- Viele `build_*/`-Verzeichnisse im Root sind einfach Ninja/CMake-Ausgabeordner
  aus Experimenten mit `tools/build-profile.ps1` (alle via `.gitignore` ausgeschlossen).

## Repo-Struktur — was wozu gehört

- `src/main.c`, `src/ble_ftms.c/h`, `src/ble_hr.c/h` — der **einzige aktiv
  kompilierte Code** (siehe `CMakeLists.txt`). Superloop-Architektur, kein RTOS-Threading.
- `src/atmegamain.c`, `src/addStrings.c`, `parsedisplay.c`, `sample_usart.c` —
  **Referenzcode der alten AVR-Firmware**, NICHT im Build. Bewusst so belassen
  (Ursprungsbasis des Projekts) — nicht versehentlich löschen.
- `d52q_gfit_bridge/` — separates Zusatzprojekt (GFIT-Protokoll-Bridge zu FTMS).
  Bleibt im Repo, evtl. später wieder gebraucht.
- `nohrd_legacy_gfit_app/` — Referenzkopie der originalen OEM-GFIT-Firmware
  (ANT+/BLE-Legacy, GFIT-UART) für Interop-/Abnahmetest-Vergleiche.
  `nohrd_legacy_gfit_app/build/` (vorgebaute Zips) ist gitignored.
- `diagnostics/` — kleine Standalone-Bring-up-Harnesses (BLE-OTA-Flags,
  Boot-UART-Flags, NOR-Wipe etc.), Debug-Historie der Hardware-Inbetriebnahme.
- `hardware/devkit_spi/` — das aktuell aktive Hardware-Profil für
  `tools/build-profile.ps1`.
- `sysbuild/`, `sysbuild.conf`, `pm_static.yml`, `prj_dfu.conf` — MCUboot/DFU-
  Build-Infrastruktur. Secondary-DFU-Slot liegt auf externem SPI-NOR-Flash.
- `keys/nohrd-bike-mcuboot-ec256.pem` — **echter privater MCUboot-Signierschlüssel**.
  Ist in `.gitignore` (`/keys/`, `*.pem`) — niemals versehentlich `git add -f` darauf anwenden.
- `artifacts/` — gitignored, historische Firmware-Snapshots (merged.hex/.zip).

## Wichtige Entscheidungen aus der letzten Session (2026-08-12)

Nach einer vollständigen Code-/Build-Analyse wurden folgende Fixes umgesetzt
(4 Commits: ANT+-Entfernung, Feature/Hardware-Bring-up, MCUboot/DFU-Infra,
Doku/Diagnostics/Legacy-Referenz):

- **Boot-Healthcheck**: `boot_write_img_confirmed()` in `main()` wird nur noch
  aufgerufen, wenn `ble_ftms_init`, `init_resistance_storage` und
  `init_training_inputs` erfolgreich waren — sonst bleibt das Image
  unbestätigt und MCUboot rollt beim nächsten Reset zurück.
- **Externer-Flash-Selftest entfernt**: `run_external_flash_selftest()` inkl.
  der nur dafür genutzten Debug-LEDs (`debug_led0/1`) wurde gelöscht — war
  nur für initiale Hardware-Tests gedacht, hätte sonst bei jedem Boot den
  Flash-Sektor abgenutzt.
- **BLE Heart Rate Service wird jetzt dynamisch registriert**
  (`CONFIG_BT_GATT_DYNAMIC_DB=y` in `prj.conf`): Der HRS taucht erst in der
  GATT-Service-Liste auf, wenn tatsächlich ein HR-Sensor verbunden ist
  (`ble_ftms_hrs_set_active` in `ble_hr.c`'s Discovery-/Disconnect-Callbacks),
  und BPM-Werte werden live durchgereicht (`ble_ftms_notify_heart_rate`).
  **Offener Punkt**: Register/Unregister läuft synchron im BLE-Callback-Kontext —
  sollte beim nächsten Pairing-Test am echten Gerät verifiziert werden.
- **`strcpy` in `ble_hr.c`** durch bounded-copy-Helfer ersetzt.
- **ANT+ komplett entfernt** (`ant_hr.c/h` gelöscht) — kein ANT+-Support geplant,
  außer eine passende Code-Lib für den nRF52832 taucht auf.
- **`boards/nrf54h20dk_*` und `nrf54l15dk_*`-Overlays gelöscht** — falsche
  SoC-Familie, keine davon passte zum D52Q/nRF52832-Ziel. Falls einer davon
  doch für eine geplante nRF54-Migration gedacht war, ist er per `git log`
  wiederherstellbar (Commit `93f672c`).
- **Bewusst NICHT geändert** (explizite Vorgabe): Zeichensatz-Sonderzeichen in
  den Text-Tabellen (Display ersetzt sie durch Fremdsprachen-Zeichen — so
  gewollt), `resistance_percent * 2`-Skalierung in FTMS Indoor Bike Data
  (FTMS-Standard-Auflösung 0,5 % pro Einheit), DIS-Firmware/Software-Version-
  Strings in `prj.conf` (werden später mit `VERSION` synchronisiert).
- **Git-Hygiene**: `.gitignore` um `/keys/`, `*.pem`, `/artifacts/`,
  `/nohrd_legacy_gfit_app/build/`, `*.pdf`, `/src/Log*.txt` erweitert — vorher
  lag der private Signierschlüssel ungeschützt als untracked Datei im Repo.

## Bekannte Altlasten / für später vorgemerkt

- Magische Zahlen ohne Herleitung in `main.c` (`FLYWHEEL_TO_PEDAL_RATIO_X100`,
  Power-/Speed-Regressionskoeffizienten) — stammen aus der alten AVR-Firmware.
- `display_cmd()` in `main.c` nutzt ein VLA mit Laufzeitgröße — fragil, aber
  aktuell unkritisch, da alle Caller kleine feste Payloads nutzen.
- Init-Fehler von BLE/NVS/GPIO in `main()` werden größtenteils weiterhin
  ignoriert (außer dem neuen Boot-Healthcheck-Gate).
