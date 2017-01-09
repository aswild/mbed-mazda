#ifndef MBED_ISP__ISP__
#define MBED_ISP__ISP__


int     isp_flash_write( const char *file_name );
void    start_target_in_normal_mode( int baud_rate );
void    start_success_indicator( void );
void    toggle_led( char v );
void    leds_off( void );
void    show_progress( int total_size, int file_size );


enum {
    NO_ERROR                            = 0x00,

    ERROR_AT_TARGET_OPEN,
    ERROR_DATA_SIZE_ZERO,

    ERROR_AT_FILE_OPEN,
    ERROR_AT_SECTOR_ERASE,

    ERROR_AT_MALLOC_FOR_WRITE_BUFF,
    ERROR_AT_WRITE_COMMAND,
    ERROR_AT_PREPARE_COMMAND,
    ERROR_AT_COPY_COMMAND,
    ERROR_AT_SENDING_CHECKSUM,

    ERROR_AT_MALLOC_FOR_VERIFY_FILE_BUFF,
    ERROR_AT_MALLOC_FOR_VERIFY_DATA_BUFF,
    ERROR_AT_READ_COMMAND,

    ERROR_DATA_DOES_NOT_MATCH,

    WARNING_CRP_CODE_DETECTED
};


enum {
    NO_ISP  = 0x4E697370,
    CRP1    = 0x12345678,
    CRP2    = 0x87654321,
    CRP3    = 0x43218765,
    CRP_CHECK_ERROR    = 0xFFFFFFFF
};


#endif  //  MBED_ISP__ISP__

