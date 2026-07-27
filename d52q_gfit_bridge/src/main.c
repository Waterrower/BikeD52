#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>

#include "ble_ftms.h"
#include "gfit_protocol.h"

#define UART_DEVICE_NODE DT_NODELABEL(uart0)
#define USER_NODE DT_PATH(zephyr_user)
#define UART_RX_RING_SIZE 256
#define GFIT_POLL_MS 5

#define GFIT_BOOT 0x6f
#define GFIT_EVENT 0xe0
#define GFIT_CUSTOM_EVENT 0xb1
#define GFIT_HR_EVENT 0xb2
#define GFIT_CONTROL_EVENT 0xb5
#define GFIT_PERIPHERAL_EVENT 0xba
#define GFIT_METRIC_PACKET 0xe1
#define GFIT_COMMAND 0xe2
#define GFIT_CUSTOM_PROTOCOL 0xfe

#define GFIT_TAG_SPEED 0x21
#define GFIT_TAG_CADENCE 0x22
#define GFIT_TAG_POWER 0x2a
#define GFIT_TAG_RESISTANCE 0x33

struct ring_buffer {
	uint16_t start;
	uint16_t end;
	uint8_t data[UART_RX_RING_SIZE];
};

struct bike_metrics {
	uint16_t speed_centi_kmh;
	uint16_t cadence_half_rpm;
	int16_t resistance_percent;
	int16_t power_watt;
};

static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);
static const struct gpio_dt_spec gfit_sleep =
	GPIO_DT_SPEC_GET(USER_NODE, gfit_sleep_gpios);
static const struct gpio_dt_spec gfit_reset =
	GPIO_DT_SPEC_GET(USER_NODE, gfit_reset_gpios);
static struct ring_buffer uart_rx;
static struct gfit_parser parser;
static struct bike_metrics last_metrics;
static bool bridge_ready;
static bool gfit_control_active;

static bool ring_is_empty(const struct ring_buffer *ring)
{
	return ring->start == ring->end;
}

static bool ring_is_full(const struct ring_buffer *ring)
{
	return ((ring->end + 1U) % UART_RX_RING_SIZE) == ring->start;
}

static void ring_write(struct ring_buffer *ring, uint8_t byte)
{
	if (ring_is_full(ring)) {
		ring->start = (ring->start + 1U) % UART_RX_RING_SIZE;
	}
	ring->data[ring->end] = byte;
	ring->end = (ring->end + 1U) % UART_RX_RING_SIZE;
}

static uint8_t ring_read(struct ring_buffer *ring)
{
	uint8_t byte = 0;

	if (!ring_is_empty(ring)) {
		byte = ring->data[ring->start];
		ring->start = (ring->start + 1U) % UART_RX_RING_SIZE;
	}

	return byte;
}

static void uart_send(const uint8_t *data, size_t length)
{
	for (size_t i = 0; i < length; i++) {
		uart_poll_out(uart_dev, data[i]);
	}
}

static void gfit_send_payload(const uint8_t *payload, size_t length)
{
	uint8_t frame[GFIT_PAYLOAD_MAX + 3U];
	size_t frame_length;

	if (length > GFIT_PAYLOAD_MAX) {
		return;
	}

	gfit_make_frame(payload, length, frame, &frame_length);
	uart_send(frame, frame_length);
}

static void gfit_send_boot_event(void)
{
	const uint8_t payload[] = { GFIT_BOOT };

	gfit_send_payload(payload, sizeof(payload));
}

static void gfit_send_response(bool success)
{
	const uint8_t payload[] = {
		GFIT_COMMAND, 0xb0, GFIT_CUSTOM_PROTOCOL,
		0x01, success ? 0x01 : 0x00, 0x01, 0x00, 0x00,
	};

	gfit_send_payload(payload, sizeof(payload));
}

static void gfit_send_calibrated_status(bool calibrated)
{
	const uint8_t payload[] = {
		0x02, 0x01, calibrated ? 0x01 : 0x02, 0x00, 0x00,
	};

	gfit_send_payload(payload, sizeof(payload));
}

static void gfit_send_peripheral_event(bool connected)
{
	const uint8_t payload[] = {
		GFIT_EVENT, GFIT_PERIPHERAL_EVENT, connected ? 0x00 : 0x01,
	};

	gfit_send_payload(payload, sizeof(payload));
}

static uint16_t get_le16(const uint8_t *data)
{
	return (uint16_t)data[0] | ((uint16_t)data[1] << 8);
}

static void handle_metric_packet(const uint8_t *payload, size_t length)
{
	size_t pos = 2;

	while (pos < length) {
		uint8_t tag = payload[pos++];

		switch (tag) {
		case GFIT_TAG_CADENCE:
			if (pos + 2U <= length) {
				last_metrics.cadence_half_rpm = get_le16(&payload[pos]);
				pos += 2U;
			}
			break;
		case GFIT_TAG_RESISTANCE:
			if (pos + 2U <= length) {
				uint16_t raw = get_le16(&payload[pos]);

				last_metrics.resistance_percent = (int16_t)(raw / 2U);
				pos += 2U;
			}
			break;
		case GFIT_TAG_POWER:
			if (pos + 2U <= length) {
				last_metrics.power_watt = (int16_t)get_le16(&payload[pos]);
				pos += 2U;
			}
			break;
		case GFIT_TAG_SPEED:
			if (pos + 2U <= length) {
				uint16_t mm_per_sec = get_le16(&payload[pos]);
				uint32_t centi_kmh = ((uint32_t)mm_per_sec * 36U) / 100U;

				last_metrics.speed_centi_kmh = (uint16_t)centi_kmh;
				pos += 2U;
			}
			break;
		default:
			pos = length;
			break;
		}
	}

	(void)ble_ftms_notify_indoor_bike(last_metrics.speed_centi_kmh,
					  last_metrics.cadence_half_rpm,
					  last_metrics.resistance_percent,
					  last_metrics.power_watt);
}

