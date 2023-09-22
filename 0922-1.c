/*

NUNIX Edition A
8-24-2023

*/
#define NX_VERSION ( const char* )"0003"

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<stdint.h>
#include<stdarg.h>

/* NUNIX Header Definitions */

#define NX_SUCCESS 0
#define NX_ERROR 1

typedef struct {
    uint32_t id;
    char* cmd;
    int32_t ( *exec )( void );
    bool valid;
} nx_program_t;

typedef struct {
    nx_program_t *entries;
    uint32_t count;
} nx_program_list_t;

typedef struct {
    bool run;
    int32_t err;
} nx_state_t;

// ADD FS
/* End NUNIX Header Definition */

/* Internal Function Prototypes */
int32_t cpf_end( void );
int32_t cpf_help( void );
int32_t cpf_ver( void );
int32_t cpf_freg( void );
//INT32 cpf_test_print( void );

/* Programs */
#define _NX_PROGRAMS 4
nx_program_t p_end = { 0, "end", cpf_end, true };
nx_program_t p_help = { 1, "help", cpf_help, true };
nx_program_t p_ver = { 2, "ver", cpf_ver, true };
nx_program_t p_freg = { 3, "freg", cpf_freg, true };

/* Internal Functions */
static inline void* nx_sizeof_array( void* arr );

/* System-wide Variables and Tables */
nx_state_t g_state = { 0 };
nx_program_list_t g_program_list = {
    .entries = {
        &p_end,
        &p_help,
        &p_ver,
        &p_freg
    },
    .count = _NX_PROGRAMS
};

// Input buffer for scanf();
char* in_buffer = 0;

int main( void ) {
    /* Memory Allocation */
    in_buffer = calloc( 128, sizeof( *in_buffer ) );
    
    /* Variable Assignment */
    g_state.run = true;
    
    /* Introductory Statements */
    // TODO add beginning words
    
    /* Main Loop */
    while( g_state.run ) {
        // Set to TRUE if a command is found.
        bool cmd_found = false;
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
                    int32_t lp_result = g_program_list.entries[ i ].exec();
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
static inline void* nx_sizeof_array( void* arr ) {
    return ( void* )( *( &arr + 1 ) - arr );
}

/* Corresponding Program Functions */
int32_t cpf_end( void ) {
    printf( "Halting...\n" );
    g_state.run = false;
    return NX_SUCCESS;
}
int32_t cpf_help( void ) {
    printf( "Help coming soon.\n" );
    return NX_SUCCESS;
}
int32_t cpf_ver( void ) {
    printf( "NUNIX Edition A version %s\n", NX_VERSION );
    return NX_SUCCESS;
}
int32_t cpf_freg( void ) {
    printf( "Not implemented.\n" );
    return NX_SUCCESS;
}
