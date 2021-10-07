/*
 * Copyright (c) 2020 The ZMK Contributors
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
/*
#define BUZZER_NODE DT_ALIAS(buzzer)

#if !DT_NODE_HAS_STATUS(BUZZER_NODE, okay)
    #error "Unsupported board: buzzer devicetree alias is not defined"
#endif

#define LED_R DT_GPIO_LABEL(BUZZER_NODE, gpios)
#define PIN_R DT_GPIO_PIN(BUZZER_NODE, gpios)
#define FLAGS_R DT_GPIO_FLAGS(BUZZER_NODE, gpios)

typedef struct cyber60_buzzer {
    const char *name;
    gpio_pin_t pin;
    gpio_flags_t flags;
} cyber60_led_t;
/*
void reset_leds()
{
    for (int i = 0; i < led_size; i++) {
        const struct device *dev = device_get_binding(leds[i].name);
        if (dev == NULL) {
            return;
        }
        gpio_pin_set(dev, leds[i].pin, false);
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


void led_work_handler(struct k_work *work) {
    reset_leds();
}

K_WORK_DEFINE(led_work, led_work_handler);

void led_expiry_function()
{
    k_work_submit(&led_work);
}

K_TIMER_DEFINE(led_timer, led_expiry_function, NULL);
*/
int buzzer_listener(const zmk_event_t *eh)
{
    //uint8_t index = zmk_ble_active_profile_index();

    /*
    k_timer_stop(&led_timer);

    reset_leds();
    switch(index) {
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
    k_timer_start(&led_timer, K_SECONDS(3), K_SECONDS(3));
*/
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(buzzer_output_status, buzzer_listener)
#if defined(CONFIG_ZMK_BLE)
ZMK_SUBSCRIPTION(buzzer_output_status, zmk_ble_active_profile_changed);
#endif

