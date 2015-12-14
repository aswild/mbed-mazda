#include "MCP41XXX.h"
#include "math.h"

MCP41XXX::MCP41XXX(PinName spi_mosi, PinName spi_miso, PinName spi_sck, PinName _cs,
                   bool _inverted, float _r_min, float _r_max) : 
    spi(spi_mosi, spi_miso, spi_sck), cs(_cs), r_min(_r_min), r_max(_r_max), inverted(_inverted)
{
    spi.format(16, 0);
    cs = 1;

    if (inverted)
    {
        slope = 255.0 / (r_min - r_max);
        offset = -1.0 * slope * r_max;
    }
    else
    {
        slope = 255.0 / (r_max - r_min);
        offset = -1.0 * slope * r_min;
    }
}

void MCP41XXX::set_wiper(uint8_t value)
{
    uint16_t cmd = CMD_SETWIPER | value;
    write_command(cmd);
}

uint8_t MCP41XXX::set_resistance(int r)
{
    uint8_t value = 0;

    if ((r < r_min && !inverted) || (r > r_max && inverted))
        value = 0;
    else if ((r < r_min && inverted) || (r > r_max && !inverted))
        value = 255;
    else
        value = round((slope * r) + offset);

    set_wiper(value);
    return value;
}

void MCP41XXX::shutdown()
{
    write_command(CMD_SHUTDOWN);
}

void MCP41XXX::write_command(uint16_t cmd)
{
    cs = 0;
    spi.write(cmd);
    cs = 1;
}
