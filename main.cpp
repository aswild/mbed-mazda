/*****************************************
 * Main.cpp                              *
 * Main program of mbed-mazda (duh...)   *
 *                                       *
 * Allen Wild, 2016                      *
 *****************************************/

#include "mbed.h"
#include "pin_names.h"
#include "Input.h"
#include "PioneerRemote.h"
#include "RN52.h"

/***************** Debugging stuff ********************************/
#ifdef SERIAL_DEBUG
Serial dbg_serial(PIN_SERIAL_TX, PIN_SERIAL_RX);
#define dprintf(format, ...) dbg_serial.printf(format, ##__VA_ARGS__)

#else // SERIAL_DEBUG disabled - use do-nothing macros
#define dprintf(...) {}
#define WB_str(x) {}
#endif

/************************ Main Loop ticker *************************/
#define LOOP_TIME_MS    80
#define LOOP_TIME_US    (LOOP_TIME_MS * 1000)
Ticker loop_ticker;
volatile bool main_loop_continue = true;
void loop_ticker_isr()
{
    // the ticker ISR sets this flag to tell the main loop to continue
    // after THIS interrupt.  Keeps the main loop asleep if the CPU is woken
    // up by other interrupts
    main_loop_continue = true;
}

/*********************** LPC1114 settings *************************/
#ifdef TARGET_LPC1114
PioneerRemote pioneer(PIN_SPI_MOSI, PIN_SPI_MISO, PIN_SPI_SCK, PIN_POT_CS);
#define PIONEER_HOLD(x)   pioneer.hold_button(x)
#define PIONEER_RELEASE() pioneer.release_button()

/*********************** LPC1768 settings *************************/
#elif defined(TARGET_LPC1768)
DigitalOut led1(LED1);
DigitalOut led2(LED2);

#define PIONEER_HOLD(x)     dprintf("PBUTTON %d\n", (x))
#define PIONEER_RELEASE()   dprintf("PRELEASE\n")
#endif

/***************** Common Definitions ******************************/
#ifdef ENABLE_RN52
RN52 bluetooth(PIN_RN52_TX, PIN_RN52_RX, PIN_RN52_IO9, PIN_RN52_IO2);
#endif

int main()
{
    input_button_t button;
    input_button_t last_button = W_IDLE;

#if defined(TARGET_LPC1114) && defined(SERIAL_DEBUG)
    int i;
    for (i = 0; i <= NUM_WHEEL_BUTTONS-1; i++)
        dprintf("Threshold %d: 0x%04X\r\n", i, WHEEL_BUTTON_THRESHOLD[i]);
#endif
#if defined(TARGET_LPC1768) && defined(SERIAL_DEBUG)
    dbg_serial.baud(115200);
#endif

    dprintf("\n\n############## MBED MAZDA ##############\n");
#ifdef ENABLE_RN52
    bluetooth.init();
    dprintf("Bluetooth init done\n");
    bluetooth.set_user_defaults();
    dprintf("Bluetooth defaults done\n");
#endif

    dprintf("Entering main loop\n");
    loop_ticker.attach_us(&loop_ticker_isr, LOOP_TIME_US);
    for (;;)
    {
        if (!main_loop_continue)
            goto _main_sleep;

#ifdef ENABLE_RN52
        if (bluetooth.status_update)
        {
            bluetooth.check_status();
#ifdef TARGET_LPC1768
            led1 = bluetooth.is_connected;
#endif
        }
#endif

        button = read_input_button();
        if (button != last_button)
        {
            dprintf("Switched to button %s\r\n", INPUT_BUTTON_STR[button]);
            if (button == W_VOLDOWN)
            {
                PIONEER_HOLD(P_VOLDOWN);
            }
            else if (button == W_VOLUP)
            {
                PIONEER_HOLD(P_VOLUP);
            }
            else if (button == W_IDLE)
            {
                PIONEER_RELEASE();
            }
#ifdef ENABLE_RN52
#ifdef TARGET_LPC1768
            else if (button == W_STATUSCHECK)
            {
                bluetooth.check_status();
                led1 = bluetooth.is_connected;
            }
#endif // TARGET_LPC1768
            else if (bluetooth.is_connected)
            {
                switch (button)
                {
                    case W_NEXT:
                        bluetooth.next_track();
                        break;
                    case W_PREV:
                        bluetooth.prev_track();
                        break;
                    case W_MODE:
                        bluetooth.voice_command();
                        break;
                    case W_MUTE:
                        bluetooth.play_pause();
                    default:
                        break;
                }
            }
#endif // ENABLE_RN52
            else
            {
                switch (button)
                {
                    case W_NEXT:
                        PIONEER_HOLD(P_NEXT);
                        break;
                    case W_PREV:
                        PIONEER_HOLD(P_PREV);
                        break;
                    case W_MODE:
                        PIONEER_HOLD(P_SOURCE);
                        break;
                    case W_MUTE:
                        PIONEER_HOLD(P_MUTE);
                    default:
                        break;
                }
            }
            last_button = button;
        }
        main_loop_continue = false; // Ticker ISR will reset this
_main_sleep:
        sleep();
    }
}
