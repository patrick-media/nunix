#include"../prgms.h"
int32_t cpf_end( int argc, char** argv ) {
    if( argc > 1 ) {
        SYS_nonfatal( "invalid number of arguments.\n" );
        return NX_ERROR;
    }
    printf( "Halting...\n" );
    g_state.run = false;
    return NX_SUCCESS;
}