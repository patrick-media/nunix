#include<stdio.h>
#include<stdint.h>
#include<stdbool.h>
#include<stdlib.h>
#include<stdarg.h>
typedef struct {
    uint8_t hr;
    uint8_t min;
    uint8_t sec;
    uint8_t day;
    uint8_t mo;
    uint16_t yr;
} fmtdate_t;
typedef struct {
    uint8_t f0 : 1; // reserved
    uint8_t f1 : 1; // reserved
    uint8_t f2 : 1; // reserved
    uint8_t f3 : 1; // reserved
    uint8_t f4 : 1; // reserved
    uint8_t f5 : 1; // reserved
    uint8_t f6 : 1; // reserved
    uint8_t f7 : 1; // reserved
} fdfl_t; // file descriptor flags

#define STR_CPY( src, dest, sz ) for( int _i = 0; _i < sz; _i++ ) { \
                                    dest[ _i ] = src[ _i ]; \
                                }

const char* fd_type_str[ 5 ] = {
    "NULL", "EXEC", "RTXT", "BIN", "DEV"
};
typedef struct {
    fmtdate_t fd_cdate; // creation date
    fmtdate_t fd_mdate; // modified date
    uint64_t fd_size; // size
    uint8_t fd_flags; // flags
    char fd_name[ 8 ]; // name
    enum {
        FD_NULL = 0,
        FD_EXEC, // executable
        FD_RTXT, // raw text
        FD_BIN,  // raw binary
        FD_DEV  // mapped to device
    } fd_type; // file type
    void* fd_block; // block of memory that the file refers to
} unode_t;

bool g_24hr;

void fatal( char* fmt, ... ) {
    va_list vlist;
    va_start( vlist, fmt );
    printf( "FATAL ERROR: " );
    vprintf( fmt, vlist );
    va_end( vlist );
    exit( 1 );
}
void nonfatal( char* fmt, ... ) {
    va_list vlist;
    va_start( vlist, fmt );
    printf( "NONFATAL ERROR: " );
    vprintf( fmt, vlist );
    va_end( vlist );
}

bool i_valid_date( fmtdate_t date ) {
    if( date.hr > 23 ) fatal( "invalid date format: hr is %d, more than 23", date.hr );
    if( date.min > 59 ) fatal( "invalid date format: min is %d, more than 59", date.min );
    if( date.sec > 59 ) fatal( "invalid date format: sec is %d, more than 59", date.sec );
    if( date.day > 31 ) fatal( "invalid date format: day is %d, more than 31", date.day );
    if( date.mo > 12 ) fatal( "invalid date format: mo is %d, more than 12", date.mo );
    return true;
}
void i_printdate( fmtdate_t date ) {
    i_valid_date( date );
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
fmtdate_t i_date(
    uint8_t mo,
    uint8_t day,
    uint16_t yr,
    uint8_t hr,
    uint8_t min,
    uint8_t sec ) {
    fmtdate_t date;
    date.yr = yr;
    date.mo = mo;
    date.day = day;
    date.hr = hr;
    date.min = min;
    date.sec = sec;
    i_valid_date( date ); // check for validity
    return date;
}
void table( uint8_t ncol, ... ) {
    va_list vlist;
    va_start( vlist, ncol );
    uint8_t ncol_real = 2*ncol;
    uint8_t vlist_call = 0;
    struct {
        char title[ 16 ];
        int width;
    } ncol_list[ 10 ];
    for( int i = 0; i < 10; i++ ) {
        if( vlist_call >= ncol_real ) break;
        //* l_title = va_arg( vlist, char );
        //printf( "l_title = %s\n", l_title );
        vlist_call++;
        printf( "vlist_call = %d ncol_real = %d\n", vlist_call, ncol_real );
        if( vlist_call > ncol_real ) fatal( "incorrect number of arguments supplied to table( uint8_t )\n" );
        //STR_CPY( l_title, ncol_list[ i ].title, 16 );
        ncol_list[ i ].width = va_arg( vlist, int );
        vlist_call++;
    }
    
    // top row border
    printf( "+" );
    for( int i = 0; i < ncol_list[ 0 ].width; i++ ) {
        printf( "-" );
    }
    printf( "+" );
    va_end( vlist );
}

unode_t* register_file( uint8_t type, char name[ 8 ], uint8_t flags ) {
    unode_t* file = calloc( 1, sizeof( *file ) );
    if( !file ) fatal( "failed to register file: memory allocation error.\n" );
    file->fd_cdate = i_date( 9, 19, 2023, 13, 30, 3 );
    file->fd_mdate = i_date( 9, 19, 2023, 13, 31, 34 );
    file->fd_size = 16;
    file->fd_flags = flags;
    STR_CPY( name, file->fd_name, 8 );
    file->fd_type = type;
    file->fd_block = 0;
    return file;
}
void unregister_file( unode_t* file ) {
    free( file );
}
void file_stat( unode_t* file ) {
    printf( "Date created: " );
    i_printdate( file->fd_cdate );
    printf( "\nDate modified: " );
    i_printdate( file->fd_mdate );
    printf( "\nSize: %d", file->fd_size );
    printf( "\nFlags: " );
    if( file->fd_flags == 0 ) printf( "NULL" );
    else printf( "%d", file->fd_flags );
    printf( "\nName: %s", file->fd_name );
    printf( "\nType: %d (%s)", file->fd_type, fd_type_str[ file->fd_type ] );
}

int main( void ) {
    g_24hr = false;
    unode_t* test_file = register_file( FD_BIN, "test", 0 );
    //file_stat( test_file );
    unregister_file( test_file );
    table( 4, "test1", 10, "test2", 10 );
    return 0;
}
