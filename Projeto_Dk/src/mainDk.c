#include <zephyr/types.h>
#include <stddef.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h> // Biblioteca UART

#define DEVICE_NAME "Zephyr_Dk" //nome dispositivo 
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)
#define UPDATE_INTERVAL K_SECONDS(5)  // Intervalo atualizacao 5 seg

// UART 
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_console)
static const struct device *uart_dev;

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

/* Scan Response */
static const struct bt_data sd[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
};

/* vars */
static struct bt_conn *default_conn;
static int64_t connect_time;

/* func mensagem via UART */
void send_uart_message(const char *message)
{
    if (uart_dev) {
        while (*message) {
            uart_poll_out(uart_dev, *message++); // Envia via UART
        }
        uart_poll_out(uart_dev, '\n'); //nova linha
    }
}

/* Callback conexao blueTooth */
static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        printk("Connection failed (err %u)\n", err);
        send_uart_message("ligado: false"); // Mensg UART
        return;
    }

    char addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    default_conn = conn; //armazena conec ativa
    connect_time = k_uptime_get(); // conexao time
    printk("Connected to %s\n", addr);
    send_uart_message("ligado: true"); // Mensg UART
}

/* Callback perca conexao */
static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    char addr[BT_ADDR_LE_STR_LEN];
    int64_t disconnect_time = k_uptime_get(); //tempo disconnect

    if (default_conn) {
        bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));
        printk("Disconnected from %s (reason %u)\n", addr, reason);

        //calcular tempo conexao 
        int64_t duration = disconnect_time - connect_time;
        int64_t duration_seconds = duration / 1000;// Conversao segundos
        printk("Connection duration: %lld seconds\n", duration_seconds);

        default_conn = NULL;
    }
    send_uart_message("ligado: false"); // Mensg UART
}

/* Connection callbacks structure */
static struct bt_conn_cb conn_callbacks = {
    .connected = connected,
    .disconnected = disconnected,
};

/* Callback to indicate Bluetooth */
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

// INICIO MAIN
int main(void)
{
    int err;
    //START
    printk("Starting Beacon Demo Zephyr_Dk\n");

    /* Initialize the UART device */
    uart_dev = DEVICE_DT_GET(UART_DEVICE_NODE);
    if (!device_is_ready(uart_dev)) {
        printk("UART device not ready\n");
        return -1;
    }

    /* Initialize the Bluetooth*/
    err = bt_enable(bt_ready);
    if (err) {
        printk("Bluetooth init failed (err %d)\n", err);
        return 0;
    }

    /* Register connection callbacks */
    bt_conn_cb_register(&conn_callbacks);

    //Loop 5s
    while (1) {
        if (default_conn) {
            int64_t current_time = k_uptime_get();
            int64_t connected_duration = current_time - connect_time;
            int64_t connected_duration_sec = connected_duration / 1000;// Conversao segundos

            char addr[BT_ADDR_LE_STR_LEN];
            bt_addr_le_to_str(bt_conn_get_dst(default_conn), addr, sizeof(addr));

            printk("\nCurrently connected to %s", addr);
            printk("\nConnection Duration: %lld seconds", connected_duration_sec);
        } else {
            printk("\nNo active connection");
        }

        k_sleep(UPDATE_INTERVAL);  // verifificar em 5s
    }

    return 0;
}
