#ifndef NOHRD_LEGACY_CONFIG_H
#define NOHRD_LEGACY_CONFIG_H

#include <stdint.h>

#define NOHRD_PRODUCT_NAME          "NOHRD Bike"
#define NOHRD_BLE_ADV_NAME_DEFAULT "NOHRD Bike"

/* GFIT UART framing observed in the old project. */
#define GFIT_FRAME_SYNC             0xA4u
#define GFIT_PAYLOAD_MAX            128u

/*
 * D52Q UART pins from the bridge investigation.
 * These are kept here as reference and must be checked against the legacy
 * board header used by the old nRF5 SDK project.
 */
#define D52Q_ATMEGA_RX_PIN          16u
#define D52Q_ATMEGA_TX_PIN          17u
#define D52Q_SLEEP_PIN              7u
#define D52Q_RESET_PIN              21u

/*
 * Legacy DFU reference clues from D52QGF-ap_4.04.00.
 * Do not use as final release values before verifying with the original
 * bootloader/SoftDevice build configuration.
 */
#define D52QGF_REFERENCE_SD_REQ     0x0094u
#define D52QGF_REFERENCE_APP_SIZE   168324u

#endif /* NOHRD_LEGACY_CONFIG_H */
