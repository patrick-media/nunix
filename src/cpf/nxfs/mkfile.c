#include"../prgms.h"
#include"../../fs/fs.h"
uint32_t test_block = 4;
int32_t cpf_mkfile( int argc, char** argv ) {
    if( argc != 2 ) {
        SYS_nonfatal( "incorrect number of arguments.\n" );
        return NX_ERROR;
    }
    unode_t* test = nxfs_register_file( FD_BIN, argv[ 1 ], 0, 1, &test_block );
    printf( "Created file with name '%s'.\n", test->fd_name );
    return NX_SUCCESS;
}