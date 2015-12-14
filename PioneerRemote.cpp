/********************************************************************
 * Module to control Pioneer head unit via wired remote on MCP41XXX *
 ********************************************************************/

#include "PioneerRemote.h"
#include "pin_names.h"

PioneerRemote::PioneerRemote(int press_length_ms) : pot(PIN_SPI_MOSI, PIN_SPI_MISO, PIN_SPI_SCK, PIN_POT_CS, true)
{
    pot.shutdown();
    press_length_us = press_length_ms * 1000;
    pressed = false;
}

void PioneerRemote::set_press_length(int press_length_ms)
{
    press_length_us = press_length_ms * 1000;
}

void PioneerRemote::press_button(pioneer_button_t function)
{
    if (pressed)
        timeout.detach();
    pot.set_wiper(BUTTON_VALUES[function]);
    pressed = true;
    timeout.attach_us(this, &PioneerRemote::release_button, press_length_us);
}

void PioneerRemote::hold_button(pioneer_button_t function)
{
    if (pressed)
        timeout.detach();
    pot.set_wiper(BUTTON_VALUES[function]);
    pressed = true;
}

void PioneerRemote::release_button()
{
    pot.shutdown();
    pressed = false;
}
