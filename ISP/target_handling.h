
#include    "target_table.h"


#define     ENTER_TO_ISP_MODE   0
#define     NO_ISP_MODE         1

#define     SECTOR_SIZE_VALIABLE4Kx16_AND_32KxN     1
#define     SECTOR_SIZE_VALIABLE4Kx24_AND_32KxN     2

target_param    *open_target( int baud_date );
void            reset_target( int isp_pin_state );

