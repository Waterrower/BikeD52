#include "ble_ftms.h"

#include <errno.h>
#include <stddef.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/sys/byteorder.h>

#define BT_UUID_FTMS_VAL 0x1826
#define BT_UUID_FTMS_FEATURE_VAL 0x2acc
#define BT_UUID_INDOOR_BIKE_DATA_VAL 0x2ad2

static struct bt_conn *current_conn;
static bool bt_ready;
static bool ftms_enabled;
static bool ftms_advertising;
static bool indoor_bike_notify_enabled;
static ble_ftms_conn_cb_t connection_cb;

static const struct bt_uuid_16 ftms_uuid = BT_UUID_INIT_16(BT_UUID_FTMS_VAL);
static const struct bt_uuid_16 ftms_feature_uuid =
	BT_UUID_INIT_16(BT_UUID_FTMS_FEATURE_VAL);
static const struct bt_uuid_16 indoor_bike_data_uuid =
	BT_UUID_INIT_16(BT_UUID_INDOOR_BIKE_DATA_VAL);

static const uint8_t ftms_feature[8] = {
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
};

static const struct bt_data advertising_data[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL, BT_UUID_16_ENCODE(BT_UUID_FTMS_VAL)),
};

static const struct bt_data scan_response_data[] = {
	BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME,
		sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

static ssize_t read_ftms_feature(struct bt_conn *conn,
				 const struct bt_gatt_attr *attr,
				 void *buf, uint16_t len, uint16_t offset)
{
	return bt_gatt_attr_read(conn, attr, buf, len, offset,
				 ftms_feature, sizeof(ftms_feature));
}

static void indoor_bike_ccc_changed(const struct bt_gatt_attr *attr,
				    uint16_t value)
{
	ARG_UNUSED(attr);
	indoor_bike_notify_enabled = (value == BT_GATT_CCC_NOTIFY);
}

BT_GATT_SERVICE_DEFINE(ftms_service,
	BT_GATT_PRIMARY_SERVICE(&ftms_uuid.uuid),
	BT_GATT_CHARACTERISTIC(&ftms_feature_uuid.uuid,
			       BT_GATT_CHRC_READ,
			       BT_GATT_PERM_READ,
			       read_ftms_feature, NULL, NULL),
	BT_GATT_CHARACTERISTIC(&indoor_bike_data_uuid.uuid,
			       BT_GATT_CHRC_NOTIFY,
			       BT_GATT_PERM_NONE,
			       NULL, NULL, NULL),
	BT_GATT_CCC(indoor_bike_ccc_changed,
		    BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

static int start_advertising(void)
{
	int err;

	if (!bt_ready || ftms_advertising || current_conn != NULL) {
		return 0;
	}

	err = bt_le_adv_start(BT_LE_ADV_CONN_FAST_2,
			      advertising_data, ARRAY_SIZE(advertising_data),
			      scan_response_data, ARRAY_SIZE(scan_response_data));
	if (err == -EALREADY) {
		err = 0;
	}

	if (err == 0) {
		ftms_advertising = true;
	}

	return err;
}

static int stop_advertising(void)
{
	int err;

	if (!ftms_advertising) {
		return 0;
	}

	err = bt_le_adv_stop();
	if (err == -EALREADY || err == -EINVAL || err == -EACCES) {
		err = 0;
	}

	if (err == 0) {
		ftms_advertising = false;
	}

	return err;
}

static void connected(struct bt_conn *conn, uint8_t err)
{
	struct bt_conn_info info;

	if (bt_conn_get_info(conn, &info) != 0 ||
	    info.role != BT_CONN_ROLE_PERIPHERAL) {
		return;
	}

	if (err != 0) {
		if (ftms_enabled) {
			(void)start_advertising();
		}
		return;
	}

	current_conn = bt_conn_ref(conn);
	ftms_advertising = false;
	if (connection_cb != NULL) {
		connection_cb(true);
	}
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
	ARG_UNUSED(reason);

	if (current_conn != conn) {
		return;
	}

	bt_conn_unref(current_conn);
	current_conn = NULL;
	indoor_bike_notify_enabled = false;
	if (connection_cb != NULL) {
		connection_cb(false);
	}

	if (ftms_enabled) {
		(void)start_advertising();
	}
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
	.connected = connected,
	.disconnected = disconnected,
};

int ble_ftms_init(ble_ftms_conn_cb_t cb)
{
	int err;

	connection_cb = cb;
	if (bt_ready) {
		return 0;
	}

	err = bt_enable(NULL);
	if (err == 0) {
		bt_ready = true;
	}

	return err;
}

int ble_ftms_set_enabled(bool enabled)
{
	int err;

	if (!bt_ready) {
		err = ble_ftms_init(connection_cb);
		if (err != 0) {
			return err;
		}
	}

	if (enabled) {
		err = start_advertising();
		if (err == 0) {
			ftms_enabled = true;
		}
		return err;
	}

	ftms_enabled = false;
	err = stop_advertising();
	if (current_conn != NULL) {
		int disconnect_err = bt_conn_disconnect(current_conn,
							BT_HCI_ERR_REMOTE_USER_TERM_CONN);
		if (err == 0) {
			err = disconnect_err;
		}
	}

	return err;
}

bool ble_ftms_is_enabled(void)
{
	return ftms_enabled;
}

bool ble_ftms_is_connected(void)
{
	return current_conn != NULL;
}

int ble_ftms_notify_indoor_bike(uint16_t speed_centi_kmh,
				uint16_t cadence_half_rpm,
				int16_t resistance_percent,
				int16_t power_watt)
{
	uint8_t data[10];

	if (current_conn == NULL || !indoor_bike_notify_enabled) {
		return 0;
	}

	sys_put_le16(0x0064, &data[0]);
	sys_put_le16(speed_centi_kmh, &data[2]);
	sys_put_le16(cadence_half_rpm, &data[4]);
	sys_put_le16((uint16_t)resistance_percent, &data[6]);
	sys_put_le16((uint16_t)power_watt, &data[8]);

	return bt_gatt_notify(current_conn, &ftms_service.attrs[4],
			      data, sizeof(data));
}
