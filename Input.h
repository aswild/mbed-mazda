/***********************************************************************
 * Input.h                                                             *
 * Defintions and shared macros for Input.cpp                          *
 ***********************************************************************/

#ifndef __INPUT_H__
#define __INPUT_H__

#define WHEEL_SAMP_COUNT 8

#define WHEEL_RP 610.0
// Wheel button thresholds - using the Preprocessor as a calculator
#define R_WB1   56.0
#define R_WB2  149.1
#define R_WB3  303.4
#define R_WB4  565.0
#define R_WB5 1040.0
#define R_WB6 2041.0
#define R_WB7 5046.0

#define U16_WB1 ((uint16_t)((65535 * R_WB1) / (R_WB1 + WHEEL_RP)))
#define U16_WB2 ((uint16_t)((65535 * R_WB2) / (R_WB2 + WHEEL_RP)))
#define U16_WB3 ((uint16_t)((65535 * R_WB3) / (R_WB3 + WHEEL_RP)))
#define U16_WB4 ((uint16_t)((65535 * R_WB4) / (R_WB4 + WHEEL_RP)))
#define U16_WB5 ((uint16_t)((65535 * R_WB5) / (R_WB5 + WHEEL_RP)))
#define U16_WB6 ((uint16_t)((65535 * R_WB6) / (R_WB6 + WHEEL_RP)))
#define U16_WB7 ((uint16_t)((65535 * R_WB7) / (R_WB7 + WHEEL_RP)))

#define T_WB1 (U16_WB1 / 2)
#define T_WB2 ((U16_WB2 + U16_WB1) / 2)
#define T_WB3 ((U16_WB3 + U16_WB2) / 2)
#define T_WB4 ((U16_WB4 + U16_WB3) / 2)
#define T_WB5 ((U16_WB5 + U16_WB4) / 2)
#define T_WB6 ((U16_WB6 + U16_WB5) / 2)
#define T_WB7 ((U16_WB7 + U16_WB6) / 2)

#define THRESHOLD_VOLDOWN   T_WB1
#define THRESHOLD_VOLUP     T_WB2
#define THRESHOLD_NEXT      T_WB3
#define THRESHOLD_PREV      T_WB4
#define THRESHOLD_MODE      T_WB5
#define THRESHOLD_MUTE      T_WB6
#define THRESHOLD_IDLE      T_WB7

#define NUM_WHEEL_BUTTONS 7
#ifdef TARGET_LPC1114
#define NUM_INPUT_BUTTONS 7
#elif defined(TARGET_LPC1768)
// LPC1768 extra buttons on keypad
#define NUM_INPUT_BUTTONS 8
#endif

enum input_button_t {
    W_VOLDOWN = 0,
    W_VOLUP,
    W_NEXT,
    W_PREV,
    W_MODE,
    W_MUTE,
    W_IDLE
#ifdef TARGET_LPC1768
    , W_STATUSCHECK
#endif
};

#ifdef __INPUT_CPP__
// MAZDA3 steering wheel controls, resistance when voltage-divided with a pullup resistor
// Read analog with read_u16() which gives 0x0000-0xFFFF value. These thresholds are the lower limit
// the function is assumed to be the highest threshold it's greater than
extern const uint16_t WHEEL_BUTTON_THRESHOLD[NUM_WHEEL_BUTTONS] = {
    THRESHOLD_VOLDOWN,
    THRESHOLD_VOLUP,
    THRESHOLD_NEXT,
    THRESHOLD_PREV,
    THRESHOLD_MODE,
    THRESHOLD_MUTE,
    THRESHOLD_IDLE
};
extern const char *INPUT_BUTTON_STR[NUM_INPUT_BUTTONS] = {
    "VOLDOWN",
    "VOLUP",
    "NEXT",
    "PREV",
    "MODE",
    "MUTE",
    "IDLE"
#ifdef TARGET_LPC1768
    , "STATUSCHECK"
#endif
};

#else
// not in Input.cpp, so don't re-define
extern const uint16_t WHEEL_BUTTON_THRESHOLD[NUM_WHEEL_BUTTONS];
extern const char    *INPUT_BUTTON_STR[NUM_INPUT_BUTTONS];
#endif

input_button_t read_input_button();

#endif // __INPUT_H__
