/*

NUNIX Edition A
8-24-2023

*/

/* Common Inclusions */
#include"com.h"
/* NUNIX Filesystem */
#include"fs/fs.h"
/* Corresponding Program Functions */
#include"cpf/prgms.h"

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<stdint.h>
#include<stdarg.h>

/* Global Functions */
// Fatal error - exit program after displaying a message
void SYS_fatal( char* fmt, ... ) {
    va_list vlist;
    va_start( vlist, fmt );
    printf( "FATAL ERROR: " );
    // vprintf allows for the use of va_list
    vprintf( fmt, vlist );
    va_end( vlist );
    exit( 1 );
}
// Non-fatal error - display message but don't exit
void SYS_nonfatal( char* fmt, ... ) {
    va_list vlist;
    va_start( vlist, fmt );
    printf( "NONFATAL ERROR: " );
    // vprintf allows for the use of va_list
    vprintf( fmt, vlist );
    va_end( vlist );
}

// Argument search method
static char** _arg_search( char* buffer, int* l_argc );
// Free memory allocated by _arg_search that needs to remain allocated after the function returns
static void _del_args( char** args );

// Use 24-hour clock
bool g_24hr;

// Corresponding Program Functions
const nx_program_t p_end =      { 0, "end", cpf_end, true };
const nx_program_t p_help =     { 1, "help", cpf_help, true };
const nx_program_t p_ver =      { 2, "ver", cpf_ver, true };
const nx_program_t p_mkfile =   { 3, "mkfile", cpf_mkfile, true };
const nx_program_t p_fstat =    { 4, "fstat", cpf_fstat, true };
const nx_program_t p_rm =       { 5, "rm", cpf_rm, true };

/* System-wide Variables and Tables */
// System state
nx_state_t g_state = { 0 };
// Program list
nx_program_list_t g_program_list = {
    // Program functions
    .entries = { p_end, p_help, p_ver, p_mkfile, p_fstat, p_rm },
    // Total programs
    .count = _NX_PROGRAMS
};

// Input buffer for scanf();
char* in_buffer = 0;

int main( int argc, char** argv ) {
    /* Memory Allocation */
    in_buffer = calloc( 128, sizeof( *in_buffer ) );
    MEM_SAFECHECK( in_buffer );
    g_fs = calloc( 1, sizeof( unode_t ) );
    MEM_SAFECHECK( g_fs );
    
    /* Variable Assignment */
    g_state.run = true;
    
    /* Introductory Statements */
    printf( "\nNUNIX Edition A\n" );
    printf( "Build: " NX_VERSION "  " NX_DATEBLD "\n" );
    
    /* Main Loop */
    while( g_state.run ) {
        bool cmd_found = false;
        printf( "> " );
        // Put input into in_buffer. \n at the beginning flushes stdout.
        scanf( "\n%[^\n]s", in_buffer );

        // Local argc
        int l_argc = 0;
        // Arguments (argv)
        char** args = _arg_search( in_buffer, &l_argc );

        // Iterate through programs.
        for( int i = 0; i < g_program_list.count; i++ ) {
            // Stop if it's not valid.
            if( g_program_list.entries[ i ].valid ) {
                // Check for the program command.
                if( strcmp( g_program_list.entries[ i ].cmd, args[ 0 ] ) == 0 ) {
                    cmd_found = true;
                    // Result - used for error checking
                    int32_t lp_result = g_program_list.entries[ i ].exec( l_argc, args );
                    // Error check - if program returns anything but NX_SUCCESS
                    if( lp_result != NX_SUCCESS ) {
                        SYS_nonfatal( "Program '%s' exited with a return value of %d.\n",
                                        g_program_list.entries[ i ].cmd, lp_result );
                    }
                }
            }
        }
        // Free memory allocated to args (through _arg_search)
        _del_args( args );
        // Error check
        if( !cmd_found ) {
            printf( "Command not found.\n" );
        }
        // Reset in_buffer
        in_buffer[ 0 ] = 0;
        //memset( in_buffer, 0, 1 );
    }
    
    // Exit program
    return NX_SUCCESS;
}

// Macro to increment a buffer for every space, newline, or tab
#define SKIP_WS( buf ) while( *buf == ' ' || *buf == '\n' || *buf == '\t' ) *buf++;
static bool _arg_validchar( char c ) {
    // Return false if the character is a space, newline, or tab
    if( c == ' ' || c == '\n' || c == '\t' ) return false;
    // Otherwise, return true
    return true;
}
static char** _arg_search( char* buffer, int* l_argc ) {
    // Allocate memory for return value
    char** args = calloc( 32, sizeof( *args ) );
    // Make sure calloc() worked
    MEM_SAFECHECK( args );
    // Allocate memory for each sub-pointer in args
    for( int i = 0; i < 32; i++ ) {
        args[ i ] = calloc( 64, sizeof( *args[ i ] ) );
        MEM_SAFECHECK( args[ i ] );
    }

    // Argument count
    int argc = 0;
    // Set to true when an argument is found
    bool argfound = false;

    // Used to prevent infinite loops
    int attempt = 0;
    // Search loop
    while( !argfound ) {
        // Infinite loop prevention
        attempt++;
        if( attempt > 100 ) {
            printf( "too many attempts: 100\n" );
            exit( 1 );
        }
        // Temporary variable
        char* tmp = calloc( 128, sizeof( *tmp ) );
        MEM_SAFECHECK( tmp );
        // Used to denote the length of tmp later
        int len;
        // Used when assigning tmp characters as to not increment the pointer
        int i = 0;
        // Skip whitespace
        SKIP_WS( buffer );
        // Loop for valid characters (not spaces)
        while( _arg_validchar( *buffer ) ) {
            // Stop if null-terminator is found
            if( *buffer == 0 ) {
                argfound = true;
                break;
            }
            // Copy characters into tmp
            memset( ( tmp + i ), *buffer++, 1 );
            i++;
        }
        // Null-terminate the very end of the string.
        // i is pointing to the next position in the string
        // since it is incremented at the end of the loop
        memset( ( tmp + i ), '\0', 1 );
        len = strlen( tmp );
        memcpy( args[ argc ], tmp, len );
        argc++;
        free( tmp );
    }
    // Argument count output
    *l_argc = argc;
    return args;
}
static void _del_args( char** args ) {
    // Loop through sub-pointer pointers
    for( int i = 0; i < 32; i++ ) {
        free( args[ i ] );
    }
    // Free full parent pointer
    free( args );
}