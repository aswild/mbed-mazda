
#include    "mbed.h"
#include    "writing.h"
#include    "command_interface.h"
#include    "uu_coding.h"
#include    "serial_utilities.h"
#include    "isp.h"
#include    "_user_settings.h"


int     write_uuencoded_data( FILE *fp, target_param *tpp, int *total_size_p, int file_size );
int     write_binary_data( FILE *fp, int ram_size, int sector_size, unsigned int ram_start, int *total_size_p, int file_size );
int     get_flash_writing_size( int ram_size, unsigned int ram_start );
void    show_progress( int total_size, int file_size );


int write_flash( FILE *fp, target_param *tpp, int *transferred_size_p, int file_size )
{
    if ( tpp->write_type == BINARY )
        return ( write_binary_data(  fp, tpp->ram_size, tpp->sector_size, tpp->ram_start_address, transferred_size_p, file_size ) );
    else // UUENCODE
        return ( write_uuencoded_data( fp, tpp, transferred_size_p, file_size ) );
}





int write_uuencoded_data( FILE *fp, target_param *tpp, int *total_size_p, int file_size )
{
#define     BYTES_PER_LINE      45

    char    command_str[ STR_BUFF_SIZE ];
    long    checksum        = 0;
    int     size;
    int     total_size      = 0;
    int     sector_number;

    int     flash_writing_size;
    int     lines_per_transfer;
    int     transfer_size;

    char    *b;

    int ram_size            = tpp->ram_size;
    unsigned int ram_start  = tpp->ram_start_address;

    initialize_uue_table();

    flash_writing_size  = get_flash_writing_size( ram_size, ram_start );
    lines_per_transfer  = ((flash_writing_size / BYTES_PER_LINE) + 1);
    transfer_size       = (((flash_writing_size + 11) / 12) * 12);

    //  char    b[ transfer_size ]; // this can be done in mbed-compiler. but I should do it in common way

    if ( NULL == (b = (char *)malloc( transfer_size * sizeof( char ) )) )
        return( ERROR_AT_MALLOC_FOR_WRITE_BUFF );

    for ( int i = flash_writing_size; i < transfer_size; i++ )
        b[ i ]  = 0;    //  this is not neccesary but just stuffing stuffing bytes

    while ( size    = fread( b, sizeof( char ), flash_writing_size, fp ) ) {

        if ( !total_size ) {
            //  overwriting 4 bytes data for address=0x1C
            //  there is a slot for checksum that is checked in (target's) boot process
            add_isp_checksum( b );
        }

        sprintf( command_str, "W %ld %ld\r\n", ram_start, transfer_size );
        if ( try_and_check( command_str, "0" ) )
            return ( ERROR_AT_WRITE_COMMAND );

        for ( int i = 0; i < lines_per_transfer; i++ ) {

            checksum   += bin2uue( b + (i * BYTES_PER_LINE), command_str, i == (lines_per_transfer - 1) ? (transfer_size % BYTES_PER_LINE) : BYTES_PER_LINE );

            //  printf( "  data -- %02d %s\r", i, command_str );

            put_string( command_str );

            if ( !((i + 1) % 20) ) {
                if ( send_RAM_transfer_checksum( checksum ) )
                    return ( ERROR_AT_SENDING_CHECKSUM );

                checksum   = 0;
            }
        }

        if ( send_RAM_transfer_checksum( checksum ) )
            return ( ERROR_AT_SENDING_CHECKSUM );

        checksum   = 0;

//        sprintf( command_str, "P %d %d\r\n", total_size / sector_size, total_size / sector_size );

        sector_number   = find_sector( total_size, tpp );
        sprintf( command_str, "P %d %d\r\n", sector_number, sector_number );

        if ( try_and_check( command_str, "0" ) )
            return ( ERROR_AT_PREPARE_COMMAND );

        sprintf( command_str, "C %d %d %d\r\n", total_size, ram_start, flash_writing_size );
        if ( try_and_check( command_str, "0" ) )
            return ( ERROR_AT_COPY_COMMAND );

        total_size  += size;

#ifdef ENABLE_PROGRESS_DISPLAY
        show_progress( total_size, file_size );
#endif
    }

    free( b );
    *total_size_p   = total_size;

    return ( NO_ERROR );
}


int write_binary_data( FILE *fp, int ram_size, int sector_size, unsigned int ram_start, int *total_size_p, int file_size )
{
    char    command_str[ STR_BUFF_SIZE ];
    int     total_size  = 0;
    int     size;
    int     flash_writing_size;
    char    *b;

    flash_writing_size  = 256;

    if ( NULL == (b     = (char *)malloc( flash_writing_size * sizeof( char ) )) )
        return( ERROR_AT_MALLOC_FOR_WRITE_BUFF );

    while ( size    = fread( b, sizeof( char ), flash_writing_size, fp ) ) {

        if ( !total_size ) {
            //  overwriting 4 bytes data for address=0x1C
            //  there is a slot for checksum that is checked in (target's) boot process
            add_isp_checksum( b );
        }

        sprintf( command_str, "W %ld %ld\r\n", ram_start, flash_writing_size );
        if ( try_and_check( command_str, "0" ) )
            return ( ERROR_AT_WRITE_COMMAND );

        put_binary( b, flash_writing_size );
        put_string( "\r\n" );

        sprintf( command_str, "P %d %d\r\n", total_size / sector_size, total_size / sector_size );
        if ( try_and_check( command_str, "0" ) )
            return ( ERROR_AT_PREPARE_COMMAND );

        sprintf( command_str, "C %d %d %d\r\n", total_size, ram_start, flash_writing_size );
        if ( try_and_check( command_str, "0" ) )
            return ( ERROR_AT_COPY_COMMAND );

        total_size  += size;
        //printf( "  total %d bytes transferred\r", total_size );

#ifdef ENABLE_PROGRESS_DISPLAY
        show_progress( total_size, file_size );
#endif
    }

    free( b );
    *total_size_p   = total_size;

    return ( NO_ERROR );
}


void add_isp_checksum( char *b )
{
    //  see http://www.lpcware.com/content/nxpfile/lpc177x8x-checksum-insertion-program

    unsigned int    *p;
    unsigned int    cksum   = 0;

    p  = (unsigned int *)b;

    for ( int i = 0; i < 7; i++ ) {
        cksum   += *p++;
    }

    printf( "  -- value at checksum slot : 0x%08X\r\n", *p );

    *p  = 0xFFFFFFFF - cksum + 1;
    printf( "  -- calculated checksum    : 0x%08X\r\n", *p );

    printf( "     new checksum will be used programing flash\r\n" );
}


int get_flash_writing_size( int ram_size, unsigned int ram_start )
{
    int flash_writing_size[]    = {
        4096,
        1024,
        512,
        256
    };
    int     available_size;
    int     i;

    available_size  = ram_size - (ram_start & 0xFFFF);

    for ( i = 0; i < sizeof( flash_writing_size ) / sizeof( int ); i++ ) {
        if ( flash_writing_size[ i ] < available_size )
            break;
    }

    return ( flash_writing_size[ i ] );
}


int post_writing_process( target_param *tpp )
{
    if ( tpp->write_type == UUENCODE )
        return ( try_and_check( "G 0 T\r\n", "0" ) );
    else
        return ( 0 );
}