static void handle_custom_protocol(const uint8_t *payload, size_t length)
{
	uint8_t operation;
	uint8_t target;

	if (length < 5U) {
		gfit_send_response(false);
		return;
	}

	operation = payload[3];
	target = payload[4];

	if (operation == 0x02U && (target == 0x01U || target == 0x02U)) {
		gfit_send_response(true);
		return;
	}

	if (operation == 0x04U) {
		gfit_send_response(true);
		gfit_send_calibrated_status(true);
		return;
	}

	gfit_send_response(false);
}

static void handle_gfit_command(const uint8_t *payload, size_t length)
{
	if (length >= 2U && payload[1] == GFIT_METRIC_PACKET) {
		handle_metric_packet(payload, length);
		return;
	}

	if (length >= 3U && payload[1] == GFIT_CUSTOM_EVENT &&
	    payload[2] == GFIT_CUSTOM_PROTOCOL) {
		handle_custom_protocol(payload, length);
		return;
	}

	if (length >= 2U && payload[1] == 0xd2U) {
		bridge_ready = true;
		(void)ble_ftms_set_enabled(true);
		gfit_send_response(true);
		return;
	}

	if (length >= 3U && payload[1] == 0xe5U) {
		(void)ble_ftms_set_enabled(payload[2] != 0U);
		gfit_send_response(true);
		return;
	}

	gfit_send_response(true);
}

static void handle_gfit_payload(const uint8_t *payload, size_t length)
{
	if (!gfit_control_active) {
		return;
	}

	if (length == 0U) {
		return;
	}

	if (payload[0] == GFIT_COMMAND) {
		handle_gfit_command(payload, length);
	}
}

static void uart_irq_handler(const struct device *dev, void *user_data)
{
	ARG_UNUSED(user_data);

	if (!uart_irq_update(dev)) {
		return;
	}

	while (uart_irq_rx_ready(dev)) {
		uint8_t data[16];
		int read = uart_fifo_read(dev, data, sizeof(data));

		if (read <= 0) {
			break;
		}

		for (int i = 0; i < read; i++) {
			ring_write(&uart_rx, data[i]);
		}
	}
}

static void ble_connection_changed(bool connected)
{
	if (!gfit_control_active) {
		return;
	}

	gfit_send_peripheral_event(connected);
}

static bool gfit_control_pins_active(void)
{
	bool awake = true;
	bool reset_released = true;

	if (gpio_is_ready_dt(&gfit_sleep)) {
		awake = gpio_pin_get_dt(&gfit_sleep) == 0;
	}

	if (gpio_is_ready_dt(&gfit_reset)) {
		reset_released = gpio_pin_get_dt(&gfit_reset) > 0;
	}

	return awake && reset_released;
}

static void update_gfit_control_state(void)
{
	bool active = gfit_control_pins_active();

	if (active == gfit_control_active) {
		return;
	}

	gfit_control_active = active;
	if (active) {
		gfit_parser_init(&parser);
		bridge_ready = true;
		(void)ble_ftms_set_enabled(true);
		gfit_send_boot_event();
	} else {
		bridge_ready = false;
		(void)ble_ftms_set_enabled(false);
		gfit_parser_init(&parser);
		uart_rx.start = uart_rx.end;
	}
}

static void init_gfit_control_pins(void)
{
	if (gpio_is_ready_dt(&gfit_sleep)) {
		(void)gpio_pin_configure_dt(&gfit_sleep, GPIO_INPUT);
	}

	if (gpio_is_ready_dt(&gfit_reset)) {
		(void)gpio_pin_configure_dt(&gfit_reset, GPIO_INPUT);
	}
}

static void process_uart(void)
{
	while (!ring_is_empty(&uart_rx)) {
		const uint8_t *payload;
		size_t length;
		uint8_t byte = ring_read(&uart_rx);

		if (gfit_parser_push(&parser, byte, &payload, &length)) {
			handle_gfit_payload(payload, length);
		}
	}
}

int main(void)
{
	int ret;

	if (!device_is_ready(uart_dev)) {
		return 0;
	}

	gfit_parser_init(&parser);
	init_gfit_control_pins();

	ret = uart_irq_callback_user_data_set(uart_dev, uart_irq_handler, NULL);
	if (ret < 0) {
		return 0;
	}
	uart_irq_rx_enable(uart_dev);

	(void)ble_ftms_init(ble_connection_changed);
	update_gfit_control_state();

	while (1) {
		update_gfit_control_state();
		process_uart();
		ARG_UNUSED(bridge_ready);
		k_msleep(GFIT_POLL_MS);
	}

	return 0;
}
