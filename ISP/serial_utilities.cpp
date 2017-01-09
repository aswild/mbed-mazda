#include    "mbed.h"
#include    "serial_utilities.h"
#include    "isp.h"
#include    "_user_settings.h"

Serial          pc ( USBTX,USBRX );

#if 0
Serial          target( p28, p27 );
#else
#define MODSERIAL_DEFAULT_RX_BUFFER_SIZE 512
#define MODSERIAL_DEFAULT_TX_BUFFER_SIZE 512
#include "MODSERIAL.h"
MODSERIAL       target(SERIAL_TX_PIN, SERIAL_RX_PIN);   //
//MODSERIAL       target(  p9, p10 ); //    if you need to change the UART pin, change here
#endif

Timeout timeout;

int timeout_flag    = 0;


void set_target_baud_rate( int baud_rate )
{
    target.baud( baud_rate );
}


void usb_serial_bridge_operation( void )
{
    while (1) {

        if ( pc.readable() ) {
            target.putc( pc.getc() );
        }

        if ( target.readable() ) {
            pc.putc( target.getc() );
        }
    }
}


void put_string( const char *s )
{
    char            c;

    while ( (c = *s++) != '\0' ) {
        target.putc( c );
        toggle_led( 0 );
    }
}


void put_binary( char *b, int size )
{
    for ( int i = 0; i < size; i++ ) {
        target.putc( *b++ );
        toggle_led( 0 );
    }
}

void set_flag()
{
    timeout_flag    = 1;
}


void get_string( char *s )
{
    int     i   = 0;
    char    c   = 0;
    timeout_flag    = 0;

    timeout.attach( &set_flag, 1 );

    do {
        do {
            if ( target.readable() ) {
                c  = target.getc();

                if ( ( c == '\n') || (c == '\r') )
                    break;

                *s++    = c;
                i++;
                toggle_led( 1 );
            }

            if ( timeout_flag )
                return;
        } while ( 1 );
    } while ( !i );

    *s  = '\0';
}


int get_binary( char *b, int length )
{
    int i;

    timeout_flag    = 0;
    timeout.attach( &set_flag, 1 );

    for ( i = 0; i < length; ) {
        if ( target.readable() ) {
            *b++    = target.getc();
            i++;
            toggle_led( 1 );
        }

        if ( timeout_flag )
            return ( i );
    }

    return ( i );
}


char read_byte( void )
{
    while ( !target.readable() )
        ;

    return ( target.getc() );
}


