#include QMK_KEYBOARD_H

/***************************
 * Trackball related defines
 **************************/

#include "pointing_device.h"
#include "../../pmw3360/pmw3360.h"

uint8_t track_mode = 0; // 0 Mousecursor; 1 arrowkeys/carret; 2 scrollwheel; 3 sound/brightness
#define cursor_mode 0
#define carret_mode 1
#define scroll_mode 2
#define sound_brightness_mode 3
uint8_t prev_track_mode = 0;
bool integration_mode = false;
int16_t cum_x = 0;
int16_t cum_y = 0;
int16_t sensor_x = 0;
int16_t sensor_y = 0;
uint16_t mouse_moved_timer;

// Thresholds help to move only horizontal or vertical. When accumulated distance reaches threshold, only move one discrete value in direction with bigger delta.
uint8_t	carret_threshold = 24;		 // higher means slower
uint16_t carret_threshold_inte = 340; // in integration mode higher threshold

#define regular_smoothscroll_factor 8
bool smooth_scroll = true;
uint8_t	scroll_threshold = 8 / regular_smoothscroll_factor;	// divide if started smooth
uint16_t scroll_threshold_inte = 1200 / regular_smoothscroll_factor;

uint16_t cursor_multiplier = 60;	// adjust cursor speed
uint16_t cursor_multiplier_inte = 20;
#define CPI_STEP 20

int16_t cur_factor;



/***************************
 * Keycode and layer defines
 **************************/
#define _BASE 0
#define _MOVE 1
#define _MOVE_SHIFT 2
#define _SELECT 3
#define _SELECT_MOVE 4
#define _SELECT_MOVE_SHIFT 5
#define _NUM 6
#define _SYM 7
#define _MOUSE 8

//#define _FN4 6
enum {
	TD_CPY_PST = 0,
	TD_CMD,
	TD_SLSH,
 	TD_VRDSK
};
enum custom_keycodes {
	MOVE = SAFE_RANGE,
	MOVE_SHIFT,
	SELECT,
	SELECT_MOVE,
	SELECT_MOVE_SHIFT,
	NUM,
	SYM,
	MOUSE,
	dance_cln_finished,
	KC_GUITAB,
	KC_MOVE,
	KC_MSHIFT,
	KC_SEL_L,
    KC_SEL_R,
    KC_SEL_U,
    KC_SEL_D,
    KC_SEL_L_WRD,
    KC_SEL_R_WRD,
    KC_SEL_PG_UP,
    KC_SEL_PG_D,
    KC_SEL_HOME,
    KC_SEL_END,
    KC_L_WRD,
    KC_R_WRD,
    KC_B_WRD,
    KC_S_COPY,
    KC_S_CUT,
    KC_SEL_A,
    KC_JLCLK,
    KC_KRCLK
};

#define KC_ KC_TRNS
#define _______ KC_TRNS
#define KC_CAPW LGUI(X_LSHIFT(KC_3))        // Capture whole screen
#define KC_CPYW LGUI(X_LSHIFT(LCTL(KC_3)))  // Copy whole screen
#define KC_CAPP LGUI(X_LSHIFT(KC_4))       // Capture portion of screen
#define KC_CPYP LGUI(X_LSHIFT(LCTL(KC_4)))  // Copy portion of screen
//#define KC_GUITAB RWIN_T(KC_TAB)

#define KC_SEL_A M(23) // OSL(_SELECT) // turns on the select oneshot, turns off when any key is pressed
#define KC_TABNUM LT(_NUM, KC_TAB)
#define KC_SYM MO(_SYM)
#define KC_CTRLENT MT(MOD_LCTL, KC_ENT)
#define KC_MOUSESC LT(_MOUSE, KC_ESC)

#define KC_EXLM LSFT(KC_1)
#define KC_AT LSFT(KC_2)
#define KC_HASH LSFT(KC_3)
#define KC_DLR LSFT(KC_4)
#define KC_PERC LSFT(KC_5)
#define KC_CAR LSFT(KC_6)
#define KC_AMPR LSFT(KC_7)
#define KC_ASTR LSFT(KC_8)
#define KC_LPRN LSFT(KC_9)
#define KC_RPRN LSFT(KC_0)

#define KC_ALTAB OS_CTL(KC_TAB)
#define KC_AALTAB OS_CTL(KC_TILDE)

