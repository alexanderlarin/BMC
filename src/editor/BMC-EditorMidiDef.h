/*
  See https://www.RoxXxtar.com/bmc for more details
  Copyright (c) 2020 RoxXxtar.com
  Licensed under the MIT license.
  See LICENSE file in the project root for full license information.

  Contains all the function ids for all editor communication
*/
#ifndef BMC_EDITOR_MIDI_DEF_H
#define BMC_EDITOR_MIDI_DEF_H

// if read/write flag is 0 it's checking the connection status,
// if 1, it's trying to connect
#define BMC_GLOBALF_CONNECT 0
// ignores read/write flag
#define BMC_GLOBALF_DISCONNECT 1
#define BMC_GLOBALF_SETTINGS 2
#define BMC_GLOBALF_LIBRARY 3
#define BMC_GLOBALF_LEDS 4
// BMC_GLOBALF_POT_CALIBRATION doesn't use write
#define BMC_GLOBALF_POT_CALIBRATION 5
#define BMC_GLOBALF_CUSTOM_SYSEX 6
#define BMC_GLOBALF_TRIGGERS 7
#define BMC_GLOBALF_TEMPO_TO_TAP 8
#define BMC_GLOBALF_PRESET 9
#define BMC_GLOBALF_STARTUP 10
#define BMC_GLOBALF_STORE_ADDRESS 11
#define BMC_GLOBALF_BACKUP 12
#define BMC_GLOBALF_PARAMETER 13
#define BMC_GLOBALF_FORCE_DISCONNECT 14
#define BMC_GLOBALF_EDITOR_FETCH_COMPLETE 15
#define BMC_GLOBALF_SKETCH_BYTES 16
#define BMC_GLOBALF_SKETCH_BYTES_DATA 17
#define BMC_GLOBALF_NL_RELAYS 18
#define BMC_GLOBALF_L_RELAYS 19
#define BMC_GLOBALF_TIME 20
#define BMC_GLOBALF_PORTS_PRESETS 21
#define BMC_GLOBALF_EDITOR_FEEDBACK 22
#define BMC_GLOBALF_EDITOR_PERFORM_MODE 23
#define BMC_GLOBALF_EDITOR_GET_METRICS 24
#define BMC_GLOBALF_EDITOR_MESSENGER 25
#define BMC_GLOBALF_SETLISTS 26
#define BMC_GLOBALF_STRING_LIBRARY 27
#define BMC_GLOBALF_BUTTON 28
#define BMC_GLOBALF_ENCODER 29
#define BMC_GLOBALF_POT 30
// BMC_GLOBALF_POT_CALIBRATION doesn't use write
#define BMC_GLOBALF_GLOBAL_POT_CALIBRATION 31
#define BMC_GLOBALF_BUTTON_EVENT_SHIFT_POSITION 32

