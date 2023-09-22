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

typedef enum {
    FD_NULL = 0,
    FD_EXEC, // executable
    FD_RTXT, // raw text
    FD_BIN, // raw binary
    FD_DEV // mapped to device
} fd_type_t;
typedef struct {
    uint32_t size;
    int32_t* contents;
} fd_block_t;
typedef struct {
    fmtdate_t fd_cdate; // creation date
    fmtdate_t fd_mdate; // modified date
    uint64_t fd_size; // size
    uint8_t fd_flags; // flags
    char fd_name[ 8 ]; // name
    fd_type_t fd_type; // file type
    int32_t* fd_block; // block of memory that the file refers to
} unode_t;
typedef enum {
    T_NULL = 0,
    T_INT8,
    T_INT16,
    T_INT32,
    T_INT64,
    T_CHAR,
    T_STRING,
    T_HEX,
    T_VOIDP
} fstat_contents_t;

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
// put off for later lol
void table( uint8_t ncol, ... ) {
    va_list vlist;
    uint8_t ncol_real = 2*ncol;
    uint8_t vlist_call = 0;
    struct {
        char title[ 16 ];
        int width;
    } ncol_list[ 10 ];
    // initialize
    for( int i = 0; i < 10; i++ ) {
        ncol_list[ i ].width = -1;
    }
    // get args into list
    va_start( vlist, ncol );
    for( int i = 0; i < 10; i++ ) {
        // error check
        if( vlist_call >= ncol_real ) break;
        // first arg - title
        char* l_title = va_arg( vlist, char* );
        vlist_call++;
        // error check
        if( vlist_call >= ncol_real ) fatal( "incorrect number of arguments supplied to table( uint8_t )\n" );
        STR_CPY( l_title, ncol_list[ i ].title, 16 );
        // second arg - width
        ncol_list[ i ].width = va_arg( vlist, int );
        vlist_call++;
        // debug
        //printf( "ncol_list[ %d ].title = %s\nncol_list[ %d ].width = %d\n\n", i, ncol_list[ i ].title, i, ncol_list[ i ].width );
    }
    va_end( vlist );
    printf( "\n\n" );
    
    // top row border
    printf( "+" );
    for( int i = 0; i < 10; i++ ) {
        if( ncol_list[ i ].width < 0 ) {
            break;
        }
        for( int k = 0; k < ncol_list[ i ].width; k++ ) {
            printf( "-" );
        }
        printf( "+" );
    }
    
    printf( "\n" );
    // header contents
    for( int i = 0; i < 10; i++ ) {
        // continue
    }
}

unode_t* register_file( uint8_t type, char name[ 8 ], uint8_t flags, uint32_t blocks, void* block ) {
    unode_t* file = calloc( 1, sizeof( *file ) );
    if( !file ) fatal( "failed to register file: memory allocation error.\n" );
    file->fd_cdate = i_date( 9, 19, 2023, 13, 30, 3 );
    file->fd_mdate = i_date( 9, 19, 2023, 13, 31, 34 );
    file->fd_size = blocks*4;
    file->fd_flags = flags;
    STR_CPY( name, file->fd_name, 8 );
    file->fd_type = type;
    //file->fd_block.size = block;
    //file->fd_block.contents = block;
    file->fd_block = block;
    return file;
}
void fmod_block( unode_t* file, void* block, uint32_t size ) {
    file->fd_block = block;
    file->fd_size = size;
}
void fmod_attrib( unode_t* file, uint8_t chgflags, fd_type_t fd_type, char name[ 8 ], fmtdate_t fd_cdate, fmtdate_t fd_mdate, uint8_t flags ) {
    // chgflags: binary flags left to right in order of function parameters
    // digits go right to left: changing the type will set bit 0b00000001
    if( chgflags & 0b00000001 ) file->fd_type = fd_type;
    if( chgflags & 0b00000010 ) STR_CPY( name, file->fd_name, 8 );
    if( chgflags & 0b00000100 ) file->fd_cdate = fd_cdate;
    if( chgflags & 0b00001000 ) file->fd_mdate = fd_mdate;
    if( chgflags & 0b00010000 ) file->fd_flags = flags;
}
void unregister_file( unode_t* file ) {
    free( file );
}
void file_stat( unode_t* file ) {
    printf( "| FILE STAT\n" );
    printf( "| Date created: " );
    i_printdate( file->fd_cdate );
    printf( "\n| Date modified: " );
    i_printdate( file->fd_mdate );
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
            fatal( "null fd_type processed.\n" );
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
            fatal( "unkonwn fd_type processed.\n" );
            break;
    }
}

#define FMODA_CHG_TYPE 0b00000001
#define FMODA_CHG_NAME 0b00000010
#define FMODA_CHG_CDATE 0b00000100
#define FMODA_CHG_MDATE 0b00001000
#define FMODA_CHG_FLAGS 0b00010000

#define FMODA_TYPE_NULL 0
#define FMODA_NAME_NULL ""
#define FMODA_DATE_NULL ( fmtdate_t ){ 0, 0, 0, 0, 0, 0 }
#define FMODA_FLAGS_NULL 0

int main( void ) {
    g_24hr = false;
    int32_t test_block_int = 1024;
    char* test_block_str1 = "testing";
    char* test_block_str2 = "testing2";
    unode_t* test_file1 = register_file( FD_BIN, "test1", 0, 1, &test_block_int );
    unode_t* test_file2 = register_file( FD_RTXT, "test2", 0, 8, test_block_str1 );
    file_stat( test_file1 );
    printf( ">\n" );
    fmod_block( test_file1, test_block_str2, 9 );
    fmod_attrib( test_file1, FMODA_CHG_TYPE, FD_RTXT, FMODA_NAME_NULL, FMODA_DATE_NULL, FMODA_DATE_NULL, FMODA_FLAGS_NULL );
    file_stat( test_file1 );
    printf( ">\n" );
    file_stat( test_file2 );
    unregister_file( test_file1 );
    unregister_file( test_file2 );
    return 0;
}