#define KC_NAVTEST OS_CTL(LSFT(KC_T))
#define KC_SAVE OS_CTL(KC_S)
#define KC_COPY OS_CTL(KC_C)
#define KC_CUT OS_CTL(KC_X)
#define KC_PASTE OS_CTL(KC_V)
#define KC_TERMPASTE OS_CTL(LSFT(KC_V))
#define KC_UNDO OS_CTL(KC_Z)
#define KC_ALTENT LALT(KC_ENT)
#define KC_COMMENT OS_CTL(KC_SLSH)
#define KC_REDO OS_CTL(LSFT(KC_Z))
#define KC_FORMAT LALT(LSFT(KC_L))
#define KC_GOTO OS_CTL(KC_B)
#define KC_DIR LALT(LSFT(KC_D))
#define KC_OPEN LALT(LSFT(KC_O))
#define KC_FINDA LALT(LSFT(KC_F))
#define KC_OOG LALT(LSFT(KC_4)) // Open in oposite group intellij
//#define KC_RUN LALT(LCTL(KC_R)) // Run on cursor intellij
//#define KC_DEBUG LALT(LCTL(KC_D)) // Debug on cursor intellij
//#define KC_SRUN LSFT(LALT(LCTL(KC_R))) // Run on cursor intellij
//#define KC_SDEBUG LSFT(LALT(LCTL(KC_D))) // Debug on cursor intellij
#define KC_RUN LALT(LSFT(KC_1)) // Run on cursor intellij
#define KC_DEBUG LALT(LSFT(KC_2)) // Debug on cursor intellij
#define KC_SRUN LALT(LSFT(KC_3)) // Run on cursor intellij
#define KC_SDEBUG LALT(LSFT(KC_5)) // Debug on cursor intellij

#define KC_MVRT LALT(KC_4) // Move window right (do a split) in intellij
#define KC_MVGRP LALT(LSFT(KC_4)) // Alternate the group of the window in intellij
#define KC_CLOSE OS_CTL(KC_W)
#define KC_REP OS_CTL(KC_R)
#define KC_REPA LALT(LSFT(KC_R))
#define KC_OPG LALT(KC_S) // Move to the opposite group intellij
#define KC_FIND OS_CTL(KC_F)
#define KC_P_TAB LCTL(KC_PGUP)
#define KC_N_TAB LCTL(KC_PGDN)
#define KC_PREV LALT(KC_LBRC)
#define KC_NEXT LALT(KC_RBRC)
#define KC_REF LCTL(KC_R)

#define KC_SELALL OS_CTL(KC_A)


#define XOS_CTL X_LCTL
#define OS_CTL LCTL
//#define OS_CTL LCMD
//#define XOS_CTL X_LCMD

//#define MOVE X_LALT // mac
//#define LMOVE LALT // mac
#define MOVE X_LCTL // linux
#define LMOVE LCTL // linux


// Fillers to make layering more clear

#define ____ KC_TRNS

/***************************
 * Mouse pressed
 **************************/

void on_mouse_button(uint8_t mouse_button, bool pressed) {
	report_mouse_t report = pointing_device_get_report();

	if(pressed)
		report.buttons |= mouse_button;
	else
		report.buttons &= ~mouse_button;
	pointing_device_set_report(report);
	pointing_device_send();
}

bool volatile shift_down=false;
bool volatile space_shift_down=false;
bool volatile gui_down=false;
bool volatile alt_down=false;
bool volatile select_active=false;
bool volatile pressed_something_else=false;
bool volatile pressed_something_else_gui=false;
bool volatile mouse_pressed=false;
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    bool p = record->event.pressed;
  	switch (keycode) {
      	case KC_SEL_L: // select left
      	    if (p) {
      	      SEND_STRING(SS_DOWN(X_LSHIFT) SS_DOWN(X_LEFT) SS_UP(X_LSHIFT));
      	    }
      	    else {
      	      SEND_STRING(SS_UP(X_LEFT) SS_UP(X_LSHIFT));
      	    }
  //    	    set_oneshot_layer(_SELECT, ONESHOT_START)
      	    return false;

      	case KC_SEL_R: // select right
      	    if (p) {
                SEND_STRING(SS_DOWN(X_LSHIFT) SS_DOWN(X_RIGHT));
      	    }
      	    else {
      	      SEND_STRING(SS_UP(X_RIGHT) SS_UP(X_LSHIFT));
      	    }
  //    	    set_oneshot_layer(_SELECT, ONESHOT_START)
              return false;

          case KC_SEL_U: // select up
              if(p) {
                SEND_STRING(SS_DOWN(X_LSHIFT) SS_DOWN(X_UP));
              }
              else {
                SEND_STRING(SS_UP(X_UP) SS_UP(X_LSHIFT));
              }
  //            set_oneshot_layer(_SELECT, ONESHOT_START)
              return false;

          case KC_SEL_D: // select down
              if (p) {
                SEND_STRING(SS_DOWN(X_LSHIFT) SS_DOWN(X_DOWN));
              }
              else {
                SEND_STRING(SS_UP(X_DOWN) SS_UP(X_LSHIFT));
              }
  //            set_oneshot_layer(_SELECT, ONESHOT_START)
              return false;

          case KC_SEL_L_WRD: // select left word
              if (p) {
                SEND_STRING(SS_DOWN(MOVE) SS_DOWN(X_LSHIFT) SS_DOWN(X_LEFT));
              }
              else {
                SEND_STRING(SS_UP(X_LEFT) SS_UP(X_LSHIFT) SS_UP(MOVE));
              }
  //            set_oneshot_layer(_SELECT, ONESHOT_START)
              return false;

          case KC_SEL_R_WRD: // select right word
              if(p) {
                SEND_STRING(SS_DOWN(MOVE) SS_DOWN(X_LSHIFT) SS_DOWN(X_RIGHT));
              }
              else {
                SEND_STRING(SS_UP(X_RIGHT) SS_UP(X_LSHIFT) SS_UP(MOVE));
              }
  //            set_oneshot_layer(_SELECT, ONESHOT_START)
              return false;

          case KC_SEL_PG_UP: // select page up
              if (p) {
                SEND_STRING(SS_DOWN(X_LSHIFT) SS_DOWN(X_PGUP));
              }
              else {
                SEND_STRING(SS_UP(X_PGUP) SS_UP(X_LSHIFT));
              }
  //            set_oneshot_layer(_SELECT, ONESHOT_START)
              return false;

          case KC_SEL_PG_D: // select page down
              if (p) {
                SEND_STRING(SS_DOWN(X_LSHIFT) SS_DOWN(X_PGDOWN));
              }
              else {
                SEND_STRING(SS_UP(X_PGDOWN) SS_UP(X_LSHIFT));
              }
  //            set_oneshot_layer(_SELECT, ONESHOT_START)
              return false;

          case KC_SEL_HOME: // select home
              if(p) {
                SEND_STRING(SS_DOWN(X_LSHIFT) SS_DOWN(X_HOME));
              }
              else {
                SEND_STRING(SS_UP(X_HOME) SS_UP(X_LSHIFT));
              }
  //            set_oneshot_layer(_SELECT, ONESHOT_START)
              return false;

          case KC_SEL_END: // select end
              if(p) {
                SEND_STRING(SS_DOWN(X_LSHIFT) SS_DOWN(X_END));
              }
              else {
                SEND_STRING(SS_UP(X_END) SS_UP(X_LSHIFT));
              }
  //            set_oneshot_layer(_SELECT, ONESHOT_START)
              return false;

      	case KC_L_WRD: // left word
      	    if (p) {
      	      SEND_STRING(SS_DOWN(MOVE) SS_DOWN(X_LEFT));
      	    }
      	    else {
        	      SEND_STRING(SS_UP(X_LEFT) SS_UP(MOVE));
      	    }
      		return false;

      	case KC_SEL_A: // Activate select mode
                   if(p) {
                     select_active = !select_active;
                     layer_on(_SELECT_MOVE);
                   }
                   return false;

      	case KC_R_WRD: // right word
      	    if(p) {
                SEND_STRING(SS_DOWN(MOVE) SS_DOWN(X_RIGHT));
      	    }
      	    else {
                SEND_STRING(SS_UP(X_RIGHT) SS_UP(MOVE));
      	    }
              return false;

          case KC_B_WRD: // backspace left word
              if (p) {
                SEND_STRING(SS_DOWN(MOVE) SS_DOWN(X_BSPACE));
              }
              else {
                SEND_STRING(SS_UP(X_BSPACE) SS_UP(MOVE));
              }
              return false;

          case KC_S_COPY: //scopy
              if (p) {

                SEND_STRING(SS_DOWN(XOS_CTL) SS_DOWN(X_C));
              }
              else {
                SEND_STRING(SS_UP(X_C) SS_UP(XOS_CTL) SS_TAP(X_ESCAPE));
                select_active=false;
                layer_off(_SELECT_MOVE);
                layer_on(_MOVE);
              }
              return false;

          case KC_S_CUT: //scut
              if (p) {
                SEND_STRING(SS_DOWN(XOS_CTL) SS_DOWN(X_X));
              }
              else {
                SEND_STRING(SS_UP(X_X) SS_UP(XOS_CTL) SS_TAP(X_ESCAPE));
                select_active=false;
                layer_off(_SELECT_MOVE);
                layer_on(_MOVE);
              }
              return MACRO_NONE;


           case KC_MOVE: // malt
              if(p) {
                alt_down=true;
                if (select_active) {
                  if (shift_down) {
                    SEND_STRING(SS_UP(X_LSHIFT));
                    layer_on(_SELECT_MOVE_SHIFT);
                  }
                  else {
                    layer_on(_SELECT_MOVE);
                  }
                }
                else {
                  if (shift_down) {
                    SEND_STRING(SS_UP(X_LSHIFT));
                    layer_on(_MOVE_SHIFT);
                  }
                  else {
                    layer_on(_MOVE);
                  }
                }
              }
              else {
                alt_down=false;
                select_active = false;
                layer_off(_MOVE);
                layer_off(_MOVE_SHIFT);
                layer_off(_SELECT_MOVE_SHIFT);
                layer_off(_SELECT_MOVE);
              }
              return false;

           case KC_MSHIFT: // mshift
              if(p) {
                shift_down=true;
                if(alt_down) {
                  if(select_active) {
                    layer_on(_SELECT_MOVE_SHIFT);
                  }
                  else {
                    layer_on(_MOVE_SHIFT);
                  }
                }
                else {
                  SEND_STRING(SS_DOWN(X_LSHIFT));
                }
              }
              else {
                shift_down=false;
                layer_off(_SELECT_MOVE_SHIFT);
                layer_off(_MOVE_SHIFT);
                layer_off(_MOVE);
                if (alt_down) {
                  if(select_active) {
                    layer_on(_SELECT_MOVE);
                  }
                  else {
                    layer_on(_MOVE);
                  }
                }
                SEND_STRING(SS_UP(X_LSHIFT));
              }
              return false;

      case KC_JLCLK:
        if (p) {
          if (timer_elapsed(mouse_moved_timer) < 400) {
      		on_mouse_button(MOUSE_BTN1, p);
            mouse_pressed = true;
          } else {
            SEND_STRING(SS_DOWN(X_J));
          }
        } else {
          if (mouse_pressed) {
      		on_mouse_button(MOUSE_BTN1, p);
            mouse_pressed = false;
          } else {
            SEND_STRING(SS_UP(X_J));
          }
        }
        return false;
      case KC_KRCLK:
        if (p) {
          if (timer_elapsed(mouse_moved_timer) < 400) {
            mouse_pressed = true;
       		on_mouse_button(MOUSE_BTN2, p);
          } else {
            SEND_STRING(SS_DOWN(X_K));
          }
        } else {
          if (mouse_pressed) {
            on_mouse_button(MOUSE_BTN2, p);
            mouse_pressed = false;
          } else {
            SEND_STRING(SS_UP(X_K));
          }
        }
        return false;

  	}

   return true;
}



const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* Base (qwerty)
 * ,----------------------------------,                             ,----------------------------------,
 * |   q  |   w  |   e  |   r  |   t  |                             |   y  |   u  |   i  |   o  |   p  |
 * |------+------+------+------+------|                             |-------------+------+------+------|
 * |   a  |   s  |   d  |   f  |   g  |                             |   h  |   j  |   k  |   l  |   ;  |
 * |------+------+------+------+------|                             |------|------+------+------+------|
 * |   z  |   x  |   c  |   v  |   b  |                             |   n  |   m  |   ,  |   .  |   /  |
 * |------+------+------+-------------,                             ,-------------+------+------+------,
 *        |  CAPS| ALT  |                                                         |   \  |   =  |
 *        '------+------'-------------'                             '-------------'------+------'
 *                             |  TAB |                             | ENT  |
 *                             |   +  |                             |  +   |
 *                             | NUM |                             | CTRL KCCTRLENT |
 *                             +------'                             '------u
 *                                    '------+------' '------+------'
 *                                    | MOVE | SHFT | | BSP  |SPACE |
 *                                    '------+------' '------+------'
 *                                    | GUI  | SYM  | |      |      |
 *                                    '------+------' '------+------'
 */
[_BASE] = LAYOUT( \
  KC_Q, KC_W, KC_E,    KC_R,    KC_T,                                       KC_Y, KC_U,    KC_I,    KC_O,   KC_P,    \
  KC_A, KC_S, KC_D,    KC_F,    KC_G,                                       KC_H, KC_JLCLK,    KC_KRCLK,    KC_L,   KC_SCLN, \
  KC_Z, KC_X, KC_C,    KC_V,    KC_B,                                       KC_N, KC_M,    KC_COMM, KC_DOT, KC_SLSH, \
       KC_CAPS, KC_LALT,                                                                    KC_BSLS, KC_EQL,          \
                                  KC_TABNUM,                        RESET,                                       \
                                      KC_MOVE, KC_SYM,   KC_BSPC,  KC_SPACE,                                           \
                                      KC_LSFT, KC_LGUI,    KC_MOUSESC,  ____
),

/* MOVE
 * ,----------------------------------,                             ,----------------------------------,
 * |ALTAB | CLOSE|B_WRD |  REF |      |                             |      | L_WRD|  UP  | R_WRD|      |
 * |------+------+------+------+------|                             |-------------+------+------+------|
 * |SELALL| SAVE |mshift| FIND |      |                             |   END| LEFT | DOWN | RIGHT|      |
 * |------+------+------+------+------|                             |------|------+------+------+------|
 * | UNDO | CUT  | COPY |PASTE |      |                             |      |      | P_TAB|N_TAB |      |
 * |------+------+------+-------------,                             ,-------------+------+------+------,
 *        |      |MSHIFT|                                                         | PREV | NEXT |
 *        '------+------'-------------'                             '-------------'------+------'
 *                             |      |                             |      |
 *                             |      |                             |ALTENT|
 *                             |      |                             |      |
 *                             +------'                             '------+
 *                                    '------+------' '------+------'
 *                                    |      |MSHIFT| | RUN  | SEL_A|
 *                                    '------+------' '------+------'
 *                                    |      |      | | DEBUG|      |
 *                                    '------+------' '------+------'
 */

