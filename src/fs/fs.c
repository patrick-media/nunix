#include"fs.h"

const char* fd_type_str[ 5 ] = {
    "NULL", "EXEC", "RTXT", "BIN", "DEV"
};

unode_t* g_fs = 0;
uint64_t g_fs_sz = 0;

bool nxfs_valid_date( fmtdate_t date ) {
    if( date.hr > 23 ) SYS_fatal( "invalid date format: hr is %d, more than 23", date.hr );
    if( date.min > 59 ) SYS_fatal( "invalid date format: min is %d, more than 59", date.min );
    if( date.sec > 59 ) SYS_fatal( "invalid date format: sec is %d, more than 59", date.sec );
    if( date.day > 31 ) SYS_fatal( "invalid date format: day is %d, more than 31", date.day );
    if( date.mo > 12 ) SYS_fatal( "invalid date format: mo is %d, more than 12", date.mo );
    return true;
}
void nxfs_printdate( fmtdate_t date ) {
    nxfs_valid_date( date );
    // morning / afternoon logic
    char am_pm[ 3 ] = "AM";
    // 24-hour clock
    if( !g_24hr ) {
        if( date.hr > 11 ) {
            // keep 12 PM
            if( date.hr != 12 ) date.hr -= 12;
            am_pm[ 0 ] = 'P';
        }
    }
    
    // print leading zero
    if( date.hr < 10 ) printf( "0" );
    // print number
    printf( "%d:", date.hr );
    if( date.min < 10 ) printf( "0" );
    printf( "%d:", date.min );
    if( date.sec < 10 ) printf( "0" );
    printf( "%d ", date.sec );
    printf( "%s", am_pm );
    
    printf( "  " );
    
    // print date
    if( date.mo < 10 ) printf( "0" );
    printf( "%d/", date.mo );
    if( date.day < 10 ) printf( "0" );
    printf( "%d/", date.day );
    printf( "%d", date.yr );
}
fmtdate_t nxfs_date( uint8_t mo,
    uint8_t day,
    uint16_t yr,
    uint8_t hr,
    uint8_t min,
    uint8_t sec )
{
    fmtdate_t date;
    date.yr = yr;
    date.mo = mo;
    date.day = day;
    date.hr = hr;
    date.min = min;
    date.sec = sec;
    nxfs_valid_date( date ); // check for validity
    return date;
}
unode_t* nxfs_register_file(
    uint8_t type,
    char* name,
    uint8_t flags,
    uint32_t blocks,
    void* block )
{
    // Warning: first 7 characters will be valid, the rest will be truncated
    char _name[ 8 ];
    for( int i = 0; i < 8; i++ ) {
        _name[ i ] = name[ i ];
        if( name[ i ] == 0 ) {
            for( int k = i; k < 8; k++ ) {
                _name[ k ] = 0;
            }
            break;
        }
    }
    _name[ 7 ] = 0;
    unode_t* file = calloc( 1, sizeof( *file ) );
    MEM_SAFECHECK( file );
    file->fd_cdate = nxfs_date( 9, 19, 2023, 13, 30, 3 );
    file->fd_mdate = nxfs_date( 9, 19, 2023, 13, 31, 34 );
    switch( type ) {
        case FD_BIN:
            blocks *= 4;
            break;
    }
    file->fd_size = blocks;
    file->fd_flags = flags;
    STR_CPY( _name, file->fd_name, 8 );
    file->fd_type = type;
    file->fd_block = block;
    g_fs_sz++;
    g_fs = realloc( g_fs, g_fs_sz*sizeof( unode_t ) );
    MEM_SAFECHECK( g_fs );
    g_fs[ g_fs_sz-1 ] = *file;
    return file;
}
void nxfs_fmod_block( unode_t* file,
    void* block,
    uint32_t size )
{
    file->fd_block = block;
    file->fd_size = size;
}
void nxfs_fmod_attrib(
    unode_t* file,
    uint8_t chgflags,
    fd_type_t fd_type,
    char* name,
    fmtdate_t fd_cdate,
    fmtdate_t fd_mdate,
    uint8_t flags )
{
    // Warning: first 7 characters will be valid, the rest will be truncated
    char _name[ 8 ];
    for( int i = 0; i < 8; i++ ) {
        _name[ i ] = name[ i ];
        if( name[ i ] == 0 ) {
            for( int k = i; k < 8; k++ ) {
                _name[ k ] = 0;
            }
            break;
        }
    }
    _name[ 7 ] = 0;
    // chgflags: binary flags left to right in order of function parameters
    // digits go right to left: changing the type will set bit 0b00000001
    if( chgflags & 0b00000001 ) file->fd_type = fd_type;
    if( chgflags & 0b00000010 ) STR_CPY( name, file->fd_name, 8 );
    if( chgflags & 0b00000100 ) file->fd_cdate = fd_cdate;
    if( chgflags & 0b00001000 ) file->fd_mdate = fd_mdate;
    if( chgflags & 0b00010000 ) file->fd_flags = flags;
}
void nxfs_unregister_file( unode_t* file ) {
    g_fs_sz--;
    g_fs = realloc( g_fs, g_fs_sz*sizeof( unode_t ) );
    unode_t nullfile = ( unode_t ){ FMODA_DATE_NULL, FMODA_DATE_NULL, 0, 0, "", 0, 0 };
    g_fs[ g_fs_sz ] = nullfile;
    free( file );
}
void nxfs_file_stat( unode_t* file ) {
    printf( "| FILE STAT\n" );
    printf( "| Date created: " );
    nxfs_printdate( file->fd_cdate );
    printf( "\n| Date modified: " );
    nxfs_printdate( file->fd_mdate );
    printf( "\n| Size: %d bytes", file->fd_size );
    printf( "\n| Flags: " );
    if( file->fd_flags == 0 ) printf( "NULL" );
    else printf( "%d", file->fd_flags );
    printf( "\n| Name: %s", file->fd_name );
    uint8_t l_fd_type = file->fd_type;
    printf( "\n| Type: %d (%s)", file->fd_type, fd_type_str[ file->fd_type ] );
    printf( "\n| Contents:\n|\t" );
    switch( l_fd_type ) {
        case FD_NULL:
            SYS_fatal( "null fd_type processed.\n" );
            break;
        case FD_EXEC:
            printf( "Executable format: unable to display contents.\n" );
            break;
        case FD_RTXT:
            printf( "%s\n", ( char* )file->fd_block );
            break;
        case FD_BIN:
            printf( "0x%x\n", *( int32_t* )file->fd_block );
            break;
        case FD_DEV:
            printf( "Device format: contents display not implemented.\n" );
            break;
        default:
            SYS_fatal( "unkonwn fd_type processed.\n" );
            break;
    }
}
unode_t* nxfs_file_search( char* name ) {
    for( int i = 0; i < g_fs_sz-1; i++ ) {
        if( strcmp( name, g_fs[ i ].fd_name ) == 0 ) {
            return &g_fs[ i ];
        }
    }
    SYS_nonfatal( "file search for name '%s' failed.\n", name );
    return NULL;
}