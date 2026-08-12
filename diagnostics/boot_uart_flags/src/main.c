#include <stdint.h>
#include <stddef.h>

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>

#define UART_DEVICE_NODE DT_NODELABEL(uart0)

#define IMG_BG_GRID_LIGHT 0x03
#define IMG_TILE1_BLANK_LIGHT 0x04
#define IMG_TILE1_ENGLISH_ICON 0x84
#define IMG_TILE1_GERMAN_ICON 0x85
#define IMG_TILE1_FRENCH_ICON 0x86
#define IMG_TILE1_SPANISH_ICON 0x87
#define IMG_TILE1_US_ICON 0x88

#define SF_LABEL0_DARK 0x11

static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);
static const struct gpio_dt_spec led = {
	.port = DEVICE_DT_GET(DT_NODELABEL(gpio0)),
	.pin = 17,
	.dt_flags = GPIO_ACTIVE_HIGH,
};

static uint16_t display_crc_ccitt(const uint8_t *data, size_t len)
{
	uint16_t crc = 0x0000;

	for (size_t i = 0; i < len; i++) {
		crc ^= (uint16_t)data[i] << 8;
		for (uint8_t bit = 0; bit < 8; bit++) {
			if ((crc & 0x8000U) != 0U) {
				crc = (uint16_t)((crc << 1) ^ 0x1021U);
			} else {
				crc <<= 1;
			}
		}
	}

	return crc;
}

static void display_send(const uint8_t *data, size_t len)
{
	for (size_t i = 0; i < len; i++) {
		uart_poll_out(uart_dev, data[i]);
	}
}

static void display_cmd(const uint8_t *payload, size_t len)
{
	uint8_t buffer[32];
	uint16_t crc = display_crc_ccitt(payload, len);
	size_t pos = 0;

	if (len > 25U) {
		return;
	}

	buffer[pos++] = 0xaa;
	for (size_t i = 0; i < len; i++) {
		buffer[pos++] = payload[i];
	}
	buffer[pos++] = (uint8_t)(crc >> 8);
	buffer[pos++] = (uint8_t)crc;
	buffer[pos++] = 0xe4;
	buffer[pos++] = 0x1b;
	buffer[pos++] = 0x11;
	buffer[pos++] = 0xee;

	display_send(buffer, pos);
}

static void display_show_image(uint8_t image)
{
	uint8_t payload[] = {0x80, image};

	display_cmd(payload, sizeof(payload));
}

static void display_show_text(uint8_t field, const char *text)
{
	uint8_t payload[11] = {0x9f, field};
	size_t len = 0;

	while (text[len] != '\0' && len < 9U) {
		payload[2U + len] = (uint8_t)text[len];
		len++;
	}

	display_cmd(payload, 2U + len);
}

int main(void)
{
	static const uint8_t flags[] = {
		IMG_TILE1_ENGLISH_ICON,
		IMG_TILE1_GERMAN_ICON,
		IMG_TILE1_FRENCH_ICON,
		IMG_TILE1_SPANISH_ICON,
		IMG_TILE1_US_ICON,
	};
	size_t index = 0;

	if (!device_is_ready(uart_dev)) {
		return 0;
	}

	if (gpio_is_ready_dt(&led)) {
		(void)gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	}

	k_msleep(500);

	while (1) {
		display_show_image(IMG_BG_GRID_LIGHT);
		display_show_image(IMG_TILE1_BLANK_LIGHT);
		display_show_text(SF_LABEL0_DARK, "BOOT TEST");
		display_show_image(flags[index]);

		index = (index + 1U) % ARRAY_SIZE(flags);
		k_msleep(1000);
	}

	return 0;
}
