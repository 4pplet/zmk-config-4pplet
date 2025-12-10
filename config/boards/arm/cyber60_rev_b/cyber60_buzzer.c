#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/event_manager.h>
#include <zmk/events/ble_active_profile_changed.h>

#define BUZZER_NODE DT_ALIAS(buzzer)

#if DT_NODE_HAS_STATUS(BUZZER_NODE, okay)

#define BEEP_DURATION K_MSEC(60)

static const struct pwm_dt_spec pwm = PWM_DT_SPEC_GET(BUZZER_NODE);
static bool pwm_ready = false;

static int buzzer_init(void) {
    if (!device_is_ready(pwm.dev)) {
        LOG_ERR("PWM device %s is not ready", pwm.dev->name);
        return -ENODEV;
    }
    pwm_ready = true;
    return 0;
}

static void play_tone(uint32_t period, k_timeout_t duration) {
    int ret;

    if (!pwm_ready) {
        return;
    }

    ret = pwm_set_dt(&pwm, period, period / 2U);
    if (ret < 0) {
        LOG_ERR("Failed to set PWM: %d", ret);
        return;
    }

    k_sleep(duration);

    ret = pwm_set_dt(&pwm, 0, 0);
    if (ret < 0) {
        LOG_ERR("Failed to stop PWM: %d", ret);
    }
}

static void play_sound_sequence(const uint32_t *tones, size_t count) {
    for (size_t i = 0; i < count; i++) {
        play_tone(tones[i], BEEP_DURATION);
        k_sleep(K_MSEC(50));
    }
}

void play_sound_1() {
    const uint32_t tones[] = {1000000, 500000, 250000, 100000, 50000};
    play_sound_sequence(tones, ARRAY_SIZE(tones));
}

void play_sound_2() {
    const uint32_t tones[] = {1500000, 3900000, 1500000, 1500000};
    play_sound_sequence(tones, ARRAY_SIZE(tones));
}

void play_sound_3() {
    const uint32_t tones[] = {1500000, 3900000};
    play_sound_sequence(tones, ARRAY_SIZE(tones));
}

void play_sound_4() {
    const uint32_t tones[] = {2000000, 3900000};
    play_sound_sequence(tones, ARRAY_SIZE(tones));
}

void play_sound_5() {
    const uint32_t tones[] = {2500000, 3900000};
    play_sound_sequence(tones, ARRAY_SIZE(tones));
}

int buzzer_listener(const zmk_event_t *eh) {
    const struct zmk_ble_active_profile_changed *profile_ev = as_zmk_ble_active_profile_changed(eh);
    if (!profile_ev) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    switch (profile_ev->index) {
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

SYS_INIT(buzzer_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif