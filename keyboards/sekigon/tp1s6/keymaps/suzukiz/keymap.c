/* Copyright 2021 sekigon-gonnoc
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H
#include "mtch6102.h"
#include "pointing_device.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    {{KC_BTN1, C(KC_TAB), KC_LSFT, KC_BTN2, MO(2), MO(1)}},
    {{KC_SPC, KC_ENT, KC_LCTL, KC_LALT, KC_LGUI, _______}},
    {{KC_LCTL, KC_LALT, KC_TAB, KC_LSFT, _______, KC_LGUI}},
};

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    switch (keycode) {
        case KC_BTN1 ... KC_BTN3:
            if (record->event.pressed) {
                pointing_device_set_button(1 << (keycode - KC_BTN1));
            } else {
                pointing_device_clear_button(1 << (keycode - KC_BTN1));
            }
            return false;
            break;
        default:
            break;
    }
    return true;
}

float tp_point[2]    = {0.0f, 0.0f};
float tp_velocity[2] = {0.0f, 0.0f};
struct VelHist {
    float history[10];
    int   count;
};
struct VelHist hist[2];

static void update_history(struct VelHist* h, float n) {
    int index         = h->count % 10;
    h->history[index] = n;
    h->count++;
}
static float get_average(struct VelHist* h) {
    int   cnt = h->count > 10 ? 10 : h->count;
    float avg = 0.0f;
    for (int i = 0; i < cnt; i++) {
        avg += h->history[i];
    }
    return avg / (float)cnt;
}

float tp_oldvelocity[2] = {0.0f, 0.0f};

int scroll_update(int spd, int ch, bool on_touch) {
    float pt = tp_point[ch];
    if (on_touch) {
        float nspd = fmin(1.0f, (float)spd / 120.0f);
        pt += nspd;
        tp_velocity[ch] = nspd;

        struct VelHist* vh = &hist[ch];
        update_history(vh, nspd);
        tp_oldvelocity[ch] = get_average(vh);
    } else {
        float nspd = tp_oldvelocity[ch];
        nspd *= 0.955f;
        pt += nspd;
        tp_oldvelocity[ch] = nspd;
    }

    int move = 0;
    if (pt > 0.1f) {
        pt -= 0.1f;
        move = 1;
    } else if (pt < -0.1f) {
        pt += 0.1f;
        move = -1;
    }
    tp_point[ch] = pt;
    return move;
}

void put_key(uint8_t key) {
    add_key(key);
    send_keyboard_report();
    wait_ms(150);
    del_key(key);
    send_keyboard_report();
}

void matrix_scan_user() {
    // Change cursor movement to scroll movement if layer is 1
    if (layer_state_is(1)) {
        report_mouse_t mouse_rep = pointing_device_get_report();

        int  xspd   = mouse_rep.x;
        int  yspd   = -mouse_rep.y;
        bool ontch  = pointing_device_get_on_touch();
        mouse_rep.h = scroll_update(xspd, 0, ontch);
        mouse_rep.v = scroll_update(yspd, 1, ontch);
        mouse_rep.x = 0;
        mouse_rep.y = 0;

        pointing_device_set_report(mouse_rep);
    }
    if (layer_state_is(2)) {
        report_mouse_t mouse_rep = pointing_device_get_report();
        uint8_t        key       = KC_NO;
        const int      limspd    = 20;
        if (mouse_rep.x > limspd) {
            key = KC_RIGHT;
        } else if (mouse_rep.x < -limspd) {
            key = KC_LEFT;
        } else if (mouse_rep.y > limspd) {
            key = KC_DOWN;
        } else if (mouse_rep.y < -limspd) {
            key = KC_UP;
        }
        if (key != KC_NO) {
            put_key(key);
        }
        mouse_rep.x = 0;
        mouse_rep.y = 0;
        pointing_device_set_report(mouse_rep);
    } else {
        report_mouse_t mouse_rep = pointing_device_get_report();

        float mx    = mouse_rep.x;
        float my    = mouse_rep.y;
        float xs    = fabs(mx);
        float ys    = fabs(my);
        xs          = fmax(1.0f, (xs / 18.0f));
        ys          = fmax(1.0f, (ys / 15.0f));
        mx          = fmin(127.0f, fmax(-127.0f, mx * xs * xs));
        my          = fmin(127.0f, fmax(-127.0f, my * ys * ys));
        mouse_rep.x = mx;
        mouse_rep.y = my;

        pointing_device_set_report(mouse_rep);
    }
}
