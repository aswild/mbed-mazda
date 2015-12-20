#include "mbed.h"
#include "pin_names.h"
#include "PioneerRemote.h"

#define LOOP_TIME_MS    50
#define LOOP_TIME_US    (LOOP_TIME_MS * 1000)

#define SERIAL_DEBUG
#ifdef SERIAL_DEBUG
Serial serial(PIN_SERIAL_TX, PIN_SERIAL_RX);
#define dprintf(format, ...) serial.printf(format, ##__VA_ARGS__)
#else
static void dprintf(...) {}
#endif

#define WHEEL_RP 500.0
// {{{ Wheel button thresholds - using the Preprocessor as a calculator
#define VDD      3.3
#define R_WB0 5046.0
#define R_WB1   56.0
#define R_WB2  149.1
#define R_WB3  303.4
#define R_WB4  565.0
#define R_WB5 1040.0
#define R_WB6 2041.0

#define V_WB0 (VDD / (1.0 + (WHEEL_RP / R_WB0)))
#define V_WB1 (VDD / (1.0 + (WHEEL_RP / R_WB1)))
#define V_WB2 (VDD / (1.0 + (WHEEL_RP / R_WB2)))
#define V_WB3 (VDD / (1.0 + (WHEEL_RP / R_WB3)))
#define V_WB4 (VDD / (1.0 + (WHEEL_RP / R_WB4)))
#define V_WB5 (VDD / (1.0 + (WHEEL_RP / R_WB5)))
#define V_WB6 (VDD / (1.0 + (WHEEL_RP / R_WB6)))

#define U16_WB0 ((uint16_t)(V_WB0 * 65535.0))
#define U16_WB1 ((uint16_t)(V_WB1 * 65535.0))
#define U16_WB2 ((uint16_t)(V_WB2 * 65535.0))
#define U16_WB3 ((uint16_t)(V_WB3 * 65535.0))
#define U16_WB4 ((uint16_t)(V_WB4 * 65535.0))
#define U16_WB5 ((uint16_t)(V_WB5 * 65535.0))
#define U16_WB6 ((uint16_t)(V_WB6 * 65535.0))

#define THRESHOLD_IDLE      U16_WB0
#define THRESHOLD_VOLDOWN   U16_WB1
#define THRESHOLD_VOLUP     U16_WB2
#define THRESHOLD_NEXT      U16_WB3
#define THRESHOLD_PREV      U16_WB4
#define THRESHOLD_MODE      U16_WB5
#define THRESHOLD_MUTE      U16_WB6
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

static inline wheel_button_t read_wheel_button()
{
    uint16_t val = wheel_ain.read_u16();
    int i;

    for (i = WHEEL_BUTTON_MAX; i < 0; i--)
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
    int i;
    
    dprintf("0: %f\r\n", V_WB0);
    dprintf("1: %f\r\n", V_WB1);
    dprintf("2: %f\r\n", V_WB2);
    dprintf("3: %f\r\n", V_WB3);
    dprintf("4: %f\r\n", V_WB4);
    dprintf("5: %f\r\n", V_WB5);
    dprintf("6: %f\r\n", V_WB6);

    for (i = 0; i < WHEEL_BUTTON_MAX; i++)
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
