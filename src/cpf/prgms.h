#ifndef _NX_PRGMS_H
#define _NX_PRGMS_H

#include"../com.h"

// Number of programs
#define _NX_PROGRAMS 6

// Program pre-definitions
int32_t cpf_end( int argc, char** argv );
int32_t cpf_help( int argc, char** argv );
int32_t cpf_ver( int argc, char** argv );
int32_t cpf_mkfile( int argc, char** argv );
int32_t cpf_fstat( int argc, char** argv );
int32_t cpf_rm( int argc, char** argv );

// Program List definitions
/*
extern const nx_program_t p_end;
extern const nx_program_t p_help;
extern const nx_program_t p_ver;
extern const nx_program_t p_mkfile;
*/

#endif // _NX_PRGMS_H