// get build info, includes things like PINS, number of buttons, leds, etc. Page Number bytes specify the item being retrieved.
#define BMC_GLOBALF_BUILD_INFO 125
    #define BMC_GLOBALF_BUILD_INFO_FLAGS 0
    #define BMC_GLOBALF_BUILD_INFO_PINS_BUTTONS 1
    #define BMC_GLOBALF_BUILD_INFO_PINS_LEDS 2
    #define BMC_GLOBALF_BUILD_INFO_PINS_PWM_LEDS 3
    #define BMC_GLOBALF_BUILD_INFO_PINS_POTS 4
    #define BMC_GLOBALF_BUILD_INFO_PINS_ENCODERS 5
    #define BMC_GLOBALF_BUILD_INFO_PINS_GLOBAL_LEDS 6
    #define BMC_GLOBALF_BUILD_INFO_DEVICE_NAME 7
    #define BMC_GLOBALF_BUILD_INFO_PINS_NL_RELAYS 8
    #define BMC_GLOBALF_BUILD_INFO_PINS_L_RELAYS 9
    #define BMC_GLOBALF_BUILD_INFO_PINS_PIXELS 10
    #define BMC_GLOBALF_BUILD_INFO_MERGE_BUTTON_LEDS 12
    #define BMC_GLOBALF_BUILD_INFO_MERGE_ENCODERS 14
    #define BMC_GLOBALF_BUILD_INFO_MERGE_POTS 15
    #define BMC_GLOBALF_BUILD_INFO_MERGE_BI_LEDS 16
    #define BMC_GLOBALF_BUILD_INFO_MERGE_RGB_LEDS 17
    #define BMC_GLOBALF_BUILD_INFO_MERGE_BI_GLOBAL_LEDS 18
    #define BMC_GLOBALF_BUILD_INFO_MERGE_RGB_GLOBAL_LEDS 19
    #define BMC_GLOBALF_BUILD_INFO_MERGE_BI_PWM_LEDS 20
    #define BMC_GLOBALF_BUILD_INFO_MERGE_RGB_PWM_LEDS 21
    #define BMC_GLOBALF_BUILD_INFO_MERGE_RELAYS_L 22
    #define BMC_GLOBALF_BUILD_INFO_MERGE_RELAYS_NL 23
    #define BMC_GLOBALF_BUILD_INFO_MERGE_BUTTON_PIXELS 24
    #define BMC_GLOBALF_BUILD_INFO_PINS_RGB_PIXELS 26
    #define BMC_GLOBALF_BUILD_INFO_MERGE_BUTTON_RGB_PIXELS 27
    #define BMC_GLOBALF_BUILD_INFO_PINS_GLOBAL_BUTTONS 28
    #define BMC_GLOBALF_BUILD_INFO_PINS_GLOBAL_ENCODERS 29
    #define BMC_GLOBALF_BUILD_INFO_PINS_GLOBAL_POTS 30
    #define BMC_GLOBALF_BUILD_INFO_PINS_GLOBAL_POTS_2 31
    #define BMC_GLOBALF_BUILD_INFO_PINS_GLOBAL_POTS_3 32
    #define BMC_GLOBALF_BUILD_INFO_PINS_GLOBAL_POTS_4 33
    #define BMC_GLOBALF_BUILD_INFO_PINS_POTS_2 34
    #define BMC_GLOBALF_BUILD_INFO_PINS_POTS_3 35
    #define BMC_GLOBALF_BUILD_INFO_PINS_POTS_4 36

#define BMC_GLOBALF_UTILITY 126
    // UTILITY FUNCTIONS
    #define BMC_UTILF_BUTTON 0
    #define BMC_UTILF_LED 1
    #define BMC_UTILF_POT 2
    #define BMC_UTILF_ENCODER 3
    #define BMC_UTILF_GLOBAL_LED 4
    #define BMC_UTILF_POTS 5
    #define BMC_UTILF_PRESET 6
    #define BMC_UTILF_CLICK_TRACK 7
    #define BMC_UTILF_POT_CALIBRATION 8
    #define BMC_UTILF_POT_CALIBRATION_STATUS 9
    #define BMC_UTILF_POT_CALIBRATION_CANCEL 10
    #define BMC_UTILF_DISCONNECTED 11
    #define BMC_UTILF_LED_TEST 12
    #define BMC_UTILF_PWM_LED 13
    #define BMC_UTILF_PWM_LED_TEST 14
    #define BMC_UTILF_SKETCH_BYTES 15
    #define BMC_UTILF_NL_RELAY 16
    #define BMC_UTILF_L_RELAY 17
    #define BMC_UTILF_PIXEL 18
    #define BMC_UTILF_AUX_JACK 20
    #define BMC_UTILF_RGB_PIXEL 21
    #define BMC_UTILF_GLOBAL_BUTTON 22
    #define BMC_UTILF_GLOBAL_ENCODER 23
    #define BMC_UTILF_GLOBAL_POT 24
    #define BMC_UTILF_GLOBAL_POTS 25
    #define BMC_UTILF_GLOBAL_POT_CALIBRATION 26
    #define BMC_UTILF_GLOBAL_POT_CALIBRATION_STATUS 27
    #define BMC_UTILF_GLOBAL_POT_CALIBRATION_CANCEL 28


