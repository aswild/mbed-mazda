/*
 * RN52.cpp
 * Class to control the RN-52 bluetooth module with Serial
 *
 * Allen Wild, 2016
 */

#include "mbed.h"
#include "RN52.h"
#include <string.h>

#define CMD_BUF_LEN     16
#define REBOOT_WAIT_MS  2000

#define RESET_RESPONSE()                                \
    do {                                                \
        memset((char*)response_buf, 0, RN52_RBUF_LEN);  \
        response_len = 0;                               \
        response_ready = false;                         \
    } while (0)

#ifdef SERIAL_DEBUG
extern Serial dbg_serial;
#define dprintf(format, ...) dbg_serial.printf(format, ##__VA_ARGS__)
#else
//static void dprintf(...) {}
#define dprintf(...) {}
#endif

// use printf for now since the serial library includes it anyway
// Will need to disable this if I have to make a TinySerial library
#define RN52_USE_PRINTF

/****************** Public Functions **************************/
RN52::RN52(PinName tx, PinName rx, PinName _gpio9, PinName _gpio2) : serial(tx, rx), gpio9(_gpio9), gpio2(_gpio2)
{
    dprintf("RN52 constructor\n");
    serial.baud(115200);
    gpio9 = 1;
    
    status_update = false;
    RESET_RESPONSE();
}

void RN52::init()
{
    // enter CMD mode
    gpio9 = 0;
    // flush serial
    wait_ms(100);
    while (serial.readable())
    {
        serial.getc();
        wait_ms(1);
    }
    serial.attach(this, &RN52::serial_rx_isr, Serial::RxIrq);

    RESET_RESPONSE();
    check_status();
    gpio2.mode(PullNone);
    gpio2.fall(this, &RN52::io2_event_isr);
}

void RN52::set_user_defaults()
{
    bool need_reboot = false;

    send_command("S^,0\r");     // disable auto shutdown

    // audio routing - analog out 44.1k 24-bit
    if (!verify_set('|', RN52_DEFAULT_AROUTE))
        need_reboot = true;

    // extended features
    if (!verify_set('%', RN52_DEFAULT_EXSET))
        need_reboot = true;

    // SSP keyboard authentication (module will auto-accept, just click yes on phone)
    if (!verify_set('A', "1"))
        need_reboot = true;

    if (need_reboot)
    {
        dprintf("user defaults changed, rebooting\n");
        reboot();
    }
}

void RN52::set_factory_defaults()
{
    send_command("SF,1\r");
    wait_ms(20);
    send_command("R,1\r");
    wait_ms(500);
}

void RN52::check_status()
{
    long status;
    send_command("Q\r");
    dprintf("check_status: got Q=%s\n", (char*)response_buf);
    status = strtol((char*)response_buf, NULL, 16);
    is_connected = (status & 0x0400) != 0;
    status_update = false;
}

// {{{ simple stuff
void RN52::next_track()
{
    send_command("AT+\r");
}
void RN52::prev_track()
{
    send_command("AT-\r");
}
void RN52::play_pause()
{
    send_command("AP\r");
}
void RN52::voice_command()
{
    send_command("P\r");
}
void RN52::reboot()
{
    send_command("R,1\r", true);
    wait_ms(REBOOT_WAIT_MS);

    // flush serial
    serial.attach(NULL);
    while (serial.readable())
    {
        serial.getc();
        wait_ms(1);
    }
    serial.attach(this, &RN52::serial_rx_isr, Serial::RxIrq);
}
// }}}

void RN52::send_command(const char *cmd, bool async)
{
    RESET_RESPONSE();
    serial.puts(cmd);
    if (!async)
        response_wait();
}

/************************* Protected Functions ************************************/

// serial interrupt handler, called when we get an RX character
// Fills up response_buf until a \n is hit or the max size is hit, then
// set response_ready to true
void RN52::serial_rx_isr()
{
    char c = serial.getc();
    //dprintf("serial_rx_isr got char '%c'\n", c);
    
    if (c == '\r')
    {
        return; // discard \r, because it will be followed by a \n
    }
    else if (c == '\n')
    {
        response_ready = true;
        return;
    }

    if (response_ready)
    {
        // start of a new response, so reset the buffer
        RESET_RESPONSE();
    }

    if (response_len < (RN52_RBUF_LEN-1)) // -1 to save room for nullbyte
        response_buf[response_len++] = c;
    else
        response_ready = true;
}

void RN52::io2_event_isr()
{
    //dbg_serial.puts("io2_event_isr hit\n");
    status_update = true;
}

void RN52::response_wait(timestamp_t timeout_us)
{
    if (timeout_us)
    {
        response_timeout.attach_us(this, &RN52::response_timeout_isr, timeout_us);
    }
    while (!response_ready)
        sleep();
    response_timeout.detach();
    response_ready = false;
}
void RN52::response_timeout_isr()
{
    //dbg_serial.puts("response_timeout_isr hit\n");
    if (response_len < (RN52_RBUF_LEN-1))
        response_buf[response_len++] = '\0';
    response_ready = true;
}

// cmd should be the single-character character command (e.g. 'A' for SA,... command)
// the value is gotten first with G<cmd>, and then checked against value
// if value matches, return true
// if value doesn't match, set it and return false
bool RN52::verify_set(char cmd, const char *value)
{
    char cmd_buf[CMD_BUF_LEN];

#ifdef RN52_USE_PRINTF
    sprintf(cmd_buf, "G%c\r", cmd);
#else
    memset(cmd_buf, 0, sizeof(cmd_buf));
    cmd_buf[0] = 'G';
    cmd_buf[1] = cmd;
    cmd_buf[2] = '\r';
#endif

    //dprintf("verify_set, sending '%s'\n", cmd_buf);
    send_command(cmd_buf);

    dprintf("verify_set(%c), got response '%s'\n", cmd, response_buf);
    if (strcmp((char*)response_buf, value) == 0)
        return true;

    // value didn't match, so set it
#ifdef RN52_USE_PRINTF
    sprintf(cmd_buf, "S%c,%s\r", cmd, value);
#else
    // (manually, to avoid sprintf)
    cmd_buf[0] = 'S';
    cmd_buf[1] = cmd;
    cmd_buf[2] = ',';
    cmd_buf[3] = '\0';
    strcat(cmd_buf, value);
    strcat(cmd_buf, "\r");
#endif
    //dprintf("verify_set: sending command '%s'\n", cmd_buf);
    send_command(cmd_buf);
    return false;
}
