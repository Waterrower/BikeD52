D52Q GFIT Bridge
################

Purpose
*******

This project is a new nRF52/D52Q firmware that sits where the old GFIT module
used to sit. The Atmega firmware should continue to see a GFIT-like UART device,
while the nRF52 implements the modern BLE side directly.

Initial scope:

- Emulate the old GFIT UART frame format for the Atmega.
- Accept startup/configuration commands from the old Atmega firmware.
- Accept indoor-bike metric packets from the Atmega.
- Advertise BLE FTMS and notify Indoor Bike Data.
- Support OTA update preparation through MCUmgr over BLE.
- BLE first. ANT+ is intentionally left for a later phase.

Legacy Context
**************

The old controller board was shared between two training devices: BIKE and
SPRINTBOK. The Atmega handled UI, display, local state, sensor processing, and
training calculations. GFIT handled Garmin/ANT+/BLE fitness profiles so the
device firmware did not need to implement the full radio stack.

GFIT is now obsolete and unsupported, causing compatibility problems with newer
tablets and smartphones. This firmware is intended as a replaceable, OTA-capable
GFIT compatibility layer.

GFIT UART Compatibility
***********************

Legacy GFIT frames are:

``0xa4 <payload-length> <payload...> <xor-checksum>``

The checksum is the XOR of ``0xa4``, the payload length, and every payload byte.

Implemented now:

- Boot event ``6f`` on startup.
- Generic success/fail responses for legacy custom handshakes.
- BLE connection/disconnection event toward Atmega.
- Parsing of legacy metric packets beginning with ``e2 e1``.
- FTMS Indoor Bike Data notifications from those metrics.

Hardware Defaults
*****************

The current overlay uses UART0 at 9600 baud:

- RX on D52Q P0.16
- TX on D52Q P0.17
- Sleep input on D52Q P0.07, active high
- Reset input on D52Q P0.21, active low

The old Atmega GFIT enable sequence drives Sleep low and Reset high when the
module should run. The bridge mirrors that behavior: BLE is enabled only while
Sleep is low and Reset is released. When the lines return active, the bridge
sends a GFIT boot event again.

Build
*****

Example:

Application-only build, useful while developing:

``cmake -S d52q_gfit_bridge -B build_gfit_bridge -GNinja -DBOARD=nrf52dk/nrf52832``

``cmake --build build_gfit_bridge``

Initial OTA-capable hardware flash should use a sysbuild/MCUboot build so the
bootloader is programmed together with the application. In an NCS terminal:

``west build --sysbuild -b nrf52dk/nrf52832 d52q_gfit_bridge``

The current configuration uses the NCS development MCUboot key:
``C:/ncs/v3.3.1/bootloader/mcuboot/root-rsa-2048.pem``.
Replace this with a project key before production. Partition sizing and MCUboot
image layout still need verification on the exact D52Q target.
