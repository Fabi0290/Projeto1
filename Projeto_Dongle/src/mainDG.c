#include <zephyr/types.h>
#include <stddef.h>
#include <zephyr/sys/printk.h>
#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/hci.h>

// Endereço do dispositivo alvo
#define TARGET_ADDR "FE:7E:BC:C4:9F:1E"

// Declaração de funções
static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t adv_type, struct net_buf_simple *buf);

// Callback de conexão
static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        printk("Erro ao conectar: %u\n", err);
    } else {
        printk("Conectado ao dispositivo.\n");
    }
}

// Callback de desconexão
static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    printk("Desconectado (razão: %u)\n", reason);

    // Reinicia o scanner para procurar o dispositivo novamente
    int err = bt_le_scan_start(BT_LE_SCAN_ACTIVE, device_found);
    if (err) {
        printk("Erro ao reiniciar scanner: %d\n", err);
    } else {
        printk("Scanner reiniciado. Procurando pelo dispositivo: %s...\n", TARGET_ADDR);
    }
}

// Estruturas de callback
static struct bt_conn_cb conn_callbacks = {
    .connected = connected,
    .disconnected = disconnected,
};

// Variável para armazenar a conexão
static struct bt_conn *default_conn;

// Callback chamado quando dispositivos são encontrados
static void device_found(const bt_addr_le_t *addr, int8_t rssi, uint8_t adv_type, struct net_buf_simple *buf)
{
    char dev_addr[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(addr, dev_addr, sizeof(dev_addr));

    printk("Dispositivo encontrado: %s (RSSI: %d)\n", dev_addr, rssi);

    // Remove o tipo de endereço da string para comparação
    char stripped_addr[18];
    strncpy(stripped_addr, dev_addr, 17);
    stripped_addr[17] = '\0'; // Garante o término da string

    // Verifica se o dispositivo encontrado corresponde ao endereço alvo
    if (strcmp(stripped_addr, TARGET_ADDR) == 0) {
        printk("Dispositivo alvo encontrado: %s\n", dev_addr);

        // Para o scanner
        int err = bt_le_scan_stop();
        if (err) {
            printk("Erro ao parar o scanner: %d\n", err);
            return;
        }

        // Inicia a conexão
        err = bt_conn_le_create(addr, BT_CONN_LE_CREATE_CONN, BT_LE_CONN_PARAM_DEFAULT, &default_conn);
        if (err) {
            printk("Falha ao criar conexão: %d\n", err);
            return;
        }

        printk("Tentando conectar ao dispositivo: %s\n", dev_addr);
    }
}

void main(void)
{
    int err;

    printk("Inicializando BLE Scanner\n");

    // Inicializa o Bluetooth
    err = bt_enable(NULL);
    if (err) {
        printk("Erro ao inicializar Bluetooth: %d\n", err);
        return;
    }

    printk("Bluetooth inicializado\n");

    // Registra os callbacks de conexão
    bt_conn_cb_register(&conn_callbacks);

    // Inicia o scanner BLE
    err = bt_le_scan_start(BT_LE_SCAN_ACTIVE, device_found);
    if (err) {
        printk("Erro ao iniciar scanner: %d\n", err);
        return;
    }

    printk("Scanner iniciado. Procurando pelo dispositivo: %s...\n", TARGET_ADDR);
}
