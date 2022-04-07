/*
 * Copyright (c) 2021 Megamind (megamind4089)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <kernel.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>

#include <bluetooth/services/bas.h>

#include <logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/ble.h>
#include <zmk/event_manager.h>
#include <zmk/events/ble_active_profile_changed.h>

#define LED_NODE_R DT_ALIAS(ledred)
#define LED_NODE_G DT_ALIAS(ledgreen)
#define LED_NODE_B DT_ALIAS(ledblue)

#if !DT_NODE_HAS_STATUS(LED_NODE_R, okay)
#if !DT_NODE_HAS_STATUS(LED_NODE_G, okay)
#if !DT_NODE_HAS_STATUS(LED_NODE_B, okay)
    #error "Unsupported board: led devicetree alias is not defined"
#endif
#endif
#endif

#define LED_R DT_GPIO_LABEL(LED_NODE_R, gpios)
#define PIN_R DT_GPIO_PIN(LED_NODE_R, gpios)
#define FLAGS_R DT_GPIO_FLAGS(LED_NODE_R, gpios)

#define LED_G DT_GPIO_LABEL(LED_NODE_G, gpios)
#define PIN_G DT_GPIO_PIN(LED_NODE_G, gpios)
#define FLAGS_G DT_GPIO_FLAGS(LED_NODE_G, gpios)

#define LED_B DT_GPIO_LABEL(LED_NODE_B, gpios)
#define PIN_B DT_GPIO_PIN(LED_NODE_B, gpios)
#define FLAGS_B DT_GPIO_FLAGS(LED_NODE_B, gpios)


typedef struct cyber60_led {
    const char *name;
    gpio_pin_t pin;
    gpio_flags_t flags;
} cyber60_led_t;


static const cyber60_led_t leds[] = {
    {LED_R, PIN_R, FLAGS_R},
    {LED_G, PIN_G, FLAGS_G},
    {LED_B, PIN_B, FLAGS_B}
};

#define RED 0
#define GREEN 1
#define BLUE 2

static const size_t led_size = sizeof(leds)/sizeof(leds[0]);

void reset_leds()
{
    for (int index = 0; index < led_size; index++) {
        const struct device *dev = device_get_binding(leds[index].name);
        if (dev == NULL) {
            return;
        }
        gpio_pin_set(dev, leds[index].pin, false);
    }
}

void set_led(size_t index)
{
    const struct device *dev = device_get_binding(leds[index].name);
    if (dev == NULL) {
        return;
    }
    gpio_pin_set(dev, leds[index].pin, true);
}

static int pwr_led_init(const struct device *dev) {
    int ret;

    for (int index=0; index < sizeof(leds)/sizeof(leds[0]); index++) {
        dev = device_get_binding(leds[index].name);
        if (dev == NULL) {
            return -EIO;
        }

        ret = gpio_pin_configure(dev, leds[index].pin, GPIO_OUTPUT | leds[index].flags);
        if (ret < 0) {
            return -EIO;
        }
        // set to false so no constant battery drain, just testing for now
        gpio_pin_set(dev, leds[index].pin, false);
    }

    return 0;
}

void led_work_handler(struct k_work *work) {
    reset_leds();
}

K_WORK_DEFINE(led_work, led_work_handler);

void led_expiry_function()
{
    k_work_submit(&led_work);
}

K_TIMER_DEFINE(led_timer, led_expiry_function, NULL);

int led_listener(const zmk_event_t *eh)
{
    const struct zmk_ble_active_profile_changed *profile_ev = NULL;
    // uint8_t index = zmk_ble_active_profile_index();

    if ((profile_ev = as_zmk_ble_active_profile_changed(eh)) == NULL) {
        return ZMK_EV_EVENT_BUBBLE;
    }
    LOG_WRN("Active profile index:%d", profile_ev->index);

    k_timer_stop(&led_timer);

    reset_leds();
    switch(profile_ev->index) {
        case 0:
            set_led(RED);
            break;
        case 1:
            set_led(GREEN);
            break;
        case 2:
            set_led(BLUE);
            break;
        case 3:
            set_led(RED);
            set_led(GREEN);
            break;
        case 4:
            set_led(BLUE);
            set_led(GREEN);
            break;
        default:
            break;
    }
    k_timer_start(&led_timer, K_SECONDS(3), K_NO_WAIT);

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(led_output_status, led_listener)
#if defined(CONFIG_ZMK_BLE)
    ZMK_SUBSCRIPTION(led_output_status, zmk_ble_active_profile_changed);
#endif

SYS_INIT(pwr_led_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
