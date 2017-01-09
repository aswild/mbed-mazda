
void    set_target_baud_rate( int baud_rate );
void    usb_serial_bridge_operation( void );

void    put_string( const char *s );
void    put_binary( char *b, int size );
void    get_string( char *s );
int     get_binary( char *b, int length );
char    read_byte( void );
