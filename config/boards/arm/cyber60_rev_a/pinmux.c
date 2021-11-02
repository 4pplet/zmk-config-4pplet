/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>

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


static void configure_led(const char *name, gpio_pin_t pin, gpio_flags_t flags)
{
    const struct device *dev = device_get_binding(name);
    if (dev == NULL) {
        return;
    }

    int ret = gpio_pin_configure(dev, pin, GPIO_OUTPUT | flags);
    if (ret < 0) {
        return;
    }

    gpio_pin_set(dev, pin, false);

}


extern int led_listener(void *);

static int pwr_led_init(const struct device *dev) {
    configure_led(LED_R, PIN_R, FLAGS_R);
    configure_led(LED_G, PIN_G, FLAGS_G);
    configure_led(LED_B, PIN_B, FLAGS_B);

    // Massive HACK Alert !!!
    led_listener(NULL);
}

SYS_INIT(pwr_led_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
