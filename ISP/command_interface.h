
#define     STR_BUFF_SIZE       128

int     try_and_check( const char *command, const char *expected_return_str );
int     try_and_check2( const char *command, const char *expected_return_str );
void    print_command( const char *command );
void    print_result( int r );
int     send_RAM_transfer_checksum( int checksum );
int     erase_sectors( int last_sector );

