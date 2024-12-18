#include <zephyr/types.h>
#include <stddef.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/kernel.h>

#define DEVICE_NAME "Zephyr_Dk"
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)
#define UPDATE_INTERVAL K_SECONDS(5)  // Intervalo de atualização de 5 segundos

/*
 * Advertisement data.
 */
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL, 0xaa, 0xfe), // Eddystone UUID
    BT_DATA_BYTES(BT_DATA_SVC_DATA16,
                  0xaa, 0xfe, /* Eddystone UUID */
                  0x10,       /* Eddystone-URL frame type */
                  0x00,       /* Calibrated Tx power at 0m */
                  0x00,       /* URL Scheme Prefix http://www. */
                  'z', 'e', 'p', 'h', 'y', 'r',
                  'p', 'r', 'o', 'j', 'e', 'c', 't',
                  0x08) /* .org */
};

/* Scan Response data to include the device name */
static const struct bt_data sd[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

/* Global variables to track connection information */
static struct bt_conn *default_conn;
static int64_t connect_time;

/* Callback for connection */
static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        printk("Connection failed (err %u)\n", err);
        return;
    }

    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    default_conn = conn;
    connect_time = k_uptime_get(); // Capture the connection time
    printk("Connected to %s\n", addr);
}

/* Callback for disconnection */
static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];
    int64_t disconnect_time = k_uptime_get();

    if (default_conn) {
        bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
        printk("Disconnected from %s (reason %u)\n", addr, reason);

        /* Calculate and print the time connected */
        int64_t duration = disconnect_time - connect_time;
        int64_t duration_seconds = duration / 1000;
        printk("Connection duration: %lld seconds\n", duration_seconds);

        default_conn = NULL;
    }
}

/* Connection callbacks structure */
static struct bt_conn_cb conn_callbacks = {
    .connected = connected,
    .disconnected = disconnected,
};

/* Callback to indicate Bluetooth readiness */
static void bt_ready(int err)
{
    if (err) {
        printk("Bluetooth init failed (err %d)\n", err);
        return;
    }

    printk("Bluetooth initialized\n");

    /* Start advertising with connectable advertisement type */
    err = bt_le_adv_start(BT_LE_ADV_CONN, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    if (err) {
        printk("Advertising failed to start (err %d)\n", err);
        return;
    }

    printk("Beacon started advertising\n");
}

int main(void)
{
    int err;

    printk("Starting Beacon Demo with Name Zephyr_Dk\n");

    /* Initialize the Bluetooth Subsystem */
    err = bt_enable(bt_ready);
    if (err) {
        printk("Bluetooth init failed (err %d)\n", err);
        return 0;
    }

    /* Register connection callbacks */
    bt_conn_cb_register(&conn_callbacks);

    /* Infinite loop to show connection information every 5 seconds */
    while (1) {
        if (default_conn) {
            int64_t current_time = k_uptime_get();
            int64_t connected_duration = current_time - connect_time;
            int64_t connected_duration_sec = connected_duration / 1000;

            char addr[BT_ADDR_LE_STR_LEN];
            bt_addr_le_to_str(bt_conn_get_dst(default_conn), addr, sizeof(addr));

            printk("\nCurrently connected to %s", addr);
            printk("\nConnection Duration: %lld seconds", connected_duration_sec);
        } else {
            printk("\nNo active connection");
        }

        k_sleep(UPDATE_INTERVAL);  // Wait for 5 seconds before checking again
    }

    return 0;
}
