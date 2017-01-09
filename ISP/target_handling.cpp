#include    "mbed.h"
#include    "target_handling.h"
#include    "target_table.h"
#include    "command_interface.h"
#include    "serial_utilities.h"
#include    "_user_settings.h"


DigitalOut      reset_pin(RESET_PIN);
DigitalOut      isp_pin(ISP_ENABLE_PIN);


target_param *open_target( int baud_date )
{
    target_param    *tpp;
    char            str_buf0[ STR_BUFF_SIZE ];
    char            str_buf1[ STR_BUFF_SIZE ];
    int             retry_count = 3;

    set_target_baud_rate( baud_date );

    while ( retry_count-- ) {
        reset_target( ENTER_TO_ISP_MODE );

        if ( !try_and_check( "?", "Synchronized" ) )
            break;
    }

    if ( !retry_count )
        return ( NULL );

    try_and_check2( "Synchronized\r\n", "OK" );
    try_and_check2( "12000\r\n", "OK" );
    try_and_check2( "U 23130\r\n", "0" );
    try_and_check2( "A 0\r\n", "0" );

    try_and_check( "K\r\n", "0" );
    get_string( str_buf0 );
    get_string( str_buf1 );

#ifndef SUPPRESS_COMMAND_RESULT_MESSAGE
    printf( "    result of \"K\" = %s %s\r\n", str_buf0, str_buf1 );
#endif

    try_and_check( "J\r\n", "0" );
    get_string( str_buf0 );

#ifndef SUPPRESS_COMMAND_RESULT_MESSAGE
    printf( "    result of \"J\" = %s\r\n", str_buf0 );
#endif

    tpp  = find_target_param( str_buf0 );

    return ( tpp );
}


void reset_target( int isp_pin_state )
{
    reset_pin   = 1;
    isp_pin     = isp_pin_state;
    wait_ms( 100 );

    reset_pin   = 0;
    wait_ms( 100 );

    reset_pin   = 1;
    wait_ms( 100 );
}

