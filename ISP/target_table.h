#ifndef MBED_ISP__TARGET_TABLE__
#define MBED_ISP__TARGET_TABLE__


#define     NOT_FOUND   0

#define UUENCODE    0
#define BINARY      1


typedef struct  taget_param_st {
    char            *type_name;
    int             id;
    int             ram_size;
    int             flash_size;
    int             sector_size;
    int             write_type;
    unsigned int    ram_start_address;
}
target_param;

extern target_param    target_table[];

target_param    *find_target_param( char *device_id_string );
int             find_sector( int data_size, target_param *tpp );


#endif  //  MBED_ISP__TARGET_TABLE__

