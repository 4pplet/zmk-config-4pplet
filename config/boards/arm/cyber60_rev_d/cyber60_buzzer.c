/* Copyright (c) 2021 Megamind (megamind4089)
 * Copyright (c) 2021 The ZMK Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>

#include <zephyr/bluetooth/services/bas.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/ble.h>
#include <zmk/event_manager.h>
#include <zmk/events/ble_active_profile_changed.h>

#define BUZZER_NODE DT_ALIAS(buzzer)

#if !DT_NODE_HAS_STATUS(BUZZER_NODE, okay)
#error "Unsupported board: buzzer devicetree alias is not defined"
#else

#endif

#define BEEP_DURATION  K_MSEC(60)

static const struct pwm_dt_spec pwm = PWM_DT_SPEC_GET(BUZZER_NODE);

void _play(uint32_t period)
{
	if (!device_is_ready(pwm.dev)) {
		printk("Error: PWM device %s is not ready\n",
		       pwm.dev->name);
		return;
	}
    pwm_set_dt(&pwm, period, period / 2U); // attempt at new implementation
    k_sleep(BEEP_DURATION);
    pwm_set_dt(&pwm, 0, 0); // attempt at new implementation
    k_sleep(K_MSEC(50));
}

void play_sound_1()
{
    _play(1000000);
    _play(500000);
    _play(250000);
    _play(100000);
    _play(50000);
}

void play_sound_2()
{
    _play(1500000);
    _play(3900000);
    _play(1500000);
    _play(1500000);
}

void play_sound_3()
{
    _play(1500000);
    _play(3900000);
}

void play_sound_4()
{
    _play(2000000);
    _play(3900000);
}

void play_sound_5()
{
    _play(2500000);
    _play(3900000);               
}

int buzzer_listener(const zmk_event_t *eh)
{
    const struct zmk_ble_active_profile_changed *profile_ev = NULL;
    if ((profile_ev = as_zmk_ble_active_profile_changed(eh)) == NULL) {
        return ZMK_EV_EVENT_BUBBLE;
    }
    switch(profile_ev->index) {
        case 0:
            play_sound_1();
            break;
        case 1:
            play_sound_2();
            break;
        case 2:
            play_sound_3();
            break;
        case 3:
            play_sound_4();
            break;
        case 4:
            play_sound_5();
            break;
        default:
            break;
    }
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(buzzer_output_status, buzzer_listener)
#if defined(CONFIG_ZMK_BLE)
    ZMK_SUBSCRIPTION(buzzer_output_status, zmk_ble_active_profile_changed);
#endif
