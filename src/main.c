/**
 * @file    main.c
 * @brief   CAN-FD Telemetrie & Diagnose-Knoten fuer STM32 NUCLEO-C562RE
 * @author  Ibrahim Al Saeed (Elektronik & Technische Informatik, HTL Wien 10)
 * @date    2026-10-01
 * 
 * Hardware:
 *   - Board:          STM32 NUCLEO-C562RE (Arm Cortex-M33 @ 24 MHz HSE)
 *   - CAN Transceiver: On-board CAN-FD (FDCAN1 an PB8 RX / PB9 TX, Standby an PE2)
 *   - User LED:       LD1 an PA5
 *   - User Button:    B1 an PC13 (Interrupt-gesteuert)
 *   - Konsole:        ST-LINK V3EC Virtual COM Port (115200 Baud, 8N1)
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/can.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/byteorder.h>

/* CAN-Identifier fuer Telemetrie */
#define CAN_TELEMETRY_ID       0x120
#define CAN_RX_FILTER_ID       0x100
#define CAN_RX_FILTER_MASK     0x7F0

/* Hardware DeviceTree Knoten */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
const struct device *const can_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));

/* Callback-Datenstruktur fuer Button-Interrupt */
static struct gpio_callback button_cb_data;
static volatile uint32_t button_press_count = 0;
static struct k_work can_tx_work;

/**
 * @brief CAN RX Callback-Funktion (wird bei eingehenden CAN-Frames aufgerufen)
 */
void can_rx_callback(const struct device *dev, struct can_frame *frame, void *user_data)
{
    ARG_UNUSED(dev);
    ARG_UNUSED(user_data);

    /* Grüne LED als Empfangsbestätigung kurz aufblitzen lassen */
    gpio_pin_toggle_dt(&led);

    printk("[CAN RX] ID: 0x%03X | DLC: %d | Data: ", frame->id, frame->dlc);
    for (int i = 0; i < frame->dlc; i++) {
        printk("%02X ", frame->data[i]);
    }
    printk("\n");
}

/**
 * @brief Workqueue-Handler zum Senden von CAN-FD Telemetrie
 */
static void can_tx_work_handler(struct k_work *work)
{
    ARG_UNUSED(work);

    struct can_frame frame = {0};
    frame.id = CAN_TELEMETRY_ID;
    frame.flags = CAN_FRAME_FDF | CAN_FRAME_BRS; /* CAN-FD mit Bit-Rate Switching */
    frame.dlc = 8;                               /* 8 Bytes Nutzdaten */

    uint32_t uptime_sec = k_uptime_get_32() / 1000;

    /* Nutzdaten-Payload packen */
    sys_put_be32(button_press_count, &frame.data[0]); /* Bytes 0-3: Zaehlerstand */
    sys_put_be32(uptime_sec,          &frame.data[4]); /* Bytes 4-7: Uptime in Sekunden */

    int ret = can_send(can_dev, &frame, K_MSEC(100), NULL, NULL);
    if (ret != 0) {
        printk("[-] Fehler beim Senden des CAN-Frames: %d\n", ret);
    } else {
        printk("[+] CAN-FD Frame gesendet! ID: 0x%03X | Taster: %u | Uptime: %u s\n",
               frame.id, button_press_count, uptime_sec);
        gpio_pin_toggle_dt(&led);
    }
}

/**
 * @brief Interrupt Service Routine fuer den User-Button (PC13)
 */
void button_pressed_isr(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    ARG_UNUSED(dev);
    ARG_UNUSED(cb);
    ARG_UNUSED(pins);

    button_press_count++;
    /* Senden in den Workqueue-Thread auslagern, um die ISR kurz zu halten */
    k_work_submit(&can_tx_work);
}

int main(void)
{
    int ret;

    printk("\n=======================================================\n");
    printk("  STM32 NUCLEO-C562RE CAN-FD Telemetrie-Knoten         \n");
    printk("  Entwickler: Ibrahim Al Saeed (HTL Wien 10)           \n");
    printk("  Architektur: Arm Cortex-M33 | RTOS: Zephyr           \n");
    printk("=======================================================\n\n");

    /* 1. User-LED initialisieren */
    if (!gpio_is_ready_dt(&led)) {
        printk("[-] Fehler: LED GPIO nicht bereit!\n");
        return 0;
    }
    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        printk("[-] Fehler beim Konfigurieren der LED: %d\n", ret);
        return 0;
    }

    /* 2. User-Button mit Interrupt initialisieren */
    if (!gpio_is_ready_dt(&button)) {
        printk("[-] Fehler: Button GPIO nicht bereit!\n");
        return 0;
    }
    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_init_callback(&button_cb_data, button_pressed_isr, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);

    /* 3. Workqueue initialisieren */
    k_work_init(&can_tx_work, can_tx_work_handler);

    /* 4. CAN-FD Controller (FDCAN1) initialisieren */
    if (!device_is_ready(can_dev)) {
        printk("[-] Fehler: CAN-FD Geraet nicht bereit!\n");
        return 0;
    }

    /* CAN-FD Modus aktivieren & Bitraten einstellen */
    ret = can_set_mode(can_dev, CAN_MODE_FD);
    if (ret < 0) {
        printk("[-] Fehler beim Einstellen des CAN-FD Modus: %d\n", ret);
    }

    /* Bitrate: 500 kbit/s Nominal, 2000 kbit/s Data (CAN-FD BRS) */
    ret = can_set_bitrate(can_dev, 500000);
    ret = can_set_bitrate_data(can_dev, 2000000);

    /* CAN-Treiber starten */
    ret = can_start(can_dev);
    if (ret < 0) {
        printk("[-] CAN-Treiber konnte nicht gestartet werden: %d\n", ret);
        return 0;
    }

    /* Empfangsfilter setzen */
    const struct can_filter rx_filter = {
        .id = CAN_RX_FILTER_ID,
        .mask = CAN_RX_FILTER_MASK,
        .flags = CAN_FILTER_DATA | CAN_FILTER_FDF
    };
    can_add_rx_filter(can_dev, can_rx_callback, NULL, &rx_filter);

    printk("[✓] CAN-FD Controller erfolgreich initialisiert (500k/2M Bitrate)\n");
    printk("[✓] System bereit. Druecke den blauen User-Button (B1), um CAN-Frames zu senden!\n\n");

    /* Zyklischer Heartbeat (alle 5 Sekunden) */
    while (1) {
        k_sleep(K_SECONDS(5));
        printk("[Heartbeat] System laeuft... Uptime: %u Sekunden\n", k_uptime_get_32() / 1000);
    }

    return 0;
}
