/********************************************************************
 * Module to control Pioneer head unit via wired remote on MCP41XXX *
 ********************************************************************/

#ifndef __PIONEER_REMOTE_H__
#define __PIONEER_REMOTE_H__

#include "mbed.h"
#include "MCP41XXX.h"

#define DEFAULT_PRESS_LENGTH    100

enum pioneer_button_t
{
    P_NOP = 0,
    P_SOURCE,
    P_MUTE,
    P_DISP,
    P_NEXT,
    P_PREV,
    P_VOLUP,
    P_VOLDOWN,
    P_SELECT,  // might be a NOP
    P_BANDESC
};

// wiper values for MCP41050 inverted, must be in the same order as BUTTON_FUNCTION
const uint8_t BUTTON_VALUES[] = {
    92,     // NOP
    250,    // SOURCE
    239,    // MUTE
    229,    // DISP
    219,    // NEXT
    204,    // PREV
    181,    // VOL UP
    142,    // VOL DOWN
    92,     // Select (nop?)
    30      // BAND/ESC
};

class PioneerRemote
{
    public:
        PioneerRemote(int press_length_ms=DEFAULT_PRESS_LENGTH);
        void set_press_length(int press_length_ms);

        void press_button(pioneer_button_t);
        void hold_button(pioneer_button_t);
        void release_button();

    private:
        MCP41XXX pot;
        Timeout timeout;
        int press_length_us;
        bool pressed;
};

#endif
