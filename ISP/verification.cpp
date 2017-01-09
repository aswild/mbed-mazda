#include    "mbed.h"
#include    "verification.h"
#include    "command_interface.h"
#include    "uu_coding.h"
#include    "serial_utilities.h"
#include    "writing.h"
#include    "isp.h"
#include    "_user_settings.h"



int     verify_binary_data( FILE *fp, int *transferred_size_p, int file_size );
int     verify_uucoded_data( FILE *fp, int *transferred_size_p, int file_size );
void    get_binary_from_uucode_str( char *b, int size );


int verify_flash( FILE *fp, target_param *tpp, int *transferred_size_p, int file_size )
{
    if ( tpp->write_type == BINARY )
        return ( verify_binary_data( fp, transferred_size_p, file_size ) );
    else
        return ( verify_uucoded_data( fp, transferred_size_p, file_size ) );
}


int verify_binary_data( FILE *fp, int *read_size_p, int file_size )
{
    char    command_str[ STR_BUFF_SIZE ];
    int     read_size   = 0;
    int     size;
    int     flash_reading_size;
    char    *bf;
    char    *br;
    int     error_flag  = 0;
    unsigned long   checksum        = 0;
    unsigned long   checksum_count  = 0;


    fseek( fp, 0, SEEK_SET ); // seek back to beginning of file

    flash_reading_size  = 128;

    if ( NULL == (bf    = (char *)malloc( flash_reading_size * sizeof( char ) )) )
        return( ERROR_AT_MALLOC_FOR_VERIFY_FILE_BUFF );

    if ( NULL == (br    = (char *)malloc( flash_reading_size * sizeof( char ) )) )
        return( ERROR_AT_MALLOC_FOR_VERIFY_DATA_BUFF );


    while ( size    = fread( bf, sizeof( char ), flash_reading_size, fp ) ) {

        if ( read_size < 0x20 ) {
            for ( int i = 0; i < flash_reading_size; i += 4 ) {

                if ( checksum_count == 7 ) {
                    checksum    = 0xFFFFFFFF - checksum + 1;
                    *((unsigned int *)(bf + i)) = checksum;
                    //printf( "\r\n\r\n  -- calculated checksum    : 0x%08X\r\n", checksum );
                } else {
                    checksum    += *((unsigned int *)(bf + i));
                }

                checksum_count++;
            }
        }


        sprintf( command_str, "R %ld %ld\r\n", read_size, (size + 3) & ~0x3 );  //  reading size must be 4*N
        if ( try_and_check( command_str, "0" ) )
            return ( ERROR_AT_READ_COMMAND );

        get_binary( br, 1 );
        get_binary( br, size );

        for ( int i = 0; i < size; i++ ) {
//            printf( "   %s 0x%02X --- 0x%02X\r\n", (*(bf + i) != *(br + i)) ? "***" : "   ", *(bf + i), *(br + i) );
            if ( (*(bf + i) != *(br + i)) ) {
//                printf( "   %s 0x%02X --- 0x%02X\r\n", (*(bf + i) != *(br + i)) ? "***" : "   ", *(bf + i), *(br + i) );
                error_flag++;
            }
        }

        if ( error_flag )
            break;

        read_size   += size;

#ifdef ENABLE_PROGRESS_DISPLAY
        show_progress( read_size, file_size );
#endif
    }

    free( bf );
    free( br );

    *read_size_p    = read_size;

    return ( error_flag ? ERROR_DATA_DOES_NOT_MATCH : NO_ERROR );
}


#define     LINE_BYTES  44
#define     N_OF_LINES   4
#define     READ_SIZE   (LINE_BYTES * N_OF_LINES)


int verify_uucoded_data( FILE *fp, int *read_size_p, int file_size )
{
    char    command_str[ STR_BUFF_SIZE ];
    int     read_size   = 0;
    int     size;
    int     flash_reading_size;
    char    *bf;
    char    *br;
    int     error_flag  = 0;

    flash_reading_size  = READ_SIZE;

    initialize_uud_table();

    if ( NULL == (bf    = (char *)malloc( flash_reading_size * sizeof( char ) )) )
        error( "malloc error happened (in verify process, file data buffer)\r\n" );

    if ( NULL == (br    = (char *)malloc( flash_reading_size * sizeof( char ) )) )
        error( "malloc error happened (in verify process, read data buffer)\r\n" );

    fseek( fp, 0, SEEK_SET ); // seek back to beginning of file

    while ( size    = fread( bf, sizeof( char ), flash_reading_size, fp ) ) {

        if ( !read_size ) {
            //  overwriting 4 bytes data for address=0x1C
            //  there is a slot for checksum that is checked in (target's) boot process
            add_isp_checksum( bf );
        }

        sprintf( command_str, "R %ld %ld\r\n", read_size, (size + 3) & ~0x3 );  //  reading size must be 4*N
        if ( try_and_check( command_str, "0" ) )
            return ( ERROR_AT_READ_COMMAND );

        get_binary_from_uucode_str( br, size );

        for ( int i = 0; i < size; i++ ) {
//            printf( "   %s 0x%02X --- 0x%02X\r\n", (*(bf + i) != *(br + i)) ? "***" : "   ", *(bf + i), *(br + i) );
            if ( (*(bf + i) != *(br + i)) ) {
//                printf( "   %s 0x%02X --- 0x%02X\r\n", (*(bf + i) != *(br + i)) ? "***" : "   ", *(bf + i), *(br + i) );
                error_flag++;
            }
        }

        if ( error_flag )
            break;

        read_size   += size;

#ifdef ENABLE_PROGRESS_DISPLAY
        show_progress( read_size, file_size );
#endif
    }

    free( bf );
    free( br );

    *read_size_p    = read_size;

    return ( error_flag ? ERROR_DATA_DOES_NOT_MATCH : NO_ERROR );
}


void get_binary_from_uucode_str( char *b, int size )
{
    char    s[ N_OF_LINES ][ STR_BUFF_SIZE ];
    char    ss[ STR_BUFF_SIZE ];
    long    checksum    = 0;
    int     line_count  = 0;
    int     read_size   = 0;
    int     retry_count = 3;

    size    = (size + 3) & ~0x3;

    while ( retry_count-- ) {

        for ( int i = 0; i < ((READ_SIZE < size) ? N_OF_LINES : ((size - 1) / LINE_BYTES) + 1) ; i++ )
            get_string( s[ i ] );

        get_string( ss );


        while ( size ) {
            read_size   = uudecode_a_line( b, s[ line_count ] );

            for ( int i = 0; i < read_size; i++ )
                checksum    += *b++;

            size    -= read_size;
            line_count++;
        }

//        printf( "  checksum -- %s (internal = %ld)\r\n", ss, checksum );

        if ( checksum == atol( ss ) ) {
            put_string( "OK\r\n" );
            return;
//            printf( "  checksum OK\r\n" );
        } else {
            printf( "  checksum RESEND\r\n" );
            put_string( "RESEND\r\n" );
        }
    }
}
