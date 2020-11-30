#include QMK_KEYBOARD_H

#include "paw3204.h"
#include "pointing_device.h"

bool isScrollMode;

enum layer_names {
    _BASE,
    _RAISE,
};

enum custom_keycodes {
    // clang-format off
    BASE = SAFE_RANGE,
    RAISE,
    MBTN1,
    MBTN2,
    MBTN3,
    SCRL
    // clang-format on
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // clang-format off
  [_BASE] = LAYOUT( \
                KC_SPC,   RAISE,
     C(KC_TAB), KC_LSFT,  KC_ENT,
     MBTN1,     MBTN2,    MBTN3
  ),
  [_RAISE] = LAYOUT( \
                KC_SPC,   KC_TRNS,
     KC_TAB,    KC_LALT,  KC_ESC,
     KC_LCTL,   KC_LGUI,  KC_LSFT
  )
    // clang-format on
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    report_mouse_t currentReport = {};
    switch (keycode) {
        case MBTN1:
            currentReport = pointing_device_get_report();
            if (record->event.pressed) {
                currentReport.buttons |= MOUSE_BTN1;
            } else {
                currentReport.buttons &= ~MOUSE_BTN1;
            }
            pointing_device_set_report(currentReport);
            pointing_device_send();
            return false;
        case MBTN2:
            currentReport = pointing_device_get_report();
            if (record->event.pressed) {
                currentReport.buttons |= MOUSE_BTN2;
            } else {
                currentReport.buttons &= ~MOUSE_BTN2;
            }
            pointing_device_set_report(currentReport);
            pointing_device_send();
            return false;
        case MBTN3:
            currentReport = pointing_device_get_report();
            if (record->event.pressed) {
                currentReport.buttons |= MOUSE_BTN3;
            } else {
                currentReport.buttons &= ~MOUSE_BTN3;
            }
            pointing_device_set_report(currentReport);
            pointing_device_send();
            return false;
        case SCRL:
            if (record->event.pressed) {
                isScrollMode = true;
            } else {
                isScrollMode = false;
            }
            return false;
        case RAISE:
            if (record->event.pressed) {
                layer_on(_RAISE);
            } else {
                layer_off(_RAISE);
            }
            return false;
    }
    return true;
}

void matrix_init_user(void) { init_paw3204(); }

report_mouse_t mouse_rep;

void keyboard_post_init_user() {
    debug_enable = true;
    debug_mouse  = true;
}

int moment[2];

int moment_spd(int spd, int ch) {
    const int th = 40;
    const int mx = 1000;

    int n = moment[ch];
    if (abs(spd) > 50) {
        spd *= 2;
    }
    n += spd;
    bool mv = true;
    if (n > th) {
        n -= th;
    } else if (n < -th) {
        n += th;
    } else {
        mv = false;
    }
    if (n > mx) {
        n = mx;
    } else if (n < -mx) {
        n = -mx;
    }
    moment[ch] = n;

    if (mv == false) {
        return 0;
    }
    return n > 0 ? 1 : n < 0 ? -1 : 0;
}
void moment_clr(void) {
    moment[0] = 0;
    moment[1] = 0;
}

void matrix_scan_user(void) {
    static int  cnt;
    static bool paw_ready;

    report_mouse_t mouse_rep = pointing_device_get_report();

    if (cnt++ % 50 == 0) {
        uint8_t pid = read_pid_paw3204();
        if (pid == 0x30) {
            dprint("paw3204 OK\n");
            paw_ready = true;
        } else {
            dprintf("paw3204 NG:%d\n", pid);
            paw_ready = false;
        }
    }

    if (paw_ready) {
        uint8_t stat;
        int8_t  x, y;

        read_paw3204(&stat, &x, &y);

        if (isScrollMode || IS_LAYER_ON(_RAISE)) {
            int yspd    = -y;
            int xspd    = -x;
            mouse_rep.h = moment_spd(yspd, 0);
            mouse_rep.v = moment_spd(xspd, 1);
        } else {
            int16_t yspd = (y * 5 + 1) / 6;
            int16_t xspd = (x * 5 + 1) / 6;
            mouse_rep.x  = -yspd;
            mouse_rep.y  = xspd;
            moment_clr();
        }

        if (cnt % 10 == 0) {
            dprintf("stat:%3d x:%4d y:%4d\n", stat, mouse_rep.x, mouse_rep.y);
        }

        if (stat & 0x80) {
            pointing_device_set_report(mouse_rep);
        } else if (IS_LAYER_ON(_RAISE)) {
            pointing_device_set_report(mouse_rep);
        }
    }
}
