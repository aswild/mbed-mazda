
#include    "mbed.h"
#include    "target_table.h"
#include    "serial_utilities.h"
#include    "command_interface.h"
#include    "writing.h"
#include    "uu_coding.h"
#include    "target_handling.h"
#include    "verification.h"
#include    "isp.h"
#include    "_user_settings.h"

#define     CRP_WORD_OFFSET     0x2FC


BusOut          leds( LED4, LED3, LED2, LED1 );
Ticker          success;

int             file_size( FILE *fp );
unsigned int    read_crp( FILE *fp );
unsigned int    crp_check( FILE *fp );
void            success_indicator();
void            user_action_waiting_indicator();


int isp_flash_write( const char *file_name )
{
    FILE            *fp;
    target_param    *tpp;
    int             data_size;
    int             last_sector;
    int             transferred_size;
    int             err;

    if ( NULL == (tpp = open_target( ISP_BAUD_RATE )) ) {
        return ( ERROR_AT_TARGET_OPEN );
    }

    printf( "  target device found : type       = \"%s\"\r\n",      tpp->type_name );
    printf( "                        ID         =  0x%08X\r\n",     tpp->id );
    printf( "                        RAM size   =  %10d bytes\r\n", tpp->ram_size );
    printf( "                        flash size =  %10d bytes\r\n", tpp->flash_size );

    //
    //  If user selected "flash erase only"
    //

    if ( *file_name == 0xFF ) {
        last_sector = find_sector( tpp->flash_size - 1, tpp );

        //  The file selector returns 0xFF as first character of the string
        //      if user selected "flash erase".

        printf( "\r\n  ==== flash erasing ====\r\n" );

        if ( erase_sectors( last_sector ) )
            return ( ERROR_AT_SECTOR_ERASE );

        printf( "  sectors from 0 to %d are erased. bye\r\n", last_sector );
        exit ( 0 ); //  quit from app
    }

    //
    //  Normal operation
    //

    printf( "  opening file: \"%s\"\r\n", file_name );

    if ( NULL == (fp    = fopen( file_name, "r" )) ) { // ALLEN - change "rb" to "r" because that makes shit work for whatever reason
        return ( ERROR_AT_FILE_OPEN );
    }

    data_size   = file_size( fp );

    if ( !data_size )
        return ( ERROR_DATA_SIZE_ZERO );

//  last_sector = (data_size - 1) / tpp->sector_size;
    last_sector = find_sector( data_size - 1, tpp );

    if ( data_size < (int)(CRP_WORD_OFFSET + sizeof( unsigned int )) ) {
        printf( "  CRP check is not performed because data size is less than 0x300(768) bytes\r\n" );
    } else {
        if ( crp_check( fp ) ) {
            printf( "  the CRP is enabled in the data source file\r\n" );

#ifdef CHECK_CRP_CODE
            printf( "  aborting execution by CRP warning\r\n" );

            return ( WARNING_CRP_CODE_DETECTED );
#else
            printf( "  this program continues to write the CRP enabled binary into the target flash\r\n" );
#endif
        }
    }

    printf( "  data size = %d bytes, it takes %d secotrs in flash area\r\n", data_size, last_sector + 1 );
    printf( "  resetting target\r\n" );

#ifdef ENABLE_WRITING
    if ( erase_sectors( find_sector( data_size - 1, tpp ) ) )
        return ( ERROR_AT_SECTOR_ERASE );

    printf( "\r\n  ==== flash writing ====\r\n" );

    if ( (err = write_flash( fp, tpp, &transferred_size, data_size )) != 0 )
        return ( err );

    printf( "  -- %d bytes data are written\r\n", transferred_size );
    leds_off();
#else
    printf( "\r\n  ==== writing is disabled ====\r\n\r\n" );
#endif


#if 0
    if ( tpp->id == 0x26113F37 ) {
        //  The MEMMAP register should be operated if want to verify the LPC176x flash data
    }
#endif


#ifdef ENABLE_VERIFYING
    if ( (tpp->id == 0x26113F37) || (tpp->id == 0x26013F37) ) {
        printf( "\r\n  ==== for the LPC176x, verification is not supported ====\r\n\r\n" );
    } else {
        printf( "\r\n  ==== flash reading and verifying ====\r\n" );

        if ( (err    = verify_flash( fp, tpp, &transferred_size, data_size )) != 0 )
            return ( err );

        printf( "  -- %d bytes data are read and verified\r\n", transferred_size );
        leds_off();
    }
#else
    printf( "\r\n  ==== verifying has been skipped ====\r\n\r\n" );
}
#endif


    fclose( fp );

    post_writing_process( tpp );

    return ( NO_ERROR );
}


int file_size( FILE *fp )
{
    int     size;

    fseek( fp, 0, SEEK_END ); // seek to end of file
    size    = ftell( fp );    // get current file pointer
    fseek( fp, 0, SEEK_SET ); // seek back to beginning of file

    return size;
}


unsigned int crp_check( FILE *fp )
{
    unsigned int    crp;

    switch ( crp    = read_crp( fp ) ) {
        case NO_ISP :
            printf( "\r\n  WARNING : CRP code detected 0x%08X (NO_ISP)\r\n", crp );
            break;
        case CRP1 :
            printf( "\r\n  WARNING : CRP code detected 0x%08X (CRP1)\r\n", crp );
            break;
        case CRP2 :
            printf( "\r\n  WARNING : CRP code detected 0x%08X (CRP2)\r\n", crp );
            break;
        case CRP3 :
            printf( "\r\n  WARNING : CRP code detected 0x%08X (CRP3)\r\n", crp );
            break;
        default :
            crp = 0x0;  //  no CRP code detected
            break;
    }

    return ( crp );
}


unsigned int read_crp( FILE *fp )
{
    unsigned int    crp;

    fseek( fp, CRP_WORD_OFFSET, SEEK_SET ); // seek to 0x2FC (764th byte)

    if ( 1 != fread( &crp, sizeof( crp ), 1, fp ) )
        return ( CRP_CHECK_ERROR );

    fseek( fp, 0, SEEK_SET ); // seek back to beginning of file

    return ( crp );
}


void start_target_in_normal_mode( int baud_rate )
{
    set_target_baud_rate( baud_rate );
    reset_target( NO_ISP_MODE );
}

void start_success_indicator( void )
{
    success.attach( &success_indicator, 0.1 );
}

void success_indicator()
{
    static int i  = 0;
    static unsigned char c = 0xF0;

    leds = c & 0xF;

    c >>= 1;
    if ((++i % 8) == 0)
    {
        c = 0xF0;
    }
    if (i == (8*4))
    {
        leds = 0;
        success.detach();
    }
}

void toggle_led( char v )
{
    leds    = leds ^ (0x1 << v);
}


void leds_off( void )
{
    leds    = 0x0;
}

void show_progress( int total_size, int file_size )
{
    printf( "  -- %5.1f%%\r", ((float)total_size/(float)file_size) * 100.0 );
    fflush( stdout );
}
