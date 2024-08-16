#ifndef _NX_FS_H
#define _NS_FS_H

#include"../com.h"

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<stdint.h>
#include<stdarg.h>

typedef struct {
    uint8_t hr;
    uint8_t min;
    uint8_t sec;
    uint8_t day;
    uint8_t mo;
    uint16_t yr;
} fmtdate_t;

#define STR_CPY( src, dest, sz ) for( int _i = 0; _i < sz; _i++ ) { \
                                    dest[ _i ] = src[ _i ]; \
                                }

extern const char* fd_type_str[];

typedef enum {
    FD_NULL = 0,
    FD_EXEC, // executable
    FD_RTXT, // raw text
    FD_BIN, // raw binary
    FD_DEV // mapped to device
} fd_type_t;
typedef struct {
    fmtdate_t fd_cdate; // creation date
    fmtdate_t fd_mdate; // modified date
    uint64_t fd_size; // size
    uint8_t fd_flags; // flags
    char fd_name[ 8 ]; // name
    fd_type_t fd_type; // file type
    int32_t* fd_block; // block of memory that the file refers to
} unode_t;

extern unode_t* g_fs;
extern uint64_t g_fs_sz;

extern bool g_24hr;

bool nxfs_valid_date( fmtdate_t date );
void nxfs_printdate( fmtdate_t date );
fmtdate_t nxfs_date( uint8_t mo, uint8_t day, uint16_t yr, uint8_t hr, uint8_t min, uint8_t sec );
unode_t* nxfs_register_file( uint8_t type, char* name, uint8_t flags, uint32_t blocks, void* block );
void nxfs_fmod_block( unode_t* file, void* block, uint32_t size );
void nxfs_fmod_attrib( unode_t* file, uint8_t chgflags, fd_type_t fd_type, char* name, fmtdate_t fd_cdate, fmtdate_t fd_mdate, uint8_t flags );
void nxfs_unregister_file( unode_t* file );
void nxfs_file_stat( unode_t* file );
unode_t* nxfs_file_search( char* name );

#define FMODA_CHG_TYPE 0b00000001
#define FMODA_CHG_NAME 0b00000010
#define FMODA_CHG_CDATE 0b00000100
#define FMODA_CHG_MDATE 0b00001000
#define FMODA_CHG_FLAGS 0b00010000

#define FMODA_TYPE_NULL 0
#define FMODA_NAME_NULL ""
#define FMODA_DATE_NULL ( fmtdate_t ){ 0, 0, 0, 0, 0, 0 }
#define FMODA_FLAGS_NULL 0

#endif // _NX_FS_H