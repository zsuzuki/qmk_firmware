#include QMK_KEYBOARD_H
#ifdef PROTOCOL_LUFA
#    include "lufa.h"
#    include "split_util.h"
#endif
#ifdef SSD1306OLED
#    include "ssd1306.h"
#endif
#include "keymap_jp.h"

extern uint8_t is_master;

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.

enum custom_keycodes { QWERTY = SAFE_RANGE, LOWER, RAISE };

enum macro_keycodes {
    KC_SAMPLEMACRO,
};

#define KC_ KC_TRNS
#define KC_RST RESET
#define KC_L_SPC LT(_LOWER, KC_SPC)     // lower
#define KC_L_KANA LT(_LOWER, JP_MKANA)  // lower
#define KC_R_ENT LT(_RAISE, KC_ENT)     // raise
#define KC_R_SPC LT(_RAISE, KC_SPC)     // raise
#define KC_G_JA LGUI_T(KC_LANG1)        // cmd or win
#define KC_G_EN LGUI_T(KC_LANG2)        // cmd or win
#define KC_C_BS LCTL_T(KC_BSPC)         // ctrl
#define KC_A_DEL ALT_T(KC_DEL)          // alt
#define KC_C_TAB CTL_T(KC_TAB)
#define KC_C_KANA CTL_T(JP_MKANA)
#define KC_G_KANA LGUI_T(JP_MKANA)
#define KC_L_TAB LT(_LOWER, KC_TAB)
#define MC_BSLH A(JP_YEN)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [_QWERTY] = LAYOUT( \
  //,--------+--------+---------+--------+---------+--------.   ,--------+---------+--------+---------+--------+--------.
     _______, KC_Q   , KC_W    , KC_E   , KC_R    , KC_T   ,     KC_Y   , KC_U    , KC_I   , KC_O    , KC_P   , _______,
  //|--------+--------+---------+--------+---------+--------|   |--------+---------+--------+---------+--------+--------|
     _______, KC_A   , KC_S    , KC_D   , KC_F    , KC_G   ,     KC_H   , KC_J    , KC_K   , KC_L    , JP_SCLN, _______,
  //|--------+--------+---------+--------+---------+--------|   |--------+---------+--------+---------+--------+--------|
     _______, KC_Z   , KC_X    , KC_C   , KC_V    , KC_B   ,     KC_N   , KC_M    , KC_COMM, KC_DOT  , KC_SLSH, _______,
  //`--------+--------+---------+--------+---------+--------/   \--------+---------+--------+---------+--------+--------'
                       KC_LSFT , KC_C_BS, LOWER   , KC_G_KANA,   KC_RALT, KC_R_SPC, KC_RSFT, KC_C_BS
  //                 `----------+--------+---------+--------'   `--------+---------+--------+---------'
  ),

  //   \ ^ ! & |  @ = + * % -
  // ( # $ " ' ~  ← ↓ ↑ → ` )
  //         { [  ] }

  [_RAISE] = LAYOUT( \
  //,--------+--------+--------+--------+--------+--------.   ,--------+--------+--------+--------+--------+--------.
//   _______, KC_BSLS, KC_CIRC, KC_EXLM, KC_AMPR, KC_PIPE,     KC_AT  , KC_EQL , KC_PLUS, KC_ASTR, KC_PERC, KC_MINS,
     _______, KC_1   , KC_2   , KC_3   , KC_4   , KC_5   ,     KC_6   , KC_7   , KC_8   , KC_9   , KC_0   , _______,
  //|--------+--------+--------+--------+--------+--------|   |--------+--------+--------+--------+--------+--------|
//   KC_LPRN, KC_HASH, KC_DLR , KC_DQT , KC_QUOT, KC_TILD,     KC_LEFT, KC_DOWN, KC_UP  , KC_RGHT, KC_GRV , KC_RPRN,
     _______, _______, _______, KC_HOME, KC_PGUP, KC_TAB ,     JP_COLN, KC_ENT , KC_UP  , JP_AT  , JP_TILD, KC_F12 ,
  //|--------+--------+--------+--------+--------+--------|   |--------+--------+--------+--------+--------+--------|
     KC_F5  , _______, _______, KC_END , KC_PGDN,C(KC_TAB),    JP_PIPE, KC_LEFT, KC_DOWN, KC_RGHT, JP_BSLS, KC_F11 ,
  //`--------+--------+--------+--------+--------+--------/   \--------+--------+--------+--------+--------+--------'
                       _______, _______, _______, _______,     _______, _______, _______, _______
  //                  `--------+--------+--------+--------'   `--------+--------+--------+--------'
  ),

  [_LOWER] = LAYOUT( \
  //,--------+--------+--------+--------+--------+--------.   ,--------+--------+--------+--------+--------+--------.
     _______, JP_DQUO, JP_HASH, JP_DLR , JP_PERC, JP_AMPR,     JP_QUOT, JP_LPRN, JP_RPRN, JP_CIRC, JP_EQL , _______,
  //|--------+--------+--------+--------+--------+--------|   |--------+--------+--------+--------+--------+--------|
     _______, JP_EXLM, C(KC_S), KC_MS_U, KC_DEL , KC_ESC ,     JP_ASTR, JP_MINS, JP_LBRC, JP_RBRC, JP_PLUS, _______,
  //|--------+--------+--------+--------+--------+--------|   |--------+--------+--------+--------+--------+--------|
     _______, KC_BTN2, KC_MS_L, KC_MS_D, KC_MS_R, KC_BTN1,     JP_YEN , MC_BSLH, JP_LABK, JP_RABK, JP_QUES, _______,
  //`--------+--------+--------+--------+--------+--------/   \--------+--------+--------+--------+--------+--------'
                       RESET  , _______, _______, _______,     _______, _______, _______, _______
  //                 `--------+--------+--------+--------'   `--------+--------+--------+--------'
  ),
};
// clang-format on

void matrix_init_user(void) {
// SSD1306 OLED init, make sure to add #define SSD1306OLED in config.h
#ifdef SSD1306OLED
    iota_gfx_init(!has_usb());  // turns on the display
#endif
}

// SSD1306 OLED update loop, make sure to add #define SSD1306OLED in config.h
#ifdef SSD1306OLED

// When add source files to SRC in rules.mk, you can use functions.
const char *read_layer_state(void);
const char *read_logo(void);
void        set_keylog(uint16_t keycode, keyrecord_t *record);
const char *read_keylog(void);
const char *read_keylogs(void);

const char *read_mode_icon(bool swap);
void        set_timelog(void);
const char *read_timelog(void);
// const char *read_host_led_state(void);

void        game_update(void);
void        game_keyupdate(uint16_t keycode);
const char *read_game(void);

void matrix_scan_user(void) { iota_gfx_task(); }

void matrix_render_user(struct CharacterMatrix *matrix) {
    if (is_master) {
        // If you want to change the display of OLED, you need to change here
        matrix_write_ln(matrix, read_layer_state());
        matrix_write_ln(matrix, read_keylog());
        game_update();
        matrix_write_ln(matrix, read_game());
        // matrix_write_ln(matrix, read_keylogs());
        // matrix_write_ln(matrix, read_mode_icon(keymap_config.swap_lalt_lgui));
        // matrix_write_ln(matrix, read_host_led_state());
        // matrix_write_ln(matrix, read_timelog());
    } else {
        matrix_write(matrix, read_logo());
        // matrix_write_ln(matrix, read_mode_icon(keymap_config.swap_lalt_lgui));
        // matrix_write_ln(matrix, read_timelog());
    }
}

void matrix_update(struct CharacterMatrix *dest, const struct CharacterMatrix *source) {
    if (memcmp(dest->display, source->display, sizeof(dest->display))) {
        memcpy(dest->display, source->display, sizeof(dest->display));
        dest->dirty = true;
    }
}

void iota_gfx_task_user(void) {
    struct CharacterMatrix matrix;
    matrix_clear(&matrix);
    matrix_render_user(&matrix);
    matrix_update(&display, &matrix);
}
#endif  // SSD1306OLED

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
#ifdef SSD1306OLED
        set_keylog(keycode, record);
#endif
        // set_timelog();
        game_keyupdate(keycode);
    }

    switch (keycode) {
        case QWERTY:
            if (record->event.pressed) {
                set_single_persistent_default_layer(_QWERTY);
            }
            return false;
            break;
        case LOWER:
            if (record->event.pressed) {
                layer_on(_LOWER);
            } else {
                layer_off(_LOWER);
            }
            return false;
            break;
        case RAISE:
            if (record->event.pressed) {
                layer_on(_RAISE);
            } else {
                layer_off(_RAISE);
            }
            return false;
            break;
    }
    return true;
}
