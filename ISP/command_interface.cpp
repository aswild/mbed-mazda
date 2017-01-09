#include    "mbed.h"
#include    "command_interface.h"
#include    "serial_utilities.h"
#include    "_user_settings.h"


int try_and_check( char *command, char *expected_return_str )
{
    char    rtn_str[ STR_BUFF_SIZE ];
    int     result  = 1;

    print_command( command );
    put_string( command );

    get_string( rtn_str );
    print_result( result  = strcmp( expected_return_str, rtn_str ) );

//    if ( result && !mode )
//        error( "command failed\r\n" );

    return ( result );
}


int try_and_check2( char *command, char *expected_return_str )
{
    char    rtn_str[ STR_BUFF_SIZE ];
    int     result  = 1;

    print_command( command );
    put_string( command );

    get_string( rtn_str );  // just readout echoback
    get_string( rtn_str );
    print_result( result  = strcmp( expected_return_str, rtn_str ) );

//    if ( result && !mode )
//        error( "command failed\r\n" );

    return ( result );
}


void print_command( char *command )
{
#ifndef SUPPRESS_COMMAND_RESULT_MESSAGE

    char    s[ STR_BUFF_SIZE ];
    char    *pos;

    strcpy( s, command );

    if ( pos    = strchr( s, '\r' ) )
        *pos    = '\0';

    if ( pos    = strchr( s, '\n' ) )
        *pos    = '\0';

    printf( "  command - %-22s : ", s );

#endif  //SUPPRESS_COMMAND_RESULT_MESSAGE
}


void print_result( int r )
{
#ifndef SUPPRESS_COMMAND_RESULT_MESSAGE
    printf( "%s\r\n", r ? "Fail" : "Pass" );
#endif  //SUPPRESS_COMMAND_RESULT_MESSAGE
}


int send_RAM_transfer_checksum( int checksum )
{
    char    command[ 16 ];

    sprintf( command, "%d\n", checksum );

    return ( try_and_check( command, "OK" ) );
}


int erase_sectors( int last_sector )
{
    char    command_str[ STR_BUFF_SIZE ];

    sprintf( command_str, "P 0 %d\r\n", last_sector );
    if ( try_and_check( command_str, "0" ) )
        return ( 1 );

    *(command_str)  = 'E';
    return ( try_and_check( command_str, "0" ) );
}


