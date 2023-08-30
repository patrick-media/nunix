#define INTERNAL32
#include"asm32.h"

static kwds_t kwds[] = {
	{ I_NOP, "nop" },
	{ I_MOV, "mov" },
	{ I_LDM, "ldm" },
	{ I_STR, "str" },
	{ I_ADD, "add" },
	{ I_SUB, "sub" },
	{ I_MUL, "mul" },
	{ I_DIV, "div" },
	{ I_SHL, "shl" },
	{ I_SHR, "shr" },
	{ I_ROR, "ror" },
	{ I_AND, "and" },
	{ I_OR, "or" },
	{ I_NOR, "nor" },
	{ I_XOR, "xor" },
	{ I_NOT, "not" },
	{ I_PUSH, "push" },
	{ I_POP, "pop" },
	{ I_PUSHA, "pusha" },
	{ I_POPA, "popa" },
	{ I_IN, "in" },
	{ I_OUT, "out" },
	{ I_B, "b" },
	{ I_BR, "br" },
	{ I_RTB, "rtb" }
};

void skip( void );

const char* statement1 = "4 +  5- 9";

char* g_buffer = 0;
token_t g_token = 0;

int asm32( char* filename_in, char* filename_out ) {
	//g_buffer = calloc( 32, sizeof( *g_buffer ) );
	g_token.string = calloc( 16, sizeof( *g_token.string ) );
	g_buffer = statement1;
	
	skip();

	int typefound = scan();
	printf( "typefound = %d\n", typefound );

	//free( g_buffer );
	free( g_token.string );
	return 0;
}
void skip( void ) {
	while( *g_buffer == ' ' || *g_buffer == '\n' ) {
		// skip whitespace / newlines
		( *g_buffer )++;
		if( *g_buffer == 0 ) {
			// end of file
		}
	}
}
int scan( void ) {
	bool found = false;
	while( !found ) {
		switch( *g_buffer ) {
		case '+':
			return T_ADD;
		}
	}
}