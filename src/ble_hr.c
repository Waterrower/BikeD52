#include "ble_hr.h"

#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>

#include "ble_ftms.h"

#define HR_SCAN_MS 5000U
#define HR_NAME_MAX 10U

struct hr_candidate {
	bt_addr_le_t addr;
	int8_t rssi;
	char name[HR_NAME_MAX + 1U];
	bool found;
};

struct adv_parse_context {
	bool has_hrs;
	char name[HR_NAME_MAX + 1U];
};

static struct bt_conn *hr_conn;
static struct bt_uuid_16 discover_uuid = BT_UUID_INIT_16(0);
static struct bt_gatt_discover_params discover_params;
static struct bt_gatt_subscribe_params subscribe_params;
static struct hr_candidate best_candidate;
static enum ble_hr_state hr_state = BLE_HR_IDLE;
static uint32_t scan_started_ms;
static uint8_t hr_bpm;
static char hr_name[HR_NAME_MAX + 1U] = "Sensor";
static int8_t hr_rssi;

static bool adv_parse(struct bt_data *data, void *user_data)
{
	struct adv_parse_context *context = user_data;

	switch (data->type) {
	case BT_DATA_UUID16_SOME:
	case BT_DATA_UUID16_ALL:
		for (uint8_t i = 0; i + sizeof(uint16_t) <= data->data_len;
		     i += sizeof(uint16_t)) {
			uint16_t uuid = sys_get_le16(&data->data[i]);

			if (uuid == BT_UUID_HRS_VAL) {
				context->has_hrs = true;
			}
		}
		break;
	case BT_DATA_NAME_SHORTENED:
	case BT_DATA_NAME_COMPLETE:
		memcpy(context->name, data->data,
		       MIN(data->data_len, HR_NAME_MAX));
		context->name[MIN(data->data_len, HR_NAME_MAX)] = '\0';
		break;
	default:
		break;
	}

	return true;
}

static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t type,
			 struct net_buf_simple *ad)
{
	struct adv_parse_context context = {0};

	if (hr_state != BLE_HR_SCANNING) {
		return;
	}

	if (type != BT_GAP_ADV_TYPE_ADV_IND &&
	    type != BT_GAP_ADV_TYPE_ADV_DIRECT_IND &&
	    type != BT_GAP_ADV_TYPE_EXT_ADV) {
		return;
	}

	bt_data_parse(ad, adv_parse, &context);
	if (!context.has_hrs) {
		return;
	}

	if (!best_candidate.found || rssi > best_candidate.rssi) {
		bt_addr_le_copy(&best_candidate.addr, addr);
		best_candidate.rssi = rssi;
		best_candidate.found = true;
		if (context.name[0] != '\0') {
			memcpy(best_candidate.name, context.name,
			       sizeof(best_candidate.name));
		} else {
			strcpy(best_candidate.name, "HR Sensor");
		}
	}
}

static uint8_t notify_func(struct bt_conn *conn,
			   struct bt_gatt_subscribe_params *params,
			   const void *data, uint16_t length)
{
	const uint8_t *measurement = data;

	ARG_UNUSED(conn);

	if (data == NULL) {
		params->value_handle = 0U;
		hr_state = BLE_HR_IDLE;
		hr_bpm = 0U;
		return BT_GATT_ITER_STOP;
	}

	if (length < 2U) {
		return BT_GATT_ITER_CONTINUE;
	}

	if ((measurement[0] & 0x01U) != 0U) {
		if (length >= 3U) {
			uint16_t bpm16 = sys_get_le16(&measurement[1]);

			hr_bpm = bpm16 > 255U ? 255U : (uint8_t)bpm16;
		}
	} else {
		hr_bpm = measurement[1];
	}

	return BT_GATT_ITER_CONTINUE;
}

static uint8_t discover_func(struct bt_conn *conn,
			     const struct bt_gatt_attr *attr,
			     struct bt_gatt_discover_params *params)
{
	int err;

	if (attr == NULL) {
		memset(params, 0, sizeof(*params));
		if (hr_state == BLE_HR_CONNECTING) {
			hr_state = BLE_HR_FAILED;
		}
		return BT_GATT_ITER_STOP;
	}

	if (!bt_uuid_cmp(discover_params.uuid, BT_UUID_HRS)) {
		memcpy(&discover_uuid, BT_UUID_HRS_MEASUREMENT,
		       sizeof(discover_uuid));
		discover_params.uuid = &discover_uuid.uuid;
		discover_params.start_handle = attr->handle + 1U;
		discover_params.type = BT_GATT_DISCOVER_CHARACTERISTIC;

		err = bt_gatt_discover(conn, &discover_params);
		if (err != 0) {
			hr_state = BLE_HR_FAILED;
		}
		return BT_GATT_ITER_STOP;
	}

	if (!bt_uuid_cmp(discover_params.uuid, BT_UUID_HRS_MEASUREMENT)) {
		memcpy(&discover_uuid, BT_UUID_GATT_CCC, sizeof(discover_uuid));
		discover_params.uuid = &discover_uuid.uuid;
		discover_params.start_handle = attr->handle + 2U;
		discover_params.type = BT_GATT_DISCOVER_DESCRIPTOR;
		subscribe_params.value_handle = bt_gatt_attr_value_handle(attr);

		err = bt_gatt_discover(conn, &discover_params);
		if (err != 0) {
			hr_state = BLE_HR_FAILED;
		}
		return BT_GATT_ITER_STOP;
	}

