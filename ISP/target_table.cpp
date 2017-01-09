#include    "mbed.h"
#include    "target_table.h"

#define     SECTOR_SIZE_VALIABLE4Kx16_AND_32KxN     1
#define     SECTOR_SIZE_VALIABLE4Kx24_AND_32KxN     2

target_param    target_table[]  = {
    { "unknown ttarget",        0xFFFFFFFF, 1024,    4096, 4096, UUENCODE, 0x10000200 },
    { "LPC1114FN28(FDH28)/102", 0x0A40902B, 4096,   32768, 4096, UUENCODE, 0x10000200 },
    { "LPC1114FN28(FDH28)/102", 0x1A40902B, 4096,   32768, 4096, UUENCODE, 0x10000200 },
    { "LPC1115",                0x00050080, 8192,   65536, 4096, UUENCODE, 0x10000200 },
    { "LPC810M021FN8",          0x00008100, 1024,    4096, 1024, BINARY,   0x10000300 },
    { "LPC811M001JDH16",        0x00008110, 2048,    8192, 1024, BINARY,   0x10000300 },
    { "LPC812M101JDH16",        0x00008120, 4096,   16384, 1024, BINARY,   0x10000300 },
    { "LPC812M101JD20",         0x00008121, 4096,   16384, 1024, BINARY,   0x10000300 },
    { "LPC812M101JDH20",        0x00008122, 4096,   16384, 1024, BINARY,   0x10000300 },
///added for LPC82x series
    { "LPC824M201JHI33",        0x00008241, 8192,   32768, 1024, BINARY,   0x10000300 },
    { "LPC822M101JHI33",        0x00008221, 4096,   16384, 1024, BINARY,   0x10000300 },
    { "LPC824M201JDH20",        0x00008242, 8192,   32768, 1024, BINARY,   0x10000300 },
    { "LPC822M101JDH20",        0x00008222, 4096,   16384, 1024, BINARY,   0x10000300 },
///added for LPC176x series
    { "LPC1769FBD100",          0x26113F37, 65536, 524288, SECTOR_SIZE_VALIABLE4Kx16_AND_32KxN, UUENCODE, 0x10000200 },
    { "LPC1768FBD100",          0x26013F37, 65536, 524288, SECTOR_SIZE_VALIABLE4Kx16_AND_32KxN, UUENCODE, 0x10000200 },
///added for LPC11U(E)68x　series
    { "LPC11U68JBD100",         0x00007C00, 36864, 262144, SECTOR_SIZE_VALIABLE4Kx24_AND_32KxN, UUENCODE, 0x10000300 },
    { "LPC11E68JBD100",         0x00007C01, 36864, 262144, SECTOR_SIZE_VALIABLE4Kx24_AND_32KxN, UUENCODE, 0x10000300 },
};

target_param *find_target_param( char *device_id_string )
{
    int     id;

    id  = atoi( device_id_string );

    for ( int i = 1; i < (sizeof( target_table ) / sizeof( target_param )); i++ ) {
        if ( id == target_table[ i ].id )
            return ( &(target_table[ i ]) );
    }

    //  return ( target_table );
    return ( NULL );
}

int find_sector( int data_size, target_param *tpp )
{
    int result;

    switch ( tpp->sector_size ) {
        case SECTOR_SIZE_VALIABLE4Kx16_AND_32KxN :
            if ( data_size <= (4096 * 16) ) {
                result = ( data_size / 4096 );
            } else {
                data_size   -= (4096 * 16);
                result = ( (data_size / (4096 * 8)) + 16 );
            }
            break;

        case SECTOR_SIZE_VALIABLE4Kx24_AND_32KxN :
            if ( data_size <= (4096 * 24) ) {
                result = ( data_size / 4096 );
            } else {
                data_size   -= (4096 * 24);
                result = ( (data_size / (4096 * 12)) + 24 );
            }
            break;

        default :
            result = ( data_size / tpp->sector_size );
            //break;
    }
    return (result);
}

