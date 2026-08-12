#include <stddef.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>

#define UART_DEVICE_NODE DT_NODELABEL(uart0)
#define EXT_FLASH_NODE DT_NODELABEL(external_flash)

#define IMG_BG_GRID_LIGHT 0x03
#define IMG_TILE1_BLANK_LIGHT 0x04
#define SF_LABEL0_DARK 0x11

#define NOR_SLOT_SIZE 0x39000U
#define NOR_SECTOR_SIZE 0x1000U

static const struct device *const uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);
static const struct device *const flash_dev = DEVICE_DT_GET(EXT_FLASH_NODE);
static const struct gpio_dt_spec led_ok = GPIO_DT_SPEC_GET(DT_ALIAS(debug_led0), gpios);
static const struct gpio_dt_spec led_err = GPIO_DT_SPEC_GET(DT_ALIAS(debug_led1), gpios);

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

static void display_cmd(const uint8_t *payload, size_t len)
{
	uint8_t buffer[32];
	uint16_t crc = display_crc_ccitt(payload, len);
	size_t pos = 0;

	if (!device_is_ready(uart_dev) || len > 25U) {
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

	for (size_t i = 0; i < pos; i++) {
		uart_poll_out(uart_dev, buffer[i]);
	}
}

static void display_show_image(uint8_t image)
{
	uint8_t payload[] = {0x80, image};

	display_cmd(payload, sizeof(payload));
}

static void display_show_text(const char *text)
{
	uint8_t payload[11] = {0x9f, SF_LABEL0_DARK};
	size_t len = 0;

	while (text[len] != '\0' && len < 9U) {
		payload[2U + len] = (uint8_t)text[len];
		len++;
	}

	display_cmd(payload, 2U + len);
}

static void show_status(const char *text)
{
	display_show_image(IMG_BG_GRID_LIGHT);
	display_show_image(IMG_TILE1_BLANK_LIGHT);
	display_show_text(text);
}

static void set_leds(bool ok)
{
	if (gpio_is_ready_dt(&led_ok)) {
		(void)gpio_pin_configure_dt(&led_ok, GPIO_OUTPUT_INACTIVE);
		(void)gpio_pin_set_dt(&led_ok, ok ? 1 : 0);
	}

	if (gpio_is_ready_dt(&led_err)) {
		(void)gpio_pin_configure_dt(&led_err, GPIO_OUTPUT_INACTIVE);
		(void)gpio_pin_set_dt(&led_err, ok ? 0 : 1);
	}
}

int main(void)
{
	int err = 0;

	k_msleep(500);
	show_status("NOR WIP");

	if (!device_is_ready(flash_dev)) {
		set_leds(false);
		show_status("NOR DEV");
		return 0;
	}

	for (uint32_t offset = 0; offset < NOR_SLOT_SIZE; offset += NOR_SECTOR_SIZE) {
		err = flash_erase(flash_dev, offset, NOR_SECTOR_SIZE);
		if (err != 0) {
			set_leds(false);
			show_status("NOR ERR");
			return 0;
		}
	}

	set_leds(true);
	show_status("NOR OK");

	while (1) {
		k_msleep(1000);
	}

	return 0;
}
