#include QMK_KEYBOARD_H

#include "paw3204.h"
#include "pointing_device.h"

bool isScrollMode;

enum layer_names {
    _BASE,
    _RAISE,
    _LOWER,
};

enum custom_keycodes {
    // clang-format off
    BASE = SAFE_RANGE,
    RAISE,
    LOWER,
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
     C(KC_TAB), KC_LSFT,  LOWER,
     MBTN1,     MBTN2,    MBTN3
  ),
  [_RAISE] = LAYOUT( \
                KC_SPC,   KC_TRNS, 
     KC_TAB,    KC_LALT,  KC_ESC,
     KC_LCTL,   KC_LGUI,  KC_LSFT
  ),
  [_LOWER] = LAYOUT( \
                KC_SPC,   KC_ESC,
     KC_LSFT,   KC_TAB,   KC_TRNS,
     KC_LCTL,   KC_LGUI,  KC_LALT
  )
    // clang-format on
};

void report_button(bool pressed, int btn) {
    report_mouse_t currentReport = pointing_device_get_report();
    if (pressed) {
        currentReport.buttons |= btn;
    } else {
        currentReport.buttons &= ~btn;
    }
    pointing_device_set_report(currentReport);
    pointing_device_send();
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case MBTN1:
            report_button(record->event.pressed, MOUSE_BTN1);
            return false;
        case MBTN2:
            report_button(record->event.pressed, MOUSE_BTN2);
            return false;
        case MBTN3:
            report_button(record->event.pressed, MOUSE_BTN3);
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
        case LOWER:
            if (record->event.pressed) {
                layer_on(_LOWER);
            } else {
                layer_off(_LOWER);
            }
            return false;
    }
    return true;
}

void matrix_init_user(void) { init_paw3204(); }

void keyboard_post_init_user() {
    debug_enable = true;
    debug_mouse  = true;
}

static int moment[2];

int moment_spd(int spd, int ch) {
    const int th = 40;
    const int mx = 1000;

    int n = moment[ch];
    if (abs(spd) > 70) {
        spd *= abs(spd) > 100 ? 3 : 2;
    }
    if ((n < 0 && spd > 0) || (n > 0 && spd < 0)) {
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

void put_key(uint8_t key) {
    add_key(key);
    send_keyboard_report();
    wait_ms(150);
    del_key(key);
    send_keyboard_report();
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
        } else if (IS_LAYER_ON(_LOWER)) {
            static uint8_t key    = KC_NO;
            const int      limspd = 15;
            uint8_t        nkey   = KC_NO;
            if (y > limspd) {
                nkey = KC_LEFT;
            } else if (y < -limspd) {
                nkey = KC_RIGHT;
            } else if (x > limspd) {
                nkey = KC_DOWN;
            } else if (x < -limspd) {
                nkey = KC_UP;
            } else {
                nkey = KC_NO;
            }

            if (key != nkey) {
                if (nkey != KC_NO) {
                    put_key(nkey);
                }
                key = nkey;
            }
        } else {
            float xs = fabs(x);
            float ys = fabs(y);
            xs       = fmax(1.0f, (xs / 30.0f));
            ys       = fmax(1.0f, (ys / 25.0f));
            float mx = fmin(127.0f, fmax(-127.0f, x * xs));
            float my = fmin(127.0f, fmax(-127.0f, y * ys));

            const float lowspd = 1.0f;
            if (mx > lowspd)
                mx -= lowspd;
            else if (mx < -lowspd)
                mx += lowspd;
            else
                mx = 0.0f;
            if (my > lowspd)
                my -= lowspd;
            else if (my < -lowspd)
                my += lowspd;
            else
                my = 0.0f;
            mouse_rep.x = -my;
            mouse_rep.y = mx;
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
