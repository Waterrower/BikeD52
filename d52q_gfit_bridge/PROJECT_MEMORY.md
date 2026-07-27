# Project Memory: D52Q GFIT Bridge

This file is intentional project memory. It records the prompt and reasoning
that define this firmware, so future work does not have to rediscover it from
the old Atmega code.

## User Context

The old hardware used one Atmega firmware/board family for two machines:
BIKE and SPRINTBOK. The Atmega code focused on UI, display, sensor inputs,
training states, and training calculations.

GFIT was the Garmin/ANT stack module optimized for fitness equipment
manufacturers. It avoided hand-writing BLE and ANT protocol logic and exposed a
simpler serial control/data interface to the Atmega.

GFIT is obsolete and no longer supported. Newer tablets and smartphones have
compatibility problems with it.

## New Goal

Create a new firmware project for D52/D52Q that replaces GFIT electrically and
logically from the Atmega's perspective. Since the Atmega cannot be OTA updated,
the D52Q must emulate the old GFIT behavior over UART and implement modern radio
functionality internally. The first scope is BLE only.

## Relationship To `nohrd_bike`

`nohrd_bike` is the full nRF52 redevelopment: UI, display, sensors, BLE FTMS,
BLE HR, and eventually ANT+ all on the nRF52.

`d52q_gfit_bridge` is different. It keeps the old Atmega firmware alive by
pretending to be GFIT:

- Atmega sends old GFIT commands and metric packets.
- D52Q parses those packets.
- D52Q advertises BLE FTMS and sends Indoor Bike Data.
- D52Q can later be updated OTA with nRF Connect.

The Atmega cannot be OTA updated, so compatibility pressure belongs on this
bridge firmware. Initial flashing still needs a programmer once so MCUboot is
present; after that, updates should go over BLE MCUmgr / nRF Connect.

## D52Q Wiring

Known D52Q pins for the GFIT replacement:

- P0.16: RX at D52Q, connected to Atmega GFIT TX.
- P0.17: TX at D52Q, connected to Atmega GFIT RX.
- P0.07: Sleep input from Atmega, active high.
- P0.21: Reset input from Atmega, active low.

The old Atmega code woke GFIT by driving Sleep low and Reset high. The bridge
should keep BLE disabled while Sleep is high or Reset is asserted, then send a
GFIT boot event when the module becomes active again.

## Legacy GFIT Summary

From `src/atmegamain.c`, GFIT handled:

- Hardware enable/reset/sleep pins.
- UART protocol with `0xa4`, payload length, payload, XOR checksum.
- Startup configuration: equipment type, metrics, product/manufacturer,
  hardware/firmware info, version query, advertising timeout, HR pairing
  settings, channel mode BLE/ANT/Both, and transition to Ready.
- State transitions: Ready, In Use, Finished, Off.
- BLE peripheral connect/disconnect events back to Atmega.
- HR pairing and HR measurement events.
- App/custom protocol handshakes and calibration status.
- Metric forwarding for FTMS-style data: cadence, resistance, power, speed,
  distance.

## Atmega Boot Init Sequence

At boot or when Bluetooth is enabled, the old Atmega runs
`startup_bluetooth_new()` and sends GFIT commands sequentially:

- `e2 e0 ...`: set equipment. For BIKE the equipment type observed in the old
  code is `25`; for SPRINTBOK it is `19`.
- `e2 e1 ...`: set the metric template. BIKE configures cadence, resistance,
  power, speed, and distance-like fields.
- `e2 e4 05 ...`: set product / advertising name. BIKE sends `NOHRDNB`,
  Ciclotte sends `NOHRDNC`, SPRINTBOK sends `NOHRDSP`.
- `e2 e4 06 ...`: set manufacturer name, observed as `Waterrower GmbH`.
- `e2 e4 04 ...`: set firmware revision bytes.
- `e2 e4 02 ...`: set hardware revision bytes.
- `e2 d1 03 ...`: set channel mode. Old code comments map masks to BLE, ANT,
  or BOTH.
- `e2 e5 00/01`: HR pairing configuration, not BLE on/off.
- `e2 d0 ...`: state transition. The old startup ends by transitioning GFIT to
  Ready.

The bridge should accept all of these in order, remember the resulting
configuration, and advertise with the product name supplied by the Atmega.

## Current Implementation Scope

The first bridge version implements only:

- GFIT-like UART parser and sender.
- Startup boot event.
- Generic compatibility ACK/NACK responses.
- BLE FTMS peripheral.
- Indoor Bike Data notifications from Atmega metric packets.
- BLE connection/disconnection events back to Atmega.

ANT+ and HR pairing are future work.
