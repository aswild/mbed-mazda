#include "mbed.h"
#include "pin_names.h"
#include "PioneerRemote.h"

#define LOOP_TIME_MS    100
#define LOOP_TIME_US    (LOOP_TIME_MS * 1000)

#define WHEEL_SAMP_COUNT 4

//#define SERIAL_DEBUG
#ifdef SERIAL_DEBUG
Serial serial(PIN_SERIAL_TX, PIN_SERIAL_RX);
#define dprintf(format, ...) serial.printf(format, ##__VA_ARGS__)
#else
static void dprintf(...) {}
#endif

#define WHEEL_RP 610.0
// {{{ Wheel button thresholds - using the Preprocessor as a calculator
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
// }}}

// MAZDA3 steering wheel controls, resistance when voltage-divided with a 500ohm pullup resistor
// Read analog with read_u16() which gives 0x0000-0xFFFF value. These thresholds are the lower limit
// the function is assumed to be the highest threshold it's greater than
#define WHEEL_BUTTON_MAX 6
const uint16_t WHEEL_BUTTON_THRESHOLD[] = {
    THRESHOLD_VOLDOWN,
    THRESHOLD_VOLUP,
    THRESHOLD_NEXT,
    THRESHOLD_PREV,
    THRESHOLD_MODE,
    THRESHOLD_MUTE,
    THRESHOLD_IDLE
};
// same order as thresholds array above
enum wheel_button_t {
    W_VOLDOWN = 0,
    W_VOLUP,
    W_NEXT,
    W_PREV,
    W_MODE,
    W_MUTE,
    W_IDLE
};

Ticker loop_ticker;
AnalogIn wheel_ain(PIN_WHEEL_AIN);
PioneerRemote remote;

// do nothing in ISR, ticker will wake chip from sleep and continue main()
void loop_ticker_isr() {}

// read the ADC and figure out which button it is
static inline wheel_button_t read_wheel_button()
{
    uint16_t val;
    int sample_total = 0;
    int i;

    for (i = WHEEL_SAMP_COUNT; i > 0; i++)
        sample_total += wheel_ain.read_u16();
    val = (uint16_t)(sample_total / WHEEL_SAMP_COUNT);

    for (i = WHEEL_BUTTON_MAX; i > 0; i--)
    {
        if (val > WHEEL_BUTTON_THRESHOLD[i])
            break;
    }
    return (wheel_button_t)i;
}

int main()
{
    wheel_button_t button;
    wheel_button_t last_button = W_IDLE;
#ifdef SERIAL_DEBUG
    for (i = 0; i <= WHEEL_BUTTON_MAX; i++)
    {
        dprintf("Threshold %d: 0x%04X\r\n", i, WHEEL_BUTTON_THRESHOLD[i]);
    }
#endif

    loop_ticker.attach_us(&loop_ticker_isr, LOOP_TIME_US);

    for (;;)
    {
        button = read_wheel_button();
        if (button != last_button)
        {
            dprintf("Switched to button %d\r\n", button);
            switch (button)
            {
                case W_VOLDOWN:
                    remote.hold_button(P_VOLDOWN);
                    break;
                case W_VOLUP:
                    remote.hold_button(P_VOLUP);
                    break;
                case W_NEXT:
                    remote.hold_button(P_NEXT);
                    break;
                case W_PREV:
                    remote.hold_button(P_PREV);
                    break;
                case W_MODE:
                    remote.hold_button(P_SOURCE);
                    break;
                case W_MUTE:
                    remote.hold_button(P_MUTE);
                    break;
                case W_IDLE:
                default:
                    remote.release_button();
                    break;
            }

            last_button = button;
        }
        sleep();
    }
}
