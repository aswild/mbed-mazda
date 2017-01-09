/*****************************************
 * Main.cpp                              *
 * Main program of mbed-mazda (duh...)   *
 *                                       *
 * Allen Wild, 2016, 2017                *
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

#define PIONEER_HOLD(x)     dprintf("PBUTTON %d\r\n", (x))
#define PIONEER_RELEASE()   dprintf("PRELEASE\r\n")
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
    dbg_serial.baud(9600);
#endif

    dprintf("\r\n\r\n############## MBED MAZDA ##############\r\n");
#ifdef ENABLE_RN52
    bluetooth.init();
    dprintf("Bluetooth init done\r\n");
    bluetooth.set_user_defaults();
    dprintf("Bluetooth defaults done\r\n");
#endif

    dprintf("Entering main loop\r\n");
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
        if ((button != last_button) && ((last_button == W_IDLE) || button == W_IDLE))
        {
            dprintf("Switched to button %s\r\n", INPUT_BUTTON_STR[button]);

#ifdef ENABLE_RN52
            if (bluetooth.is_connected)
            {
                // button mapping when bluetooth is connected
                switch (button)
                {
                    case W_VOLUP:
                        PIONEER_HOLD(P_VOLUP);
                        break;
                    case W_VOLDOWN:
                        PIONEER_HOLD(P_VOLDOWN);
                        break;
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
                        break;
#ifdef TARGET_LPC1768
                    case W_STATUSCHECK:
                        bluetooth.check_status();
                        led1 = bluetooth.is_connected;
                        break;
#endif
                    case W_IDLE:
                    default:
                        PIONEER_RELEASE();
                        break;
                }
            }
            else
#endif
            {
                switch (button)
                {
                    // button mapping when bluetooth isn't connected
                    case W_VOLUP:
                        PIONEER_HOLD(P_VOLUP);
                        break;
                    case W_VOLDOWN:
                        PIONEER_HOLD(P_VOLDOWN);
                        break;
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
                        break;
#if defined(TARGET_LPC1768) && defined(ENABLE_RN52)
                    case W_STATUSCHECK:
                        bluetooth.check_status();
                        led1 = bluetooth.is_connected;
                        break;
#endif
                    case W_IDLE:
                    default:
                        PIONEER_RELEASE();
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
