#include"../prgms.h"
int32_t cpf_ver( int argc, char** argv ) {
    if( argc > 1 ) {
        SYS_nonfatal( "invalid number of arguments.\n" );
        return NX_ERROR;
    }
    printf( "NUNIX Edition A version %s\n", NX_VERSION );
    return NX_SUCCESS;
}