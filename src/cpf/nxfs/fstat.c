#include"../prgms.h"
#include"../../fs/fs.h"
int32_t cpf_fstat( int argc, char** argv ) {
    if( argc != 2 ) {
        SYS_nonfatal( "incorrect number of arguments.\n" );
        return NX_ERROR;
    }
    unode_t* file = 0;
    bool found = false;
    for( int i = 0; i < g_fs_sz; i++ ) {
        if( g_fs[ i ].fd_type != FD_NULL ) {
            if( strcmp( argv[ 1 ], g_fs[ i ].fd_name ) == 0 ) {
                file = &g_fs[ i ];
                found = true;
            }
        }
    }
    if( !found ) {
        SYS_nonfatal( "failed to find a file with the name '%s'.\n", argv[ 1 ] );
        return NX_SUCCESS;
    }
    MEM_SAFECHECK( file );
    nxfs_file_stat( file );
    return NX_SUCCESS;
}