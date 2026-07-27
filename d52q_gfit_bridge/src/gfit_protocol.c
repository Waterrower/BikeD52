#include "gfit_protocol.h"

#include <stdbool.h>

void gfit_parser_init(struct gfit_parser *parser)
{
	parser->state = GFIT_WAIT_START;
	parser->length = 0;
	parser->index = 0;
	parser->checksum = 0;
}

uint8_t gfit_checksum(const uint8_t *payload, size_t length)
{
	uint8_t checksum = GFIT_FRAME_START ^ (uint8_t)length;

	for (size_t i = 0; i < length; i++) {
		checksum ^= payload[i];
	}

	return checksum;
}

bool gfit_parser_push(struct gfit_parser *parser, uint8_t byte,
		      const uint8_t **payload, size_t *length)
{
	switch (parser->state) {
	case GFIT_WAIT_START:
		if (byte == GFIT_FRAME_START) {
			parser->state = GFIT_WAIT_LENGTH;
			parser->checksum = GFIT_FRAME_START;
		}
		break;
	case GFIT_WAIT_LENGTH:
		if (byte > GFIT_PAYLOAD_MAX) {
			gfit_parser_init(parser);
			break;
		}
		parser->length = byte;
		parser->index = 0;
		parser->checksum ^= byte;
		parser->state = byte == 0U ? GFIT_WAIT_CHECKSUM : GFIT_COLLECT_PAYLOAD;
		break;
	case GFIT_COLLECT_PAYLOAD:
		parser->payload[parser->index++] = byte;
		parser->checksum ^= byte;
		if (parser->index == parser->length) {
			parser->state = GFIT_WAIT_CHECKSUM;
		}
		break;
	case GFIT_WAIT_CHECKSUM:
		if (byte == parser->checksum) {
			*payload = parser->payload;
			*length = parser->length;
			gfit_parser_init(parser);
			return true;
		}
		gfit_parser_init(parser);
		break;
	}

	return false;
}

void gfit_make_frame(const uint8_t *payload, size_t length,
		     uint8_t *frame, size_t *frame_length)
{
	size_t pos = 0;

	frame[pos++] = GFIT_FRAME_START;
	frame[pos++] = (uint8_t)length;
	for (size_t i = 0; i < length; i++) {
		frame[pos++] = payload[i];
	}
	frame[pos++] = gfit_checksum(payload, length);
	*frame_length = pos;
}
