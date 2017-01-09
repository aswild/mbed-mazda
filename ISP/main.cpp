/**
 *  Sample of ISP operation for NXP MCUs
 *
 *  @author  Tedd OKANO
 *  @version 1.2.1
 *  @date    Aug-2015
 *
 *  This program programs MCU flash memory through UART. It uses
 *  "In-System Programming (ISP)" interface in target MCU (NXP LPC micro-
 *  controllers).
 *
 *  The ISP is done by PC and serial cable normally. The ISP protocol is
 *  executed software on a PC. The software reads a data file and transfers
 *  the data with the ISP protocol.
 *  This program does same process of that. The mbed perform the function like
 *  "FlashMagic" and "lpc21isp".
 *  (This program not just copies the binary but also insert 4 byte checksum at
 *  address 0x1C.)
 *
 *  This program supports
 *      LPC1114, LPC1115, LPC81x, LPC82x, LPC1768/1769 and LPC11U68/LPC11E68.
 *
 *  All detailed history is available in code publishing page.
 *  Please find it on
 *    https://developer.mbed.org/users/okano/code/ika_shouyu_poppoyaki/shortlog
 *  Thank you for all feedbacks and controbutes!
 */

#include    "mbed.h"
#include    "isp.h"
#include    "serial_utilities.h"
#include    "_user_settings.h"
#include    "dir_handling.h"


LocalFileSystem local( "local" );


#define     SUMMARY_MESSAGE_FAIL        "** The data could not be " WHAT_WAS_DONE " :("
#define     SUMMARY_MESSAGE_SUCCESS     "** The data has been " WHAT_WAS_DONE " successflly :)"

int main()
{
    int     err;

    printf( "\r\n\r\n\r\nmbed ISP program : programming LPC device from mbed (v1.2.1)\r\n" );

#ifndef ENABLE_FILE_SELECTOR
    err     = isp_flash_write( SOURCE_FILE );
#else
    std::string str( SOURCE_FILE );
    if ( "" == get_file_name( str, SUFFIX_FILTER_STR ) )
        goto skip_ISP;
    err     = isp_flash_write( str.c_str() );
#endif

    printf( "\r\n  %d: %s\r\n\r\n",err,
            err ?
            SUMMARY_MESSAGE_FAIL :
            SUMMARY_MESSAGE_SUCCESS
          );

    if ( err )
        error( "  ** ISP failed\r\n" );

skip_ISP:

#ifdef  AUTO_PROGRAM_START

    start_target_in_normal_mode( TARGET_OPERATION_BAUD_RATE );

    printf( "  ** The program programmed in flash has been started!!\r\n" );

#endif

    printf( "     (now the mbed is working in \"serial through mode\")\r\n\r\n" );

    start_success_indicator();
    usb_serial_bridge_operation();  //  doesn't return. infinite loop in this function
}

