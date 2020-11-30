#include QMK_KEYBOARD_H

#include "paw3204.h"
#include "pointing_device.h"

bool isScrollMode;

enum layer_names {
    _BASE,
};

enum custom_keycodes {
    BASE = SAFE_RANGE,
    MBTN1,
    MBTN2,
    MBTN3,
    SCRL
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_BASE] = LAYOUT( \
             KC_A,    SCRL,
     KC_ENT, KC_SPC,  KC_B,       
     MBTN1,  MBTN2,   MBTN3 
  )
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  report_mouse_t currentReport = {};
  switch (keycode) {
    case MBTN1:
      currentReport = pointing_device_get_report();
      if (record->event.pressed) {
        currentReport.buttons |= MOUSE_BTN1;
      }
      else {
        currentReport.buttons &= ~MOUSE_BTN1;
      }
      pointing_device_set_report(currentReport);
      pointing_device_send();
      return false;
    case MBTN2:
      currentReport = pointing_device_get_report();
      if (record->event.pressed) {
        currentReport.buttons |= MOUSE_BTN2;
      }
      else {
        currentReport.buttons &= ~MOUSE_BTN2;
      }
      pointing_device_set_report(currentReport);
      pointing_device_send();
      return false;
    case MBTN3:
      currentReport = pointing_device_get_report();
      if (record->event.pressed) {
        currentReport.buttons |= MOUSE_BTN3;
      }
      else {
        currentReport.buttons &= ~MOUSE_BTN3;
      }
      pointing_device_set_report(currentReport);
      pointing_device_send();
      return false;
    case SCRL:
      if (record->event.pressed) {
        isScrollMode = true;
      }
      else {
        isScrollMode = false;
      }
      return false;
  }
  return true;
}

void matrix_init_user(void) {
    init_paw3204();
}

report_mouse_t mouse_rep;

void keyboard_post_init_user() {
    debug_enable = true;
    debug_mouse = true;
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
        int8_t x, y;

        read_paw3204(&stat, &x, &y);

        if (isScrollMode) {
            mouse_rep.h = -y/10;
            mouse_rep.v = -x/10;
        } else {
            mouse_rep.x = -y;
            mouse_rep.y = x;
        }

        if (cnt % 10 == 0) {
            dprintf("stat:%3d x:%4d y:%4d\n", stat, mouse_rep.x, mouse_rep.y);
        }

        if (stat & 0x80) {
            pointing_device_set_report(mouse_rep);
        }
    }
}

