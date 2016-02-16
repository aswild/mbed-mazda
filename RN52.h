/*
 * RN52.h - header file for RN52 class to handle serial communication
 *
 * Allen Wild, 2016
 */

#ifndef __RN52_H__
#define __RN52_H__

// default options used in init_defaults
#define RN52_DEFAULT_AROUTE "0002"

// extended features = Auto-accept passkey, IO2 event latch, Disable system tones,
// mute volume tones, discoverable @ startup, and reconnect @ power on
#define RN52_DEFAULT_EXSET  "48A6"
//#define RN52_DEFAULT_EXSET  "40A6"

#define RN52_RBUF_LEN 256
#define RN52_DEFAULT_RESP_TIMEOUT_US 1000000 // one second default timeout

class RN52
{
    public:
        RN52(PinName tx, PinName rx, PinName gpio9, PinName gpio2);

        // serial message buffer, volatile because ISR will change them
        volatile char response_buf[RN52_RBUF_LEN];
        volatile int  response_len;
        volatile bool response_ready;

        volatile bool is_connected;
        volatile bool is_streaming;
        volatile bool status_update;

        void init();
        void set_user_defaults();
        void set_factory_defaults();
        void check_status();

        // print current config to mbed Serial interface
        //void print_config(Stream *stream_out);

        inline void next_track()
        {
            send_command("AT+\r");
        }
        inline void prev_track()
        {
            send_command("AT-\r");
        }
        inline void play_pause()
        {
            send_command("AP\r");
        }
        inline void voice_command()
        {
            send_command("P\r");
        }

        void reconnect();
        void disconnect(uint8_t profiles=0xFF);
        void reboot();

        /**
         * send a command and return the response.
         * Commands should be terminated with \r
         */
        void send_command(const char *cmd, bool async=false);

    protected:
        Serial serial;
        DigitalOut gpio9;
        InterruptIn gpio2;
        Timeout response_timeout;

        void serial_rx_isr();
        void io2_event_isr();
        bool verify_set(char cmd, const char *value);

        void response_timeout_isr();
        void response_wait(timestamp_t timeout_us=RN52_DEFAULT_RESP_TIMEOUT_US);
};

#endif
