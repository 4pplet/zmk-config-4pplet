/* Copyright (c) 2021 Megamind (megamind4089)
 * Copyright (c) 2021 The ZMK Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <kernel.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/pwm.h>

#include <bluetooth/services/bas.h>

#include <logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/ble.h>
#include <zmk/event_manager.h>
#include <zmk/events/ble_active_profile_changed.h>

#define BUZZER_NODE DT_ALIAS(buzzer)

#if !DT_NODE_HAS_STATUS(BUZZER_NODE, okay)
#error "Unsupported board: buzzer devicetree alias is not defined"
#define BUZZ_LABEL ""
#define BUZZ_CHANNEL 0
#define BUZZ_FLAGS 0
#else
#define BUZZ_LABEL DT_LABEL(DT_PWMS_CTLR(BUZZER_NODE))
#define BUZZ_CHANNEL DT_PWMS_CHANNEL(BUZZER_NODE)
#define BUZZ_FLAGS DT_PWMS_FLAGS(BUZZER_NODE)
#endif

#define PERIOD_MIN     50
#define PERIOD_MAX     3900
#define PERIOD_INIT    1500

#define BEEP_DURATION  K_MSEC(60)

void _play(const struct device *pwm, uint32_t period)
{
    pwm_pin_set_usec(pwm, BUZZ_CHANNEL, period, period / 2U, BUZZ_FLAGS);
    k_sleep(BEEP_DURATION);

    pwm_pin_set_usec(pwm, BUZZ_CHANNEL, 0, 0, BUZZ_FLAGS);
    k_sleep(K_MSEC(50));

}

void play_sound_1(const struct device *pwm)
{
    _play(pwm, 1000);
    _play(pwm, 500);
    _play(pwm, 250);
    _play(pwm, 100);
    _play(pwm, 50);
}

void play_sound_2(const struct device *pwm)
{
    _play(pwm, 1500);
    _play(pwm, 3900);
    _play(pwm, 1500);
    _play(pwm, 1500);
}

void play_sound_3(const struct device *pwm)
{
    _play(pwm, 1500);
    _play(pwm, 3900);
}

void play_sound_4(const struct device *pwm)
{
    _play(pwm, 2000);
    _play(pwm, 3900);
}

void play_sound_5(const struct device *pwm)
{
    _play(pwm, 2500);
    _play(pwm, 3900);
}

int buzzer_listener(const zmk_event_t *eh)
{

    const struct zmk_ble_active_profile_changed *profile_ev = NULL;
    const struct device *pwm;

    if ((profile_ev = as_zmk_ble_active_profile_changed(eh)) == NULL) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    pwm = device_get_binding(BUZZ_LABEL);
    if (NULL == pwm) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    switch(profile_ev->index) {
        case 0:
            play_sound_1(pwm);
            break;
        case 1:
            play_sound_2(pwm);
            break;
        case 2:
            play_sound_3(pwm);
            break;
        case 3:
            play_sound_4(pwm);
            break;
        case 4:
            play_sound_5(pwm);
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
