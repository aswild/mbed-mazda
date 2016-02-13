/*
 * RN52.cpp
 * Class to control the RN-52 bluetooth module with Serial
 *
 * Allen Wild, 2016
 */

#include "mbed.h"
#include "RN52.h"
#include <string.h>

#define RESP_WAIT_TIME_MS 10
#define CMD_BUF_LEN 32

RN52::RN52(PinName tx, PinName rx, PinName _gpio9) : serial(tx, rx), gpio9(_gpio9)
{
    serial.baud(115200);
    serial.attach(this, &RN52::serial_isr, Serial::RxIrq);
    gpio9 = 0; // don't need serial data, so leave chip in command mode
    
    memset(last_message, 0, RN52_MBUF_LEN);
    msg_len = 0;
}

void RN52::serial_isr()
{
    char c = serial.getc();
    if (msg_len < RN52_MBUF_LEN)
        last_message[msg_len++] = c;
}

void RN52::send_command(const char *cmd)
{
    memset(last_message, 0, RN52_MBUF_LEN);
    serial.puts(cmd);
    wait_ms(RESP_WAIT_TIME_MS);
}

void RN52::set_cmd_str(const char* cmd, const char *str_value)
{
    char cmd_buf[CMD_BUF_LEN];
    sprintf(cmd_buf, "%s,%s\r", cmd, str_value);
    send_command(cmd_buf);
}

void RN52::set_cmd_hex(const char *cmd, int hex_value)
{
    char cmd_buf[CMD_BUF_LEN];
    sprintf(cmd_buf, "%s,%x\r", cmd, hex_value);
    send_command(cmd_buf);
}

void RN52::set_cmd_dec(const char *cmd, int dec_value)
{
    char cmd_buf[CMD_BUF_LEN];
    sprintf(cmd_buf, "%s,%d\r", cmd, dec_value);
    send_command(cmd_buf);
}

inline void RN52::set_audio_out(const char *audio_mode)
{
    set_cmd_str("S|", audio_mode);
}
