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

// MAZDA3 steering wheel controls, resistance when voltage-divided with a 500ohm pullup resistor
// Read analog with read_u16() which gives 0x0000-0xFFFF value. These thresholds are the lower limit
// the function is assumed to be the highest threshold it's greater than
#define WHEEL_BUTTON_MAX 6
const uint16_t WHEEL_BUTTON_THRESHOLD[] = {
    0x0000, // Vol Down
    0x2A4B, // Vol Up
    0x4DBD, // Next
    0x743E, // Prev
    0x9A59, // Mode
    0xBD41, // Mute
    0xDB46  // Idle
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
