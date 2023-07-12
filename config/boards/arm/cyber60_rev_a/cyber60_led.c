/*
 * Copyright (c) 2021 Megamind (megamind4089)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

#include <zephyr/bluetooth/services/bas.h>

#include <zephyr/logging/log.h>
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

#define RED 0
#define GREEN 1
#define BLUE 2

static const struct gpio_dt_spec LED_R = GPIO_DT_SPEC_GET(LED_NODE_R, gpios);
static const struct gpio_dt_spec LED_G = GPIO_DT_SPEC_GET(LED_NODE_G, gpios);
static const struct gpio_dt_spec LED_B = GPIO_DT_SPEC_GET(LED_NODE_B, gpios);

void reset_leds() {
    if (!device_is_ready(LED_R.port)) {
            return;
    }
    else
        gpio_pin_configure_dt(&LED_R, GPIO_DISCONNECTED);
    if (!device_is_ready(LED_G.port)) {
            return;
    }
    else
        gpio_pin_configure_dt(&LED_G, GPIO_DISCONNECTED);
    if (!device_is_ready(LED_B.port)) {
            return;
    }
    else
        gpio_pin_configure_dt(&LED_B, GPIO_DISCONNECTED);
}
void set_led(size_t index) {
    if(index == RED){
        int ret;
        if (!device_is_ready(LED_R.port)) {
            return;
        }
        ret = gpio_pin_configure_dt(&LED_R, GPIO_OUTPUT_LOW);
    }
    else if(index == GREEN){
        int ret;
        if (!device_is_ready(LED_G.port)) {
            return;
        }
        ret = gpio_pin_configure_dt(&LED_G, GPIO_OUTPUT_LOW);
    }
    else if(index == BLUE){
        int ret;
        if (!device_is_ready(LED_B.port)) {
            return;
        }
        ret = gpio_pin_configure_dt(&LED_B, GPIO_OUTPUT_LOW);
    }
    return;
}

void led_work_handler(struct k_work *work) { reset_leds(); }

K_WORK_DEFINE(led_work, led_work_handler);

void led_expiry_function() { k_work_submit(&led_work); }

K_TIMER_DEFINE(led_timer, led_expiry_function, NULL);

int led_listener(const zmk_event_t *eh) {
    const struct zmk_ble_active_profile_changed *profile_ev = NULL;

    if ((profile_ev = as_zmk_ble_active_profile_changed(eh)) == NULL) {
        return ZMK_EV_EVENT_BUBBLE;
    }
    LOG_WRN("Active profile index:%d", profile_ev->index);

    k_timer_stop(&led_timer);

    reset_leds();
    switch (profile_ev->index) {
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