/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bsp/board_api.h"
#include "tusb.h"
#include "usb_descriptors.h"
#include "bootstrap.bas.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED     = 1000,
  BLINK_SUSPENDED   = 2500,
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;
static size_t s_chars_sent = 0;
static size_t s_insertion_delay = 0;

void led_blinking_task(void);
void hid_task(void);

/*------------- MAIN -------------*/
int main(void) {
  board_init();

  // init device stack on configured roothub port
  tusb_rhport_init_t dev_init = {.role = TUSB_ROLE_DEVICE, .speed = TUSB_SPEED_AUTO};
  tusb_init(BOARD_TUD_RHPORT, &dev_init);

  board_init_after_tusb();
  while (1) {
    tud_task(); // tinyusb device task
    led_blinking_task();

    hid_task();
  }
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) {
  blink_interval_ms = BLINK_MOUNTED;
  s_chars_sent = 0;
  s_insertion_delay = 0;
}

// Invoked when device is unmounted
void tud_umount_cb(void) {
  blink_interval_ms = BLINK_NOT_MOUNTED;
  s_chars_sent = 0;
  s_insertion_delay = 0;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
  (void)remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {
  blink_interval_ms = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

static void
character_to_hid_key_and_modifier(const uint8_t character, uint8_t *keycode_p, uint8_t *modifier_p)
{
    *modifier_p = 0;
    *keycode_p = HID_KEY_NONE;

    if (character >= 'a' && character <= 'z') {
        *keycode_p = HID_KEY_A + (character - 'a');
    } else if (character >= 'A' && character <= 'Z') {
        *modifier_p = KEYBOARD_MODIFIER_LEFTSHIFT;
        *keycode_p = HID_KEY_A + (character - 'A');
    } else {
        switch (character) {
        /*
         * Why is it that the ASCII table orders numbers from 0-9 and
         * HID keycodes are 1-0... At least A-Z are the same order for both
         */
        case '!':
            *modifier_p = KEYBOARD_MODIFIER_LEFTSHIFT;
        case '1':
            *keycode_p = HID_KEY_1;
            break;
        case '@':
            *modifier_p = KEYBOARD_MODIFIER_LEFTSHIFT;
        case '2':
            *keycode_p = HID_KEY_2;
            break;
        case '#':
            *modifier_p = KEYBOARD_MODIFIER_LEFTSHIFT;
        case '3':
            *keycode_p = HID_KEY_3;
            break;
        case '$':
            *modifier_p = KEYBOARD_MODIFIER_LEFTSHIFT;
        case '4':
            *keycode_p = HID_KEY_4;
            break;
        case '%':
            *modifier_p = KEYBOARD_MODIFIER_LEFTSHIFT;
        case '5':
            *keycode_p = HID_KEY_5;
            break;
        case '^':
            *modifier_p = KEYBOARD_MODIFIER_LEFTSHIFT;
        case '6':
            *keycode_p = HID_KEY_6;
            break;
        case '&':
            *modifier_p = KEYBOARD_MODIFIER_LEFTSHIFT;
        case '7':
            *keycode_p = HID_KEY_7;
            break;
        case '*':
            *modifier_p = KEYBOARD_MODIFIER_LEFTSHIFT;
        case '8':
            *keycode_p = HID_KEY_8;
            break;
        case '(':
            *modifier_p = KEYBOARD_MODIFIER_LEFTSHIFT;
        case '9':
            *keycode_p = HID_KEY_9;
            break;
        case ')':
            *modifier_p = KEYBOARD_MODIFIER_LEFTSHIFT;
        case '0':
            *keycode_p = HID_KEY_0;
            break;
        case '\n':
            *keycode_p = HID_KEY_ENTER;
            break;
        case ' ':
            *keycode_p = HID_KEY_SPACE;
            break;
        case '>':
            *modifier_p = KEYBOARD_MODIFIER_LEFTSHIFT;
        case '.':
            *keycode_p = HID_KEY_PERIOD;
            break;
        case '<':
            *modifier_p = KEYBOARD_MODIFIER_LEFTSHIFT;
        case ',':
            *keycode_p = HID_KEY_COMMA;
            break;
        case '_':
            *modifier_p = KEYBOARD_MODIFIER_LEFTSHIFT;
        case '-':
            *keycode_p = HID_KEY_MINUS;
            break;
        case '{':
            *modifier_p = KEYBOARD_MODIFIER_LEFTSHIFT;
        case '[':
            *keycode_p = HID_KEY_BRACKET_LEFT;
            break;
        case '}':
            *modifier_p = KEYBOARD_MODIFIER_LEFTSHIFT;
        case ']':
            *keycode_p = HID_KEY_BRACKET_RIGHT;
            break;
        case '+':
            *modifier_p = KEYBOARD_MODIFIER_LEFTSHIFT;
        case '=':
            *keycode_p = HID_KEY_EQUAL;
            break;
        case '"':
            *modifier_p = KEYBOARD_MODIFIER_LEFTSHIFT;
        case '\'':
            *keycode_p = HID_KEY_APOSTROPHE;
            break;
        case '?':
            *modifier_p = KEYBOARD_MODIFIER_LEFTSHIFT;
        case '/':
            *keycode_p = HID_KEY_SLASH;
            break;
        case ':':
            *modifier_p = KEYBOARD_MODIFIER_LEFTSHIFT;
        case ';':
            *keycode_p = HID_KEY_SEMICOLON;
            break;
        case '|':
            *modifier_p = KEYBOARD_MODIFIER_LEFTSHIFT;
        case '\\':
            *keycode_p = HID_KEY_BACKSLASH;
            break;
        }
    }
}

typedef enum {
    FSM_STATE_IDLE,
    FSM_STATE_KEY_SEND,
    FSM_STATE_KEY_RELEASE,
} fsm_key_state_et;

static fsm_key_state_et s_current_fsm_state = FSM_STATE_IDLE;

static void
fsm_state_action (fsm_key_state_et next_fsm_state)
{
    uint8_t keycode[6]  = {0};
    uint8_t report_id   = 0;
    uint8_t modifier    = 0;

    switch (next_fsm_state) {
    case FSM_STATE_IDLE:
        break;
    case FSM_STATE_KEY_SEND:
        character_to_hid_key_and_modifier(bootstrap_bas[s_chars_sent], &keycode[0], &modifier);
        tud_hid_n_keyboard_report(ITF_NUM_KEYBOARD, report_id, modifier, keycode);
        s_chars_sent++;
        break;
    case FSM_STATE_KEY_RELEASE:
        character_to_hid_key_and_modifier(bootstrap_bas[s_chars_sent-1], &keycode[0], &modifier);
        tud_hid_n_keyboard_report(ITF_NUM_KEYBOARD, report_id, modifier, NULL);
        break;
    }
}

static void
send_character (void)
{
    fsm_key_state_et next_fsm_state = FSM_STATE_IDLE;

    /*
     * Need to send an empty report for each key that was sent to indicate key up
     */

    switch (s_current_fsm_state) {
    case FSM_STATE_IDLE:
        /*
         * Wait 200 * 10ms (2000ms) before sending keystrokes as the OS may drop
         * the first few if we start too soon after connection.
         */
        if (s_insertion_delay == 200 && 
            s_chars_sent < bootstrap_bas_len) {
            next_fsm_state = FSM_STATE_KEY_SEND;
        } else {
          s_insertion_delay++;
        }
        break;
    case FSM_STATE_KEY_SEND:
        next_fsm_state = FSM_STATE_KEY_RELEASE;
        break;
    case FSM_STATE_KEY_RELEASE:
        next_fsm_state = FSM_STATE_IDLE;
        break;
    }

    fsm_state_action(next_fsm_state);
    s_current_fsm_state = next_fsm_state;
}

// Every 10ms, we will sent 1 report for each HID profile (keyboard, mouse etc ..)
// tud_hid_report_complete_cb() is used to send the next report after previous one is complete
void hid_task(void) {
  // Poll every 10ms
  const uint32_t  interval_ms = 10;
  static uint32_t start_ms    = 0;

  if (board_millis() - start_ms < interval_ms) {
    return; // not enough time
  }
  start_ms += interval_ms;

  uint32_t const btn = board_button_read();

  if (tud_suspended() && btn) {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    tud_remote_wakeup();
  } else {
    // keyboard interface
    if (tud_hid_n_ready(ITF_NUM_KEYBOARD)) {
      // used to avoid send multiple consecutive zero report for keyboard
      send_character();
    }
  }
}

// Invoked when received SET_PROTOCOL request
// protocol is either HID_PROTOCOL_BOOT (0) or HID_PROTOCOL_REPORT (1)
void tud_hid_set_protocol_cb(uint8_t instance, uint8_t protocol) {
  (void)instance;
  (void)protocol;

  // nothing to do since we use the same compatible boot report for both Boot and Report mode.
  // TODO set a indicator for user
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint16_t len) {
  (void)instance;
  (void)report;
  (void)len;

  // nothing to do
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(
    uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
  // TODO not Implemented
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(
    uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
  (void)report_id;

  // keyboard interface
  if (instance == ITF_NUM_KEYBOARD) {
    // Set keyboard LED e.g Capslock, Numlock etc...
    if (report_type == HID_REPORT_TYPE_OUTPUT) {
      // bufsize should be (at least) 1
      if (bufsize < 1) {
        return;
      }

      uint8_t const kbd_leds = buffer[0];

      if (kbd_leds & KEYBOARD_LED_CAPSLOCK) {
        // Capslock On: disable blink, turn led on
        blink_interval_ms = 0;
        board_led_write(true);
      } else {
        // Caplocks Off: back to normal blink
        board_led_write(false);
        blink_interval_ms = BLINK_MOUNTED;
      }
    }
  }
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void) {
  static uint32_t start_ms  = 0;
  static bool     led_state = false;

  // blink is disabled
  if (!blink_interval_ms) {
    return;
  }

  // Blink every interval ms
  if (board_millis() - start_ms < blink_interval_ms) {
    return; // not enough time
  }
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}
