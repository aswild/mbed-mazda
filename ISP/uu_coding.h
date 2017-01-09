
void    initialize_uue_table( void );
void    initialize_uud_table( void );
long    bin2uue( char *bin, char *str, int size );
int     uudecode_a_line( char *b, char *s );

extern char    uu_table[ 0x60 + 1 ];
