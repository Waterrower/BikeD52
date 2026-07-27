#ifndef GFIT_PROTOCOL_H
#define GFIT_PROTOCOL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define GFIT_FRAME_START 0xa4
#define GFIT_PAYLOAD_MAX 40

struct gfit_parser {
	enum {
		GFIT_WAIT_START,
		GFIT_WAIT_LENGTH,
		GFIT_COLLECT_PAYLOAD,
		GFIT_WAIT_CHECKSUM,
	} state;
	uint8_t payload[GFIT_PAYLOAD_MAX];
	uint8_t length;
	uint8_t index;
	uint8_t checksum;
};

void gfit_parser_init(struct gfit_parser *parser);
bool gfit_parser_push(struct gfit_parser *parser, uint8_t byte,
		      const uint8_t **payload, size_t *length);
uint8_t gfit_checksum(const uint8_t *payload, size_t length);
void gfit_make_frame(const uint8_t *payload, size_t length,
		     uint8_t *frame, size_t *frame_length);

#endif
