#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/ble.h>
#include <zmk/event_manager.h>
#include <zmk/events/ble_active_profile_changed.h>

#define LED_NODE_R DT_ALIAS(ledred)
#define LED_NODE_G DT_ALIAS(ledgreen)
#define LED_NODE_B DT_ALIAS(ledblue)

#if !DT_NODE_HAS_STATUS(LED_NODE_R, okay) || !DT_NODE_HAS_STATUS(LED_NODE_G, okay) || !DT_NODE_HAS_STATUS(LED_NODE_B, okay)
#error "Unsupported board: led devicetree alias is not defined"
#endif

#define RED 0
#define GREEN 1
#define BLUE 2

static const struct gpio_dt_spec LED_R = GPIO_DT_SPEC_GET(LED_NODE_R, gpios);
static const struct gpio_dt_spec LED_G = GPIO_DT_SPEC_GET(LED_NODE_G, gpios);
static const struct gpio_dt_spec LED_B = GPIO_DT_SPEC_GET(LED_NODE_B, gpios);

static void configure_gpio(const struct gpio_dt_spec *led, gpio_flags_t flags) {
    if (device_is_ready(led->port)) {
        gpio_pin_configure_dt(led, flags);
    } else {
        LOG_ERR("LED device %s is not ready", led->port->name);
    }
}

void reset_leds() {
    configure_gpio(&LED_R, GPIO_OUTPUT_HIGH);
    configure_gpio(&LED_G, GPIO_OUTPUT_HIGH);
    configure_gpio(&LED_B, GPIO_OUTPUT_HIGH);
}

void set_led_rgb(bool r, bool g, bool b) {
    reset_leds(); // Ensure all LEDs are off before setting the desired color
    if (r) {
        configure_gpio(&LED_R, GPIO_OUTPUT_LOW);
    }
    if (g) {
        configure_gpio(&LED_G, GPIO_OUTPUT_LOW);
    }
    if (b) {
        configure_gpio(&LED_B, GPIO_OUTPUT_LOW);
    }
}

int led_listener(const zmk_event_t *eh) {
    const struct zmk_ble_active_profile_changed *profile_ev = as_zmk_ble_active_profile_changed(eh);
    if (!profile_ev) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    switch (profile_ev->index) {
        case 0:
            set_led_rgb(true, false, false); // Red
            break;
        case 1:
            set_led_rgb(false, true, false); // Green
            break;
        case 2:
            set_led_rgb(false, false, true); // Blue
            break;
        case 3:
            set_led_rgb(true, true, false); // Yellow
            break;
        case 4:
            set_led_rgb(true, false, true); // Magenta
            break;
        default:
            reset_leds();
            break;
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(led_output_status, led_listener)
#if defined(CONFIG_ZMK_BLE)
    ZMK_SUBSCRIPTION(led_output_status, zmk_ble_active_profile_changed);
#endif
