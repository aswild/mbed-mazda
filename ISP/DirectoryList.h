/**
 *  DirectoryList library
 *
 *  @author  Tedd OKANO
 *  @version 0.1
 *  @date    Jan-2015
 *
 *  A simple directory listing interface
 */

#ifndef MBED_DIRECTORY_LIST_H
#define MBED_DIRECTORY_LIST_H

#include    <vector>
#include    <string>

/** DirectoryList class
 *
 *  Simple interface to get file name list of directory
 *  which is specified by path.
 *
 *  The DirectoryList instance will be an object of "std::vector<std::string>".
 *
 *  This library works on local strage on mbed as well as on USB strage.
 *  (It has not been tested on SD yet.)
 *
 *  Example:
 *  @code
 *  #include "mbed.h"
 *  #include "DirectoryList.h"
 *
 *  LocalFileSystem local( "local" );
 *
 *  int main(void)
 *  {
 *      DirectoryList   dir( "/local" );
 *
 *      if ( dir.error_check() )
 *          error( "directory could not be opened\r\n" );
 *
 *      for ( int i = 0; i < dir.size(); i++ )
 *          printf( "%s\r\n", dir[ i ].c_str() );
 *  }
 *  @endcode
 */

class DirectoryList : public std::vector<std::string>
{
    typedef enum {
        NO_ERROR        = 0,
        NOT_INITIALIZED,
        ERROR_AT_FILE_OPEN
    } ErrorCode;

public:

    /** Create a DirectoryList instance of specified path
     *
     * @param path string which tells directory path
     */
    DirectoryList( char *path );

    /** Create a DirectoryList instance of specified path
     *
     * @param path string which tells directory path
     */
    DirectoryList( std::string path );

    /** Destructor of DirectoryList
     */
    ~DirectoryList();

    /** Error check function
     *
     *  @return zero if no error
     */
    int error_check( void );

private:

    void init( std::string path );
    int error_flag;
};

#endif  //  MBED_DIRECTORY_LIST_H