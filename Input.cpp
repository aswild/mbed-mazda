/***********************************************************************
 * Input.cpp                                                           *
 * Provides the function read_input_button and the associated overhead *
 ***********************************************************************/

#define __INPUT_CPP__
#include "mbed.h"
#include "pin_names.h"
#include "Input.h"

/*********************** LPC1114 implementation *************************/
#ifdef TARGET_LPC1114

AnalogIn wheel_ain(PIN_WHEEL_AIN);

// read the ADC and figure out which button it is
input_button_t read_input_button()
{
    uint16_t val;
    int sample_total = 0;
    int i;

    for (i = WHEEL_SAMP_COUNT; i > 0; i--)
        sample_total += wheel_ain.read_u16();
    val = (uint16_t)(sample_total / WHEEL_SAMP_COUNT);

    for (i = NUM_WHEEL_BUTTONS-1; i > 0; i--)
    {
        if (val > WHEEL_BUTTON_THRESHOLD[i])
            break;
    }
    return (input_button_t)i;
}

/*********************** LPC1768 implementation *************************/
#elif defined(TARGET_LPC1768)
#include "MPR121.h"

I2C keypad_i2c(PIN_KEYPAD_SDA, PIN_KEYPAD_SCL);
MPR121 keypad(&keypad_i2c, MPR121::ADD_VSS);

// for LPC1768, use MPR121 keypad as substitute for wheel buttons
input_button_t read_input_button()
{
    static const input_button_t keypad_map[] = {
        W_VOLUP,    // 0
        W_IDLE,     // 1
        W_NEXT,     // 2
        W_MODE,     // 3
        W_VOLDOWN,  // 4
        W_IDLE,     // 5
        W_PREV,     // 6
        W_MUTE,     // 7
        W_STATUSCHECK, // 8 - special LPC1768 only type
        W_IDLE,     // 9
        W_IDLE,     // 10
        W_IDLE,     // 11
        W_IDLE      // 12
    };
    uint16_t keycode = (keypad.read(1) << 8) | keypad.read(0);
    int i;
    for (i = 0; i < 12; i++)
        if (keycode & (1 << i))
            break;
    return keypad_map[i];
}
#endif
