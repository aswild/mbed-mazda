char    uu_table[ 0x60 + 1 ];


void initialize_uue_table( void )
{
    int     i;

    uu_table[ 0 ] = 0x60;           // 0x20 is translated to 0x60 !

    for ( i = 1; i < 64; i++ ) {
        uu_table[ i ] = (char)(' ' + i);
    }
}


void initialize_uud_table( void )
{
    int     i;

    uu_table[ 0x60 ] = 0;

    for ( i = 0x21; i < 0x60; i++ ) {
        uu_table[ i ] = i - 0x20;
    }
}


int uudecode_a_line( char *b, char *s )
{

    unsigned long   v;
    int             read_size;

    read_size   = (*s++) - ' ';

    for ( int i = 0; i < read_size; i += 3 ) {
        v        = uu_table[ *s++ ] << 18;
        v       |= uu_table[ *s++ ] << 12;
        v       |= uu_table[ *s++ ] <<  6;
        v       |= uu_table[ *s++ ] <<  0;

        *b++     = (v >> 16) & 0xFF;
        *b++     = (v >>  8) & 0xFF;
        *b++     = (v >>  0) & 0xFF;
    }

    return ( read_size );
}


long bin2uue( char *bin, char *str, int size )
{
    unsigned long   v;
    long            checksum    = 0;
    int             strpos      = 0;

    *(str + strpos++) = ' ' + size;

    for ( int i = 0; i < size; i += 3 ) {
        checksum    += *(bin + i + 0) + *(bin + i + 1) + *(bin + i + 2);
        v   = (*(bin + i + 0) << 16) | (*(bin + i + 1) << 8) | (*(bin + i + 2) << 0);
        *(str + strpos++) = uu_table[ (v >> 18) & 0x3F ];
        *(str + strpos++) = uu_table[ (v >> 12) & 0x3F ];
        *(str + strpos++) = uu_table[ (v >>  6) & 0x3F ];
        *(str + strpos++) = uu_table[ (v >>  0) & 0x3F ];
    }
    *(str + strpos++) = '\n';
    *(str + strpos++) = '\0';

    return checksum;
}
