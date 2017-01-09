#include    "target_table.h"


int     write_flash( FILE *fp, target_param *tpp, int *transferred_size, int file_size );
void    add_isp_checksum( char *b );
int     post_writing_process( target_param *tpp );


