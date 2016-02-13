/*
 * RN52.h - header file for RN52 class to handle serial communication
 *
 * Allen Wild, 2016
 */

#ifndef __RN52_H__
#define __RN52_H__

/*********************************
 * defines for command constants *
 *********************************/

// S|, audio output routing and format
#define AUDIO_OUT_AU_ANALOG "00" // default
#define AUDIO_OUT_AU_I2S    "01"
#define AUDIO_OUT_AU_SPDIF  "02"
#define AUDIO_OUT_AU_DAC    "03"
#define AUDIO_OUT_W_24      "0"  // default
#define AUDIO_OUT_W_32      "1"
#define AUDIO_OUT_R_8K      "0"
#define AUDIO_OUT_R_16K     "1"
#define AUDIO_OUT_R_44K1    "2"  // default
#define AUDIO_OUT_R_48K     "3"

// S%, extended features, all disabled by default
#define EX_AVRCP_EK                 (1<<0)  // Enable AVRCP buttons for EK
#define EX_PON_RECONNECT            (1<<1)  // Enable reconnect on power-on
#define EX_PON_DISCOVERABLE         (1<<2)  // Discoverable on start up
#define EX_CODEC_INDICATORS         (1<<3)  // Codec indicators PIO7 (AAC) and PIO6 (aptX)
#define EX_REBOOT_AFTER_DISC        (1<<4)  // Reboot after disconnect
#define EX_MUTE_VOL_TONES           (1<<5)  // Mute volume up/down tones
#define EX_PIO4_VOICE_BTN           (1<<6)  // Enable voice command button on PIO4
#define EX_DISABLE_SYS_TONES        (1<<7)  // Disable system tones
#define EX_PWROFF_AFTER_TIMEOUT     (1<<8)  // Power off after pairing timeout
#define EX_RESET_AFTER_PWROFF       (1<<9)  // Reset after power off
#define EX_RECONNECT_AFTER_PANIC    (1<<10) // Enable list reconnect after panic
#define EX_LATCH_INDICATE_PIO2      (1<<11) // Enable latch event indicator PIO2
#define EX_TRACK_CHANGE_EVENT       (1<<12) // Enable track change event
#define EX_TONES_FIXED_VOLUME       (1<<13) // Enable tones playback at fixed volume
#define EX_AUTO_ACCEPT_PASSKEY      (1<<14) // Enable auto-accept passkey in Keyboard I/O Authentication mode

// SA, bluetooth authentication mode
#define AUTH_MODE_OPEN      "0"
#define AUTH_MODE_SSP_KEY   "1" // default
#define AUTH_MODE_JUSTWORKS "2"
#define AUTH_MODE_LEGACY    "4"

// SD, and SK, profile discovery/connection mask. All default on
#define PROFILE_IAP     (1<<0)
#define PROFILE_SPP     (1<<1)
#define PROFILE_A2DP    (1<<2)
#define PROFILE_HFP     (1<<3)

/***********************
 * Other defines/enums *
 ***********************/

// status enum, with integer representations of
// the bytes for optimized comparisons
enum RN52_STATUS
{
    AOK = 0x00414F4B,
    ERR = 0x00455252
};

#define RN52_MBUF_LEN 256

class RN52
{
    public:

        /**
         * The message associated with the last command issued, stored
         * as a null-terminated string
         */
        char last_message[RN52_MBUF_LEN];

        /**
         * Current length (end position) of the last command's response buffer
         */
        int msg_len;

        /**
         * RN52 class constructor, takes in pin names to describe the connection
         * tx - serial tx
         * rx - serial rx
         * gpio9 - mbed pin which is connected to RN-52 GPIO9, used for switching between command and data mode
         */
        RN52(PinName tx, PinName rx, PinName gpio9);

        /*****************
         * Set Functions *
         *****************/

        /**
         * Set the audio output routing.
         * Specify a concatenation of AUDIO_OUT_AU_x AUDIO_OUT_W_y AUDIO_OUT_R_z
         */
        void set_audio_out(const char *audio_mode);

        /**
         * Set extended features using S%
         * argument should be a set of EX_xxx macros or'd together
         */
        void set_ex_features(uint16_t features);

        void set_auth_mode(const char *auth_mode);

        /**
         * Set bluetooth service class. Use only if needed for compatibility..
         */
        void set_service_class(uint32_t service_class);

        void set_discovery_mask(uint8_t discovery_mask);

        void set_connection_mask(uint8_t connection_mask);

        void set_audioin_gain(uint16_t gain_a, uint16_t gain_b);

        void set_device_name(const char *name);

        void set_auth_pin(const char *pin);

        void set_speaker_gain(uint8_t gain);

        void set_tone_gain(uint8_t gain);

        void set_connection_retry_delay(int delay);

        void set_pairing_timeout(int timeout);

        /*************************
         * Get Command Functions *
         *************************/

        // print current config to mbed Serial interface
        void print_config(Serial *ser_out);

        // copy up to n characters of current config to buffer, nullbyte terminated
        // (nullbyte included in n)
        // returns number of characters read (excluding nullbyte)
        int dump_config(char *buf, int buf_len);

        /********************
         * Action Functions *
         ********************/

        // reset settings (SF, technically classified as a set command in RN52 command guide)
        RN52_STATUS set_factory_defaults();

        void volume_up();
        void volume_down();

        void next_track();
        void prev_track();
        void play_pause();

        void reconnect();
        void disconnect(uint8_t profiles=0xFF);

        /*********************
         * Utility Functions *
         *********************/

        /**
         * send a command and return the response.
         * Commands should be terminated with \r
         */
        void send_command(const char *cmd);

    protected:
        Serial serial;
        DigitalOut gpio9;

        void serial_isr();
        void set_cmd_str(const char *cmd, const char *str_value);
        void set_cmd_hex(const char *cmd, int hex_value);
        void set_cmd_dec(const char *cmd, int dec_value);
};

#endif