[_MOVE] = LAYOUT( \
  KC_ALTAB, KC_CLOSE, KC_B_WRD,KC_REP,____,                                     ____, KC_L_WRD, KC_UP, KC_R_WRD, KC_MVRT,         \
  KC_SELALL, KC_SAVE, KC_MSHIFT,KC_FIND,    ____,                                       KC_END, KC_LEFT,  KC_DOWN, KC_RIGHT, KC_OPG,         \
  KC_UNDO, KC_CUT, KC_COPY, KC_PASTE,KC_GOTO,                                       ____,____, KC_P_TAB,KC_N_TAB,  KC_COMMENT,         \
        ____, KC_MSHIFT,                                                                  KC_PREV, KC_NEXT,               \
                                  ____,                        KC_ALTENT,                                                 \
                                      ____,____,     KC_RUN,  KC_SEL_A,                                                   \
                                      KC_MSHIFT, ____,     KC_DEBUG,  ____
),
 /* MOVE_SHIFT
  * ,----------------------------------,                             ,----------------------------------,
  * |AALTAB| CLOSE|B_WRD |REPA | NAVTEST |                             |      | L_WRD| PGUP | OPEN |      |
  * |------+------+------+------+------|                             |-------------+------+------+------|
  * |SELALL| SAVE |  DIR   | FINDA |      |                             |  HOME|      | PGDN | KC_FORMAT|      |
  * |------+------+------+------+------|                             |------|------+------+------+------|
  * | REDO |      |     |      |      |                             |      |      |      |      |      |
  * |------+------+------+-------------,                             ,-------------+------+------+------,
  *        |      |MSHIFT|                                                         |      |      |
  *        '------+------'-------------'                             '-------------'------+------'
  *                             |      |                             |      |
  *                             |      |                             |ALTENT|
  *                             |      |                             |      |
  *                             +------'                             '------+
  *                                    '------+------' '------+------'
  *                                    |      |MSHIFT| | RUN  |DEBUG |
  *                                    '------+------' '------+------'
  *                                    |      |      | |      |      |
  *                                    '------+------' '------+------'
  */
[_MOVE_SHIFT] = LAYOUT( \
  KC_AALTAB, KC_CLOSE, KC_B_WRD,KC_REPA,    KC_NAVTEST,                              ____, KC_L_WRD, KC_PGUP, KC_OPEN, KC_MVGRP,         \
  KC_SELALL, KC_SAVE, KC_DIR, KC_FINDA,    ____,                           KC_HOME, ____,    KC_PGDN, KC_FORMAT, ____,         \
  KC_REDO, ____,____ ,    KC_TERMPASTE,    ____,                                       ____, ____,    ____, ____, ____,         \
        ____, ____,                                                                        ____, ____,               \
                                  ____,                        KC_ALTENT,                                                 \
                                      ____, ____,     KC_SRUN,  ____,                                                   \
                                      KC_MSHIFT, ____,     KC_SDEBUG,  ____
),

   /* SELECT
    * ,----------------------------------,                             ,----------------------------------,
    * |      |      |      |      |      |                             |      |      |      |      |      |
    * |------+------+------+------+------|                             |-------------+------+------+------|
    * |      |      |      |      |      |                             |      |      |      |      |      |
    * |------+------+------+------+------|                             |------|------+------+------+------|
    * |      |      |      |      |      |                             |      |      |      |      |      |
    * |------+------+------+-------------,                             ,-------------+------+------+------,
    *        |      |MSHIFT|                                                         |      |      |
    *        '------+------'-------------'                             '-------------'------+------'
    *                             |      |                             |      |
    *                             |      |                             |      |
    *                             |      |                             |      |
    *                             +------'                             '------+
    *                                    '------+------' '------+------'
    *                                    |  MOVE|MSHIFT| |      |     |
    *                                    '------+------' '------+------'
    *                                    |      |      | |      |      |
    *                                    '------+------' '------+------'
    */
