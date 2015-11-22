#include "mbed.h"
#include "MCP41XXX.h"

//#ifndef TARGET_LPC1768
//#error This program needs LPC1768 target!
//#endif

SPI spix(p5, p6, p7);
MCP41XXX dpot(&spix, p8, 426, 50000, true);

Serial pc(USBTX, USBRX);

char buf[64];

int main()
{
    //uint8_t value = 0;
    int value = 0;
    int ivalue = 0;
    char c;

    pc.printf("MCP41XXX Test\r\n");

    for (;;)
    {
        c = pc.getc();
        pc.putc(c);

#if 0
        if (c >= '0' && c <= '9')
            value = (value<<4) | (c-'0');
        else if (c >= 'a' && c <= 'f')
            value = (value<<4) | (c-'a'+10);
        else if (c >= 'A' && c <= 'F')
            value = (value<<4) | (c-'A'+10);
#else
        if (c >= '0' && c <= '9')
            value = (value*10) + (c-'0');
        else if (c == 'S')
        {
            pc.printf("\r\nSHUTDOWN\r\n");
            dpot.shutdown();
        }
        else if (c == '\r' || c == '\n')
        {
#if 0
            value &= 0xff;
            pc.printf("\r\nSET %03d\r\n", value);
            dpot.set_wiper(value);
            value = 0;
#else
            ivalue = dpot.set_resistance(value);
            pc.printf("\r\nSET %d Ohms (%d)\r\n", value, ivalue);
            value = 0;
#endif
        }
#endif
    }
}
