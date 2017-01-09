
#define     STR_BUFF_SIZE       128

int     try_and_check( char *command, char *expected_return_str );
int     try_and_check2( char *command, char *expected_return_str );
void    print_command( char *command );
void    print_result( int r );
int     send_RAM_transfer_checksum( int checksum );
int     erase_sectors( int last_sector );

