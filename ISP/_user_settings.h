#ifndef MBED_ISP___USER_SETTINGS__
#define MBED_ISP___USER_SETTINGS__

///  file name for the binary data
//  edit here if you need to use different file name or storage media
#define     SOURCE_FILE         "/local/1114bin"
#define     SUFFIX_FILTER_STR   "*"

// reset and pin assignments
#define     ISP_ENABLE_PIN      p26
#define     RESET_PIN           p25
#define     SERIAL_TX_PIN       p28
#define     SERIAL_RX_PIN       p27

//  "ISP_BAUD_RATE" is baud rate for ISP operation
#define     ISP_BAUD_RATE       115200


//  "TARGET_OPERATION_BAUD_RATE" is baud rate for USB-serial bridge operation after
//  ISP completion.
//  if the target application uses serial(UART) and you use the bridge feature,
//  please set this value correctly.
#define     TARGET_OPERATION_BAUD_RATE  9600


//  suppress "command and result" monitoring output to terminal
//  the "command and result" monitoring feature is made for debugging.
#define     SUPPRESS_COMMAND_RESULT_MESSAGE


//  enable "AUTO_PROGRAM_START" to let target starts the program after flash writing complete
#define     AUTO_PROGRAM_START


//  disabling "ENABLE_WRITING" can be used for "verification only" operation.
#define     ENABLE_WRITING


//  enable "ENABLE_VERIFYING" to let perform verification by comparing "bin" file and flash read data.
#define     ENABLE_VERIFYING


//  enable "ENABLR_PROGRESS_DISPLAY" to let display data write/verify progress.
#define     ENABLE_PROGRESS_DISPLAY


//  enable "CHECK_CRP_CODE" to check the CRP (Code Read Protection). The ISP writing will be ignored if "bin" file has CRP code.
#define     CHECK_CRP_CODE


//  enable "ENABLE_VERIFYING" to let perform verification by comparing "bin" file and flash read data.
#define     ENABLE_FILE_SELECTOR


#endif  //  MBED_ISP___USER_SETTINGS__




////////////////////////////
//  FOLLOWING IS MESSAGE THAT WILL BE SHOWN WHEN COMPILE THIS CODE
//  you can edit next lines if you want remove error/warning message
///////////////////////////

#if     defined( ENABLE_WRITING ) && defined( ENABLE_VERIFYING )
    #define WHAT_WAS_DONE    "written and verified"
#elif   defined( ENABLE_WRITING ) && !defined( ENABLE_VERIFYING )
    #define WHAT_WAS_DONE    "written"
    #warning "ENABLE_VERIFYING" in "_user_setting.h" is disabled. No verification will be performed.
#elif   !defined( ENABLE_WRITING ) && defined( ENABLE_VERIFYING )
    #define WHAT_WAS_DONE    "verified"
    #warning "ENABLE_WRITING" in "_user_setting.h" is disabled. No writing will be performed.
#else
    #error both "ENABLE_WRITING" and "ENABLE_VERIFYING" are disabled ("in user_setting.h"). The program need to execute nothing
#endif