	subscribe_params.notify = notify_func;
	subscribe_params.value = BT_GATT_CCC_NOTIFY;
	subscribe_params.ccc_handle = attr->handle;

	err = bt_gatt_subscribe(conn, &subscribe_params);
	if (err == 0 || err == -EALREADY) {
		hr_state = BLE_HR_CONNECTED;
	} else {
		hr_state = BLE_HR_FAILED;
	}

	return BT_GATT_ITER_STOP;
}

static void start_discovery(void)
{
	int err;

	if (hr_conn == NULL) {
		hr_state = BLE_HR_FAILED;
		return;
	}

	memcpy(&discover_uuid, BT_UUID_HRS, sizeof(discover_uuid));
	discover_params.uuid = &discover_uuid.uuid;
	discover_params.func = discover_func;
	discover_params.start_handle = BT_ATT_FIRST_ATTRIBUTE_HANDLE;
	discover_params.end_handle = BT_ATT_LAST_ATTRIBUTE_HANDLE;
	discover_params.type = BT_GATT_DISCOVER_PRIMARY;

	err = bt_gatt_discover(hr_conn, &discover_params);
	if (err != 0) {
		hr_state = BLE_HR_FAILED;
	}
}

static void connected(struct bt_conn *conn, uint8_t conn_err)
{
	struct bt_conn_info info;

	if (bt_conn_get_info(conn, &info) != 0 ||
	    info.role != BT_CONN_ROLE_CENTRAL) {
		return;
	}

	if (conn != hr_conn) {
		return;
	}

	if (conn_err != 0U) {
		bt_conn_unref(hr_conn);
		hr_conn = NULL;
		hr_state = BLE_HR_FAILED;
		return;
	}

	start_discovery();
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	ARG_UNUSED(reason);

	if (conn != hr_conn) {
		return;
	}

	bt_conn_unref(hr_conn);
	hr_conn = NULL;
	hr_bpm = 0U;
	if (hr_state == BLE_HR_CONNECTED || hr_state == BLE_HR_CONNECTING) {
		hr_state = BLE_HR_IDLE;
	}
}

BT_CONN_CB_DEFINE(hr_conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};

int ble_hr_start_pairing(void)
{
	struct bt_le_scan_param scan_param = {
		.type = BT_LE_SCAN_TYPE_ACTIVE,
		.options = BT_LE_SCAN_OPT_NONE,
		.interval = BT_GAP_SCAN_FAST_INTERVAL,
		.window = BT_GAP_SCAN_FAST_WINDOW,
	};
	int err;

	err = ble_ftms_init();
	if (err != 0) {
		return err;
	}

	if (hr_conn != NULL) {
		(void)ble_hr_disconnect();
	}

	memset(&best_candidate, 0, sizeof(best_candidate));
	hr_bpm = 0U;
	hr_rssi = 0;
	strcpy(hr_name, "Suche");

	(void)ble_ftms_pause_advertising();

	err = bt_le_scan_start(&scan_param, device_found);
	if (err != 0) {
		hr_state = BLE_HR_FAILED;
		(void)ble_ftms_resume_advertising();
		return err;
	}

	scan_started_ms = k_uptime_get_32();
	hr_state = BLE_HR_SCANNING;
	return 0;
}

int ble_hr_disconnect(void)
{
	int err = 0;
	bool was_scanning = hr_state == BLE_HR_SCANNING;

	if (was_scanning) {
		err = bt_le_scan_stop();
		(void)ble_ftms_resume_advertising();
	}

	if (hr_conn != NULL) {
		int disconnect_err = bt_conn_disconnect(hr_conn,
							BT_HCI_ERR_REMOTE_USER_TERM_CONN);
		if (err == 0) {
			err = disconnect_err;
		}
	}

	hr_state = BLE_HR_IDLE;
	hr_bpm = 0U;
	return err;
}

void ble_hr_process(void)
{
	struct bt_conn_le_create_param *create_param;
	struct bt_le_conn_param *param;
	int err;

	if (hr_state != BLE_HR_SCANNING) {
		return;
	}

	if ((uint32_t)(k_uptime_get_32() - scan_started_ms) < HR_SCAN_MS) {
		return;
	}

	(void)bt_le_scan_stop();
	(void)ble_ftms_resume_advertising();

	if (!best_candidate.found) {
		hr_state = BLE_HR_FAILED;
		strcpy(hr_name, "Kein Gurt");
		return;
	}

	create_param = BT_CONN_LE_CREATE_CONN;
	param = BT_LE_CONN_PARAM_DEFAULT;
	err = bt_conn_le_create(&best_candidate.addr, create_param, param,
				&hr_conn);
	if (err != 0) {
		hr_state = BLE_HR_FAILED;
		strcpy(hr_name, "Fehler");
		return;
	}

	memcpy(hr_name, best_candidate.name, sizeof(hr_name));
	hr_rssi = best_candidate.rssi;
	hr_state = BLE_HR_CONNECTING;
}

enum ble_hr_state ble_hr_get_state(void)
{
	return hr_state;
}

bool ble_hr_is_connected(void)
{
	return hr_state == BLE_HR_CONNECTED;
}

uint8_t ble_hr_get_bpm(void)
{
	return hr_bpm;
}

int8_t ble_hr_get_rssi(void)
{
	return hr_rssi;
}

const char *ble_hr_get_name(void)
{
	return hr_name;
}