[_SELECT] = LAYOUT( \
  ____, ____, ____,    ____,    ____,                                       KC_A, ____,    ____, ____, ____,         \
  ____, ____, ____,    ____,    ____,                                       ____, ____,    ____, ____, ____,         \
  ____, ____, ____,    ____,    ____,                                       ____, ____,    ____, ____, ____,         \
        ____, KC_MSHIFT,                                                                        ____, ____,               \
                                  ____,                        ____,                                                 \
                                      KC_MOVE, ____,     ____,  ____,                                                   \
                                      KC_MSHIFT, ____,     ____,  ____
),

   /* SELECT_MOVE
    * ,----------------------------------,                             ,----------------------------------,
    * |      |      |      |      |      |                             |      |SEL_L_WRD| SEL_U|SEL_R_WRD|      |
    * |------+------+------+------+------|                             |-------------+------+------+------|
    * |      |      |      |      |      |                             |SEL_END|SEL_L|SEL_D | SEL_R|      |
    * |------+------+------+------+------|                             |------|------+------+------+------|
    * |  UNDO| S_CUT|S_COPY| PASTE|      |                             |      |      |      |      |      |
    * |------+------+------+-------------,                             ,-------------+------+------+------,
    *        |      |MSHIFT|                                                         |      |      |
    *        '------+------'-------------'                             '-------------'------+------'
    *                             |      |                             |      |
    *                             |      |                             |      |
    *                             |      |                             |      |
    *                             +------'                             '------+
    *                                    '------+------' '------+------'
    *                                    |  MOVE|MSHIFT| |      |     |
    *                                    '------+------' '------+------'
    *                                    |      |      | |      |      |
    *                                    '------+------' '------+------'
    */
[_SELECT_MOVE] = LAYOUT( \
  ____, ____, ____,    ____,    ____,                                       ____, KC_SEL_L_WRD,KC_SEL_U, KC_SEL_R_WRD, ____,         \
  ____, ____, ____,    ____,    ____,                                       KC_SEL_END,KC_SEL_L,KC_SEL_D, KC_SEL_R, ____,         \
  KC_UNDO, KC_S_CUT, KC_S_COPY, KC_PASTE,    ____,                                       ____, ____,    ____, ____, ____,         \
        ____, KC_MSHIFT,                                                                        ____, ____,               \
                                  ____,                        ____,                                                 \
                                      KC_MOVE, ____,     ____,  ____,                                                   \
                                      KC_MSHIFT, ____,     ____,  ____
),
   /* SELECT_MOVE_SHIFT
    * ,----------------------------------,                             ,----------------------------------,
    * |      |      |      |      |      |                             |      |      |SEL_PG_UP|      |      |
    * |------+------+------+------+------|                             |-------------+------+------+------|
    * |      |      |      |      |      |                             |SEL_HOME|    |SEL_PG_D| KC_FORMAT |      |
    * |------+------+------+------+------|                             |------|------+------+------+------|
    * |      |      |      |      |      |                             |      |      |      |      |      |
    * |------+------+------+-------------,                             ,-------------+------+------+------,
    *        |      |MSHIFT|                                                         |      |      |
    *        '------+------'-------------'                             '-------------'------+------'
    *                             |      |                             |      |
    *                             |      |                             |      |
    *                             |      |                             |      |
    *                             +------'                             '------+
    *                                    '------+------' '------+------'
    *                                    |  MOVE|MSHIFT| |      |     |
    *                                    '------+------' '------+------'
    *                                    |      |      | |      |      |
    *                                    '------+------' '------+------'
    */
[_SELECT_MOVE_SHIFT] = LAYOUT( \
  ____, ____, ____,    ____,    ____,                                       ____, ____,    KC_SEL_PG_UP, ____, ____,         \
  ____, ____, ____,    ____,    ____,                                       KC_SEL_HOME, ____,    KC_SEL_PG_D, KC_FORMAT, ____,         \
  ____, ____, ____,    ____,    ____,                                       ____, ____,    ____, ____, ____,         \
        ____, KC_MSHIFT,                                                                        ____, ____,               \
                                  ____,                        ____,                                                 \
                                      KC_MOVE, ____,     ____,  ____,                                                   \
                                      KC_MSHIFT, ____,     ____,  ____
),

/* NUM
 * ,----------------------------------,                             ,----------------------------------,
 * | F1   | F2   | F3   | F4   | F5   |                             |      |  7   |  8   |  9   |     |
 * |------+------+------+------+------|                             |-------------+------+------+------|
 * |  F6  |  F7  |  F8  |  F9  |  F10 |                             |      |  4   |  5   |  6   |      |
 * |------+------+------+------+------|                             |------|------+------+------+------|
 * |  F11 |  F12 |      |      |      |                             |      |  1   |  2   |  3   |  ENT |
 * |------+------+------+-------------,                             ,-------------+------+------+------,
 *        | REST |      |                                                         |  0   |  +   |
 *        '------+------'-------------'                             '-------------'------+------'
 *                      |      |      |                             |      |      |
 *                      |      |      |                             |      |      |
 *                      |      |      |                             |      |      |
 *                      '------+------'                             '------+------'
 *                                    '------+------' '------+------'
 *                                    |      |      | |      |      |
 *                                    '------+------' '------+------'
 *                                    |      |      | |      |      |
 *                                    '------+------' '------+------'
 */


