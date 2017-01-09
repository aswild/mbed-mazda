/**
 *  DirectoryList library
 *
 *  @author  Tedd OKANO
 *  @version 0.1
 *  @date    Jan-2015
 *
 *  A simple directory listing interface
 */

#include    "mbed.h"
#include    "DirectoryList.h"
#include    <algorithm>

DirectoryList::DirectoryList( char *path ) : error_flag( NOT_INITIALIZED )
{
    init( string( path ) );
}

DirectoryList::DirectoryList( std::string path ) : error_flag( NOT_INITIALIZED )
{
    init( path );
}

void DirectoryList::init( std::string path )
{
    DIR             *dir;
    struct dirent   *dp;

    if ( NULL == (dir   = opendir( path.c_str() )) ) {
        error_flag  = ERROR_AT_FILE_OPEN;
        return;
    }

    while ( NULL != (dp    = readdir( dir )) )
        this->push_back( dp->d_name );

    closedir( dir );

    std::sort( this->begin(), this->end() );
    error_flag  = NO_ERROR;
}

DirectoryList::~DirectoryList()
{
}

int DirectoryList::error_check( void )
{
    return error_flag;
}
