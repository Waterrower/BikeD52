//setup:
parser_context_t Display_Parser;


parser_init(&Display_Parser);

void parser_init(parser_context_t *context) {
	context->state = PENDING;
	context->payload_index = 0;
	context->current_message.complete = false;
	context->current_message.expected_length=0;
}

//polling: 
if (parse_Display(&UsartRX_Buffer,&Display_Parser)) actDisplay();




//parsing:
bool parse_Display(ring_buffer_t *ring_buffer, parser_context_t *context) {
	if (context->state==FINISHED){
		parser_init(context);
	}

	
	while (!ring_buffer_is_empty(ring_buffer)) {
		
		uint8_t byte = ring_buffer_read(ring_buffer);
		
		switch (context->state) {
			case PENDING:
			if (byte == 0xAA) { // Start of a new message
				context->current_message.prefix = byte;
				context->state = COLLECTING;
				context->payload_index = 0;
			}
			break;
			case COLLECTING:
			if (context->payload_index >= 12) {  // Message exceeds expected size, abort and reset
				context->state = ABORTED;
				break;
			}

			// Add byte to payload if not yet at suffix.
			context->current_message.payload[context->payload_index++] = byte;

			// Check if the last four bytes match the suffix pattern
			if (context->payload_index > 4 &&
			context->current_message.payload[context->payload_index - 4] == 0xE4 &&
			context->current_message.payload[context->payload_index - 3] == 0x1B &&
			context->current_message.payload[context->payload_index - 2] == 0x11 &&
			context->current_message.payload[context->payload_index - 1] == 0xEE) {
				context->state = FINISHED;
				context->current_message.payload_length = context->payload_index-6;
				context->current_message.complete = true;
				// Here, you should process the collected message before resetting.
				// Then reset for a new message.
				return true;
				break; // Exit the loop to process the collected message.
				
			}
			break;
			
			
			case ABORTED:
			// Message was aborted due to size limit, reset to start a new one.
			parser_init(context);
			return false;
		}
	}
	return false;
}

