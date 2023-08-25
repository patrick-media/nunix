/*

NUNIX Edition A
8-24-2023

*/
#define NX_VERSION ( const char* )"0001"

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>

/* NUNIX Header Definitions */
typedef char            INT8;
typedef unsigned char   UINT8;
typedef short           INT16;
typedef unsigned short  UINT16;
typedef int             INT32;
typedef unsigned int    UINT32;
typedef long            INT64;
typedef unsigned long   UINT64;

typedef char            CHAR;
typedef bool            BOOL;
typedef void            VOID;

#define CONST const
#define TRUE true
#define FALSE false

#define IN
#define OUT
#define OPTIONAL

#define NX_SUCCESS 0
#define NX_ERROR 1

typedef struct {
    UINT32 id;
    CHAR* cmd;
    INT32 ( *exec )( void );
    BOOL valid;
} nx_program_t;

typedef struct {
    nx_program_t *entries;
    UINT32 count;
} nx_program_list_t;

typedef struct {
    BOOL run;
    INT32 err;
} nx_state_t;
/* End NUNIX Header Definition */

/* Internal Function Prototypes */
INT32 cpf_end( void );
INT32 cpf_help( void );
INT32 cpf_ver( void );
INT32 cpf_test_print( void );

/* Programs */
nx_program_t p_end = { 0, "end", cpf_end, TRUE };
nx_program_t p_help = { 1, "help", cpf_help, TRUE };
nx_program_t p_ver = { 2, "ver", cpf_ver, TRUE };
nx_program_t p_test_print = { 3, "test_print", cpf_test_print, TRUE };

/* Internal Functions */
static inline VOID* nx_sizeof_array( VOID* arr );

/* System-wide Variables and Tables */
nx_state_t g_state = { 0 };
nx_program_list_t g_program_list = {
    .entries = {
        &p_end,
        &p_help,
        &p_ver,
        &p_test_print
    },
    .count = 4
};

// Input buffer for scanf();
CHAR* in_buffer = 0;

int main( void ) {
    /* Memory Allocation */
    in_buffer = calloc( 128, sizeof( *in_buffer ) );
    
    /* Variable Assignment */
    g_state.run = TRUE;
    
    /* Introductory Statements */
    // TODO add beginning words
    
    /* Main Loop */
    while( g_state.run ) {
        // Set to TRUE if a command is found.
        BOOL cmd_found = false;
        // Marker that input begins here.
        printf( "> " );
        // Put input into in_buffer. \n at the beginning flushes stdout.
        scanf( "\n%[^\n]s", in_buffer );
        // Iterate through programs.
        for( int i = 0; i < g_program_list.count; i++ ) {
            // Stop if it's not valid.
            if( g_program_list.entries[ i ].valid ) {
                // Check for the program command.
                if( strcmp( g_program_list.entries[ i ].cmd, in_buffer ) == 0 ) {
                    // Command found
                    cmd_found = true;
                    // Result - used for error checking
                    INT32 lp_result = g_program_list.entries[ i ].exec();
                    // Error check - if program returns anything but NX_SUCCESS
                    if( lp_result != NX_SUCCESS ) {
                        printf( "Program '%s' exited with a return value of %d.\n", g_program_list.entries[ i ].cmd, lp_result );
                    }
                }
            }
        }
        // Error check
        if( !cmd_found ) {
            printf( "Command not found.\n" );
        }
        // Reset in_buffer
        memset( in_buffer, 0, 1 );
    }
    
    // Exit program
    return NX_SUCCESS;
}

/* Internal Functions */
void nx_print( char* fmt, ... ) {
    void* args = ( void* )&fmt;
    printf( "DEBUG args = %x\n", args );
    printf( "DEBUG *args = %s\n", *( INT64* )args );
    args+=1;
    //args += sizeof( args[ 0 ] ) / sizeof( *args );
    printf( "DEBUG args = %x\n", args );
    printf( "DEBUG *args = %d\n", *( INT64* )args );
    printf( "DEBUG &args = %x\n", &args );
}
static inline VOID* nx_sizeof_array( VOID* arr ) {
    return ( VOID* )( *( &arr + 1 ) - arr );
}

/* Corresponding Program Functions */
INT32 cpf_end( void ) {
    printf( "Halting...\n" );
    g_state.run = FALSE;
    return NX_SUCCESS;
}
INT32 cpf_help( void ) {
    printf( "Help coming soon.\n" );
    return NX_SUCCESS;
}
INT32 cpf_ver( void ) {
    printf( "NUNIX Edition A version %s\n", NX_VERSION );
    return NX_SUCCESS;
}
INT32 cpf_test_print( void ) {
    nx_print( "test", ( INT64 )33, ( INT64 )65, ( INT64 )65, ( INT64 )66 );
    return NX_SUCCESS;
}
