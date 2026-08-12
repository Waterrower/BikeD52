# Hardware Target

The project now targets the D52Q M6 development module / breadboard setup only.
The former prototype-PCB profile has been retired.

## Dev-Kit Wiring

- Display UART TX: P0.06
- Display UART RX: P0.08
- Resistance potentiometer: AIN1 / P0.03
- Cadence input: P0.11
- Green status LED / SPI OK: P0.17
- Red status LED / SPI error: P0.13
- Spare red LED: P0.12
- SPI NOR SCK: P0.19
- SPI NOR MOSI / SI / IO0: P0.20
- SPI NOR MISO / SO / IO1: P0.22
- SPI NOR CS: P0.24

Tie the IS25LP080D `/WP` and `/HOLD` pins high to 3V3.

## Build

The root `app.overlay` and `prj.conf` describe this Dev-Kit target directly.
A normal Zephyr/NCS application build therefore uses the Dev-Kit + LED + SPI
configuration without selecting an extra hardware profile.

The helper script remains for explicit build directories:

```powershell
.\tools\build-profile.ps1 -Target app -Pristine
.\tools\build-profile.ps1 -Target dfu -Pristine
```
