#ifndef _NX_COM_H
#define _NX_COM_H

#define NX_VERSION "0004"
#define NX_DATEBLD "09/25/2023 2:19 AM"

#define NX_SUCCESS 0
#define NX_ERROR 1

#define MEM_SAFECHECK( block ) if( !block ) { SYS_fatal( "failed to allocate memory to " #block "\n" ); }

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<stdint.h>
#include<stdarg.h>

// Program structure
typedef struct {
    uint32_t id;
    char* cmd;
    int32_t ( *exec )( int argc, char** argv );
    bool valid;
} nx_program_t;
// Program List structure
typedef struct nx_program_list_s {
    nx_program_t entries[ 16 ];
    uint32_t count;
} nx_program_list_t;

// System state structure
typedef struct {
    bool run;
    int32_t err;
} nx_state_t;

extern nx_state_t g_state;
extern nx_program_list_t g_program_list;

void SYS_fatal( char* fmt, ... );
void SYS_nonfatal( char* fmt, ... );

#endif // _NX_COM_H