#ifndef GFIT_UART_H
#define GFIT_UART_H

#include <stddef.h>
#include <stdint.h>

typedef void (*gfit_frame_handler_t)(const uint8_t *payload, size_t len);

void gfit_uart_init(void);
void gfit_uart_process(void);
void gfit_uart_set_frame_handler(gfit_frame_handler_t handler);
int gfit_uart_send_frame(const uint8_t *payload, size_t len);

#endif /* GFIT_UART_H */
