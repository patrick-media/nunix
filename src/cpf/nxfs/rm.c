#include"../prgms.h"
#include"../../fs/fs.h"
int32_t cpf_rm( int argc, char** argv ) {
    if( argc != 2 ) {
        SYS_nonfatal( "incorrect number of arguments.\n" );
        return NX_ERROR;
    }
    unode_t* file = nxfs_file_search( argv[ 1 ] );
    if( !file ) {
        SYS_nonfatal( "failed to find file '%s'.\n", argv[ 1 ] );
        return NX_ERROR;
    }
    nxfs_unregister_file( file );
    printf( "Successfully removed '%s'.\n", argv[ 1 ] );
    return NX_SUCCESS;
}