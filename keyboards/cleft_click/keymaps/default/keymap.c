#include "cleft_click.h"

#define ______ KC_TRNS
#define XXXXXX KC_NO
#define _QWERTY 0
#define _LOWER 1

enum custom_keycodes {
  LOWER = SAFE_RANGE,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[_QWERTY] = KEYMAP( \
  KC_ESC,  KC_1,    KC_2,    KC_3,  KC_4,   KC_5,  KC_6,   KC_7,   KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, \
  KC_TAB,  KC_Q,    KC_W,    KC_E,  KC_R,   KC_T,          KC_Y,   KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, \
  KC_CAPS, KC_A,    KC_S,    KC_D,  KC_F,   KC_G,          KC_H,   KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT, \
  KC_LSFT, KC_Z,    KC_X,    KC_C,  KC_V,   KC_B,          KC_N,   KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, \
  KC_LCTL, KC_LGUI, KC_LALT, LOWER, KC_SPC,                KC_SPC, KC_RALT, KC_RGUI, LOWER,   KC_RCTL \
),

[_LOWER] = KEYMAP( \
  KC_GRV, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,  KC_F6,   KC_F7,   KC_F8,   KC_F9,     KC_F10,  KC_F11,   KC_F12,  KC_DEL, \
  ______, KC_MUTE, ______,  ______,  ______,  ______,          ______,  KC_INS,  KC_PGUP,   KC_PSCR, KC_SLCK,  KC_PAUS, ______, ______, \
  ______, KC_VOLD, KC_VOLU, KC_MPRV, KC_MNXT, KC_MPLY,         KC_HOME, KC_LEFT, KC_DOWN,   KC_UP,   KC_RIGHT, KC_END,  ______, \
  ______, ______,  ______,  ______,  ______,  ______,          ______,  ______,  KC_PGDOWN, ______,  ______,   ______, \
  ______, ______,  ______,  ______,  ______,                   ______,  ______,  ______,    ______,  ______\
)
/*[_QWERTY] = KEYMAP( \
  KC_7,   KC_8,    KC_9,    KC_0,    KC_MINS, KC_EQL,  KC_BSPC, \
  KC_Y,   KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, \
  KC_H,   KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT, \
  KC_N,   KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT, \
  KC_SPC, KC_RALT, KC_RGUI, LOWER,   KC_RCTL \
),
[_LOWER] = KEYMAP( \
  KC_F7,   KC_F8,   KC_F9,     KC_F10,  KC_F11,   KC_F12,  KC_DEL, \
  ______,  KC_INS,  KC_PGUP,   KC_PSCR, KC_SLCK,  KC_PAUS, ______, ______, \
  KC_HOME, KC_LEFT, KC_DOWN,   KC_UP,   KC_RIGHT, KC_END,  ______, \
  ______,  ______,  KC_PGDOWN, ______,  ______,   ______, \
  ______,  ______,  ______,    ______,  ______\
)*/
};

const uint16_t PROGMEM fn_actions[] = {

};

const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt) {
  return MACRO_NONE;
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case LOWER:
      if (record->event.pressed) {
        layer_on(_LOWER);
      }
			else {
        layer_off(_LOWER);
      }
      return false;
      break;
  }
  return true;
}