[_NUM] = LAYOUT( \
  KC_F1, KC_F2, KC_F3,    KC_F4,    KC_F5,                                       ____, KC_7,KC_8, KC_9, ____,         \
  KC_F6, KC_F7, KC_F8,    KC_F9,    KC_F10,                                       ____, KC_4,KC_5, KC_6, ____,         \
  KC_F11, KC_F12, ____,    ____,    ____,                                       ____, KC_1, KC_2, KC_3, KC_ENT,         \
        RESET, ____,                                                                      KC_0, KC_PLUS,             \
                                  ____,                        KC_PLUS,                                                 \
                                      ____, ____,     KC_DOT,  KC_0,                                                   \
                                      ____, ____,     ____,  ____
),

/* SYM
 * ,----------------------------------,                             ,----------------------------------,
 * | !    | @    |  #   |  $   |  %   |                             |  ^   |  &   |   *  |  +   |   =  |
 * |------+------+------+------+------|                             |-------------+------+------+------|
 * |  ~   |   `  |  {   |  }   |  [   |                             |   ]  |  (   |   )  |  '   |  "   |
 * |------+------+------+------+------|                             |------|------+------+------+------|
 * |      |      |  [   |  ]   |  _   |                             |   -  |  _   |      |      |  |   |
 * |------+------+------+-------------,                             ,-------------+------+------+------,
 *        |      |      |                                                         |  -   |  =   |
 *        '------+------'-------------'                             '-------------'------+------'
 *                      |      |      |                             |      |      |
 *                      |      |      |                             |      |      |
 *                      |      |      |                             |      |      |
 *                      '------+------'                             '------+------'
 *                                    '------+------' '------+------'
 *                                    |      |      | |      |      |
 *                                    '------+------' '------+------'
 *                                    |      |      | |      |      |
 *                                    '------+------' '------+------'
 */

[_SYM] = LAYOUT( \
  KC_EXLM,KC_AT, KC_HASH, KC_DLR,KC_PERC,                                       KC_CAR, KC_AMPR,KC_ASTR,KC_PLUS,KC_EQL,         \
  LSFT(KC_GRAVE), KC_GRAVE, LSFT(KC_LBRC), LSFT(KC_RBRC), KC_LBRC,             KC_RBRC, KC_LPRN,KC_RPRN, KC_QUOT, LSFT(KC_QUOT),         \
  ____,           ____,       KC_LBRC, KC_RBRC, LSFT(KC_MINUS),                KC_MINUS, LSFT(KC_MINUS), ____, ____, LSFT(KC_BSLS),         \
        ____, ____,                                                              KC_MINUS, KC_EQL,               \
                                  ____,                        ____,                                                 \
                                      ____, ____,     LSFT(KC_MINUS),  KC_MINUS,                                                   \
                                      ____, ____,     ____,  ____
),
/* MOUSE
 * ,----------------------------------,                             ,----------------------------------,
 * |      |      |      |      |      |                             |      |      |      |      |      |
 * |------+------+------+------+------|                             |-------------+------+------+------|
 * |      |      |      |      |      |                             |      |      |  up  |      |     |
 * |------+------+------+------+------|                             |------|------+------+------+------|
 * |      |      |      |      |      |                             |      | left | down | right|      |
 * |------+------+------+-------------,                             ,-------------+------+------+------,
 *        |      |      |                                                         |      |      |
 *        '------+------'-------------'                             '-------------'------+------'
 *                      |      |      |                             |      |      |
 *                      |      |      |                             |      |      |
 *                      |      |      |                             |      |      |
 *                      '------+------'                             '------+------'
 *                                    '------+------' '------+------'
 *                                    |      |      | |      |      |
 *                                    '------+------' '------+------'
 *                                    |      |      | |      |      |
 *                                    '------+------' '------+------'
 */

[_MOUSE] = LAYOUT( \
           ____, ____, ____,    ____,    ____,                                       ____, ____,   ____ , ____, ____,         \
           ____, ____, ____,    ____,    ____,                                       ____, ____, KC_MS_UP, ____, ____,         \
           ____, ____, ____,    ____,    ____,                                       ____, KC_MS_LEFT, KC_MS_DOWN, KC_MS_RIGHT, ____,         \
                 ____, ____,                                                                        ____, ____,               \
                                           ____,                        ____,                                                 \
                                               ____, ____,     ____,  ____,                                                   \
                                               ____, ____,     ____,  ____
         ),
};

void persistent_default_layer_set(uint16_t default_layer) {
  eeconfig_update_default_layer(default_layer);
  default_layer_set(default_layer);
}


/***********************
Trackball stuff
************************/