// if read/write flag is 0 it's checking the connection status, if 1, it's trying to connect
#define BMCF_NOTIFY 127
    // codes for notifications
    #define BMC_NOTIFY_CONNECTION 0
    #define BMC_NOTIFY_CONNECTION_STATUS 1
    #define BMC_NOTIFY_DISCONNECTED 2
    #define BMC_NOTIFY_SUCCESS 3
    #define BMC_NOTIFY_UNAVAILABLE 4
    #define BMC_NOTIFY_INVALID_REQUEST 5
    #define BMC_NOTIFY_INVALID_CHECKSUM 6
    #define BMC_NOTIFY_INVALID_SIZE 7
    #define BMC_NOTIFY_INVALID_INDEX 8
    #define BMC_NOTIFY_INVALID_PAGE 9
    #define BMC_NOTIFY_INVALID_BUTTON 10
    #define BMC_NOTIFY_INVALID_BUTTON_EVENT 11
    #define BMC_NOTIFY_INVALID_LED 12
    #define BMC_NOTIFY_INVALID_PWM_LED 13
    #define BMC_NOTIFY_INVALID_ENCODER 14
    #define BMC_NOTIFY_INVALID_POT 15
    #define BMC_NOTIFY_INVALID_GLOBAL_LED 16
    #define BMC_NOTIFY_INVALID_LIBRARY 17
    #define BMC_NOTIFY_INVALID_PRESET 18
    #define BMC_NOTIFY_INVALID_CUSTOM_SYSEX 19
    #define BMC_NOTIFY_INVALID_TEMPO_TO_TAP 20
    #define BMC_NOTIFY_INVALID_TRIGGER 21
    #define BMC_NOTIFY_INVALID_SKETCH_BYTE 22
    #define BMC_NOTIFY_INVALID_STORE_ADDRESS 23
    #define BMC_NOTIFY_INVALID_COPY_REQUEST 24
    #define BMC_NOTIFY_INVALID_SWAP_REQUEST 25
    #define BMC_NOTIFY_BACKUP_DATA_ACCEPTED 26
    #define BMC_NOTIFY_INVALID_SKETCH_BYTE_DATA 27
    #define BMC_NOTIFY_INVALID_NL_RELAY 28
    #define BMC_NOTIFY_INVALID_L_RELAY 29
    #define BMC_NOTIFY_REQUEST_ACCEPTED 30
    #define BMC_NOTIFY_INVALID_PIXEL 31
    #define BMC_NOTIFY_INVALID_RGB_PIXEL 32
    #define BMC_NOTIFY_INVALID_SETLIST 33
    #define BMC_NOTIFY_INVALID_SETLIST_SONG 34
    #define BMC_NOTIFY_INVALID_STRING_LIBRARY 35
    #define BMC_NOTIFY_INVALID_GLOBAL_BUTTON 36
    #define BMC_NOTIFY_INVALID_GLOBAL_ENCODER 37
    #define BMC_NOTIFY_INVALID_GLOBAL_POT 38

// PAGE FUNCTIONS
#define BMC_PAGEF_PAGE 0
#define BMC_PAGEF_BUTTON 1
#define BMC_PAGEF_LED 3
#define BMC_PAGEF_POT 4
#define BMC_PAGEF_ENCODER 5
#define BMC_PAGEF_PWM_LED 6
#define BMC_PAGEF_PIXEL 7
#define BMC_PAGEF_HARDWARE_COPY 8
#define BMC_PAGEF_HARDWARE_SWAP 9
#define BMC_PAGEF_PAGE_NAME 10
#define BMC_PAGEF_BUTTON_EVENT_SHIFT_POSITION 11
#define BMC_PAGEF_RGB_PIXEL 13


#endif
