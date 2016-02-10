/***********************************************
 * Library for MCP41XXX Digital Potentiometers *
 ***********************************************/

#ifndef __MCP41XXX_H__
#define __MCP41XXX_H__

#include "mbed.h"

#define CMD_SETWIPER 0x1100
#define CMD_SHUTDOWN 0x2100

#define DEFAULT_R_MAX 50000
#define DEFAULT_R_MIN   426

class MCP41XXX
{
    public:
        MCP41XXX(PinName spi_mosi, PinName spi_miso, PinName spi_sck, PinName _cs,
                 bool _inverted=false, float _r_min=DEFAULT_R_MIN, float _r_max=DEFAULT_R_MAX);

        void set_wiper(uint8_t value);
        uint8_t set_resistance(int r);
        void shutdown();

    private:
        SPI spi;
        DigitalOut cs;
        float r_min, r_max;
        bool inverted;
        float slope, offset;

        void write_command(uint16_t cmd);
};

#endif
