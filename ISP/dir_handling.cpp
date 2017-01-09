#include    "mbed.h"
#include    "dir_handling.h"
#include    "_user_settings.h"
#include    "DirectoryList.h"
#include    "isp.h"

#include    <iostream>
#include    <string>
#include    <cctype>
#include    <algorithm>

extern Serial      pc;
extern BusOut      leds;

void show_file_list( std::vector<std::string> list );
void user_action_waiting_indicator();


std::string get_file_name( std::string &default_path, const char *suffix )
{
    Ticker  led_blink;
    size_t  last_slash;

    last_slash  = default_path.rfind( "/" );

    if ( last_slash == std::string::npos ) {
        default_path.clear();
        return ( default_path );
    }

    string file_name( default_path.c_str() + last_slash + 1 );
    string path_name( default_path.c_str(), default_path.c_str() + last_slash );

    //printf( "path_name : %s\r\n", path_name.c_str() );
    //printf( "file_name : %s\r\n", file_name.c_str() );

    file_name.push_back( '.' );
    std::transform( file_name.begin(), file_name.end(), file_name.begin(), (int (*)(int))std::toupper );
        //  "toupper" function cast had been done for latest mbed-lib (checked with rev.100).
        //  see --> http://stackoverflow.com/questions/7131858/stdtransform-and-toupper-no-matching-function

    DirectoryList               dir( path_name );
    std::vector<std::string>    list;

    for ( size_t i = 0; i < dir.size(); i++ ) {
        if ( dir[ i ] == file_name ) {
            return ( default_path );
        }

        if ( *suffix == '*' ) {
            list    = dir;
        } else {
            //printf( "-- [%s][%d]\r\n", dir[ i ].c_str(), dir[ i ].find( ".BIN" ) );
            //if ( (8 == dir[ i ].find( ".BIN" )) && (0 != dir[ i ].find( "IKA" )) ) {
            if ( (9 == dir[ i ].find( suffix )) ) {
                list.push_back( dir[ i ] );
            }
        }
    }

    led_blink.attach( &user_action_waiting_indicator, 0.05 );

    printf( "\r\n  WARNING : No file which has name of \"%s\" was found.\r\n", default_path.c_str() );
    printf( "  now showing \"%s\" directory to select a source file.\r\n\r\n", path_name.c_str() );

    if ( !list.size() ) {
        printf( "  WARNING : No file which has suffix of \"%s\" was found.\r\n", suffix );
        default_path.clear();
        return ( default_path );
    }

#define FORMAT_FOR_SELECTED_FILE_NAME   "    selected file : [%3d] : \"%-12s\"            \r"

    //printf( "  ==== directory by \"%s/*.%s\" (excluding \"%s/ika*.bin\") ====\r\n", path_name.c_str(), suffix, path_name.c_str() );
    printf( "  ==== directory by \"%s/*.%s\", %d files ====\r\n", path_name.c_str(), suffix, list.size() );

    show_file_list( list );

    int     done    = false;
    size_t  index   = 0;
    int     c;

    printf( FORMAT_FOR_SELECTED_FILE_NAME, index, list[ index ].c_str() );
    fflush( stdout );

    while ( !done ) {
        if ( pc.readable() ) {
            switch ( c  = pc.getc() ) {
                case '\r' :
                case '\n' :
                    done    = true;
                    break;

                case 'e' :
                case 'E' :
                    printf( "\r\n  Erasing the flash memory\r\n" );
                    default_path    = "\xFF";
                    return ( default_path );
                    //break;

                case 'a' :
                case 'A' :
                case 'c' :
                case 'C' :
                    printf( "\r\n  Aborted to cancel the ISP execution\r\n" );
                    default_path.clear();
                    return ( default_path );
                    //break;

                case '0' :
                case '1' :
                case '2' :
                case '3' :
                case '4' :
                case '5' :
                case '6' :
                case '7' :
                case '8' :
                case '9' :
                    index   = c - '0';
                    index   = ( (list.size() - 1) < index ) ? (list.size() - 1) : index;
                    break;
                case '?' :
                    show_file_list( list );
                    break;


                case 0x1B :
                    if ( 0x5B == pc.getc() )
                        //  arrow key pressed
                        switch ( c  = pc.getc() ) {
                            case 0x41 :
                            case 0x43 :
                                index   += ( index == (list.size() - 1) ) ? 0 : 1;
                                break;
                            case 0x42 :
                            case 0x44 :
                                index   -= ( index == 0 ) ? 0 : 1;
                                break;
                            default :
                                break;
                        }
            }

            printf( FORMAT_FOR_SELECTED_FILE_NAME, index, list[ index ].c_str() );
            fflush( stdout );
        }
    }

    default_path.clear();
    default_path    = path_name + "/" + list[ index ];

    printf( "\r\n    flash writing source file : \"%s\"\r\n\r\n", default_path.c_str() );

    return ( default_path );
}

void show_file_list( std::vector<std::string> list )
{
    int width;
    int height;
    size_t d_i;

    width   = (list.size() / 5) + 1;
    width   = (4 < width) ? 4 : width;
    height  = (list.size() + (width - 1)) / width;

    for ( int i = 0; i < height; i++ ) {
        for ( int j = 0; j < width; j++ ) {

            d_i     = i + j * height;

            if ( list.size() <= d_i )
                break;

            printf( "  %3u : %-12s", d_i, list[ d_i ].c_str() );
        }
        printf( "\r\n" );
    }
    printf( "\r\n" );
    printf( "  ** Use arrow key (up/down or left/right) to select a file\r\n" );
    printf( "  ** or ..\r\n" );
    printf( "  **   press [a] key for abort and go to serial through mode\r\n" );
    printf( "  **   press [e] key for erasing flash\r\n" );
    printf( "  **   press [?] key to display this message again\r\n\r\n" );
}


void user_action_waiting_indicator()
{
    static int  i   = 0;

    if ( (i == 0) || (i == 3) )
        leds    = 0xF;
    else
        leds    = 0x0;

    i++;
    i   &= 0xF;
}