/***************************
 * Trackball handling
 **************************/

void pointing_device_init(void){
	if(!is_keyboard_master())
		return;

    mouse_moved_timer = timer_read();
	pmw_init();
}

int max(int num1, int num2) { return (num1 > num2 ) ? num1 : num2; }
int min(int num1, int num2) { return (num1 > num2 ) ? num2 : num1; }

int8_t sign(int x) { return (x > 0) - (x < 0); }
int8_t CLAMP_HID(int value) { return value < -127 ? -127 : value > 127 ? 127 : value; }

void tap_code_fast(uint8_t code) {
	register_code(code);
	// Dont do this:
	// if (code == KC_CAPS) {
	//	 wait_ms(TAP_HOLD_CAPS_DELAY);
	// } else {
	//	 wait_ms(TAP_CODE_DELAY);
	// }
	unregister_code(code);
}

void tap_tb(uint8_t keycode0, uint8_t keycode1, uint8_t keycode2, uint8_t keycode3) {
	if(abs(cum_x) + abs(cum_y) >= cur_factor){
		if(abs(cum_x) > abs(cum_y)) {
			if(cum_x > 0) {
				for (int8_t i = 0; i <= (abs(cum_x) + abs(cum_y)) / cur_factor; i++) {
					tap_code_fast(keycode0);
					cum_x = max(cum_x - cur_factor, 0);
				}
				cum_y = 0;
			} else {
				for (int8_t i = 0; i <= (abs(cum_x) + abs(cum_y)) / cur_factor; i++) {
					tap_code_fast(keycode1);
					cum_x = min(cum_x + cur_factor, 0);
				}
				cum_y = 0;
			}
		} else {
			if(cum_y > 0) {
				for (int8_t i = 0; i <= (abs(cum_x) + abs(cum_y)) / cur_factor; i++) {
					tap_code_fast(keycode2);
					cum_y = max(cum_y - cur_factor, 0);
					}
				cum_x = 0;
			} else {
				for (int8_t i = 0; i <= (abs(cum_x) + abs(cum_y)) / cur_factor; i++) {
					tap_code_fast(keycode3);
					cum_y = min(cum_y + cur_factor, 0);
				}
				cum_x = 0;
			}
		}
	}
}

void handle_pointing_device_modes(void){
	report_mouse_t mouse_report = pointing_device_get_report();
	if (track_mode == cursor_mode) {
		if (integration_mode)
			cur_factor = cursor_multiplier_inte;
		else
			cur_factor = cursor_multiplier;
		mouse_report.x = CLAMP_HID( sensor_x * cur_factor / 100);
		mouse_report.y = CLAMP_HID(-sensor_y * cur_factor / 100);
		if (sensor_x != 0 || sensor_y != 0) {
		  mouse_moved_timer = timer_read();
		}
	} else {
		// accumulate movement until threshold reached
		cum_x += sensor_x;
		cum_y += sensor_y;
		if (track_mode == carret_mode) {
			if (integration_mode)
				cur_factor = carret_threshold_inte;
			else
				cur_factor = carret_threshold;
			tap_tb(KC_RIGHT, KC_LEFT, KC_UP, KC_DOWN);

		} else if(track_mode == scroll_mode) {
				if (integration_mode)
					cur_factor = scroll_threshold_inte;
				else
					cur_factor = scroll_threshold;
				if(abs(cum_x) + abs(cum_y) >= cur_factor) {
					if(abs(cum_x) > abs(cum_y)) {
						mouse_report.h = sign(cum_x) * (abs(cum_x) + abs(cum_y)) / cur_factor;
					} else {
						mouse_report.v = sign(cum_y) * (abs(cum_x) + abs(cum_y)) / cur_factor;
					}
					cum_x = 0;
					cum_y = 0;
				}
		} else { // sound vol/brightness (3)
			cur_factor = carret_threshold;
			tap_tb(KC_BRIGHTNESS_UP, KC_BRIGHTNESS_DOWN, KC_AUDIO_VOL_UP, KC_AUDIO_VOL_DOWN);
		}
	}
	pointing_device_set_report(mouse_report);
	pointing_device_send();
}

void get_sensor_data(void) {
	if(!is_keyboard_master())
		return;
	report_pmw_t pmw_report = pmw_get_report();

	if (integration_mode) {
		sensor_x += pmw_report.x;
		sensor_y += pmw_report.y;
	} else {
		sensor_x = pmw_report.x;
		sensor_y = pmw_report.y;
	}
}

void pointing_device_task(void) {
#ifndef POLLING
	if ( is_keyboard_master() && integration_mode )
		handle_pointing_device_modes();
#else
	get_sensor_data();
	handle_pointing_device_modes();
#endif
}

#ifndef POLLING
	ISR(INT6_vect) {
		get_sensor_data();
		handle_pointing_device_modes();
	}
#endif

