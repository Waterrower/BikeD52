NOHRD Bike Controller
#####################

Zephyr/Nordic nRF52 firmware for the NOHRD bike display controller.

Overview
********

This application drives the serial display UI, reads cadence and resistance
inputs, exposes BLE FTMS indoor bike data, and can connect to BLE heart-rate
sensors. ANT+ heart-rate support is represented in the UI, but currently stays
disabled until the ANT stack is available.

Hardware
********

The current overlay targets ``nrf52dk/nrf52832`` with:

- UART0 TX on P0.06 and RX on P0.08 for the display.
- Cadence input on P0.11.
- Resistance ADC on P0.03 / AIN1.
- Debug/display-RX LED on P0.17.

Building
********

Build and flash with the nRF Connect SDK tools for ``nrf52dk/nrf52832``.
