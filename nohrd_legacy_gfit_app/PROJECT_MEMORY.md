# Project Memory

This project was created on 2026-07-28 after deciding to pause the Zephyr
`d52q_gfit_bridge` approach for existing field products.

Context:

- Existing products contain a D52Q/GFIT module.
- There is no practical hardware-pin/SWD access in installed customer products.
- Removing ANT/ANT+ would not be customer-friendly.
- The old GFIT stack is obsolete and has BLE compatibility issues with modern
  tablets and smartphones.
- The desired field-safe path is therefore an application-only Legacy DFU update
  that keeps the existing SoftDevice and bootloader.

Important conclusion:

Zephyr/NCS BLE cannot be mixed with the old ANT/BLE SoftDevice. The legacy
application must use the old SoftDevice APIs for both ANT and BLE:

- `sd_ant_*` for ANT/ANT+
- `sd_ble_*` for BLE

Reference DFU package:

- `../build_gfit_bridge/D52QGF-ap_4.04.00.zip`
- structure: `encrypted_nrf52832_xxaa.bin`,
  `encrypted_nrf52832_xxaa.dat`, `manifest.json`
- `nrfutil nrf5sdk-tools pkg display` output:
  - type: application
  - fw_version: 14
  - hw_version: 0
  - sd_req: 0x00
  - sd_size: 0
  - bl_size: 0
  - app_size: 168324
  - hash_type: NONE
  - signature type is shown as ECDSA_P256_SHA256, but signature is empty
  - boot_validation is empty
- The `.bin` does not start with a valid Cortex-M vector table and should be
  treated as encrypted/scrambled.

S332 documentation:

- `S332_SDS_v2.0.pdf` was added by the user on 2026-07-28.
- The document is needed as the local API/memory-layout reference for the
  legacy app-only path.
- The exact installed SoftDevice and app linker layout still need confirmation
  from old release notes, the old project, or Garmin/Dynastream.

Open dependencies:

- Garmin/Dynastream response about signing
- original nRF5 SDK version
- exact SoftDevice family/version
- exact linker layout/application base address
- Legacy Secure DFU package parameters
- encryption/scrambling method for `encrypted_nrf52832_xxaa.bin`
