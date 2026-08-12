#include "gfit_uart.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "nohrd_legacy_config.h"

enum parser_state {
	PARSER_SYNC,
	PARSER_LEN,
	PARSER_PAYLOAD,
	PARSER_CHECKSUM,
};

static gfit_frame_handler_t frame_handler;
static enum parser_state state;
static uint8_t payload[GFIT_PAYLOAD_MAX];
static uint8_t payload_len;
static uint8_t payload_pos;
static uint8_t checksum;

static void parser_reset(void)
{
	state = PARSER_SYNC;
	payload_len = 0;
	payload_pos = 0;
	checksum = 0;
}

static void parser_feed(uint8_t byte)
{
	switch (state) {
	case PARSER_SYNC:
		if (byte == GFIT_FRAME_SYNC) {
			state = PARSER_LEN;
			checksum = 0;
		}
		break;
	case PARSER_LEN:
		if (byte > GFIT_PAYLOAD_MAX) {
			parser_reset();
			break;
		}
		payload_len = byte;
		payload_pos = 0;
		checksum = byte;
		state = payload_len == 0 ? PARSER_CHECKSUM : PARSER_PAYLOAD;
		break;
	case PARSER_PAYLOAD:
		payload[payload_pos++] = byte;
		checksum ^= byte;
		if (payload_pos >= payload_len) {
			state = PARSER_CHECKSUM;
		}
		break;
	case PARSER_CHECKSUM:
		if (byte == checksum && frame_handler != NULL) {
			frame_handler(payload, payload_len);
		}
		parser_reset();
		break;
	}
}

void gfit_uart_init(void)
{
	parser_reset();
}

void gfit_uart_process(void)
{
	/*
	 * TODO: read pending bytes from the nRF5 SDK UART driver and call
	 * parser_feed() for each byte.
	 */
	(void)parser_feed;
}

void gfit_uart_set_frame_handler(gfit_frame_handler_t handler)
{
	frame_handler = handler;
}

int gfit_uart_send_frame(const uint8_t *out_payload, size_t len)
{
	if (out_payload == NULL || len > GFIT_PAYLOAD_MAX) {
		return -1;
	}

	/*
	 * TODO: write GFIT_FRAME_SYNC, len, payload bytes, and XOR checksum
	 * through the legacy UART driver.
	 */
	return 0;
}
