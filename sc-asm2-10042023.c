#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<ctype.h>

const char* input = "lvr %r0, $4\nadd %r3, %r0, %r0\0";

const char* tok_e_str[ 16 ] = { "T_EOF", "T_PERCENT", "T_DOLLAR", "T_COMMA",
                            "T_RBRACKET", "T_LBRACKET", "T_RPAREN", "T_LPAREN",
                            "T_PLUS", "T_MINUS", "T_STAR", "T_SLASH", "T_TILDE",
                            "T_LITERAL", "T_LABEL", "T_LVR" };
#define NUM_REGS 16
const char* valid_regs[ NUM_REGS ] = { "r0", "r1", "r2", "r3", "r4",
                                        "r5", "r6", "r7", "r8", "r9",
                                        "r10", "r11", "r12", "r13", "r14", "r15", };

typedef struct {
    char* name;
    int id;
    int numargs_isp;
} instr_t;

instr_t instr[ 12 ] = {
    { "lvr", 0, 3 },
    { "add", 1, 5 },
    { "sub", 2, 5 },
    { "mul", 3, 5 },
    { "div", 4, 5 },
    { "lfp", 5, 3 },
    { "lpi", 6, 3 },
    { "lpo", 7, 3 },
    { "slv", 8, 3 },
    { "vts", 9, 1 },
    { "vfs", 10, 1 },
    { "com", 11, 3 },
};

typedef enum {
    /*  0 */ T_EOF,
    /*  1 */ T_PERCENT,
    /*  2 */ T_DOLLAR,
    /*  3 */ T_COMMA,
    /*  4 */ T_RBRACKET,
    /*  5 */ T_LBRACKET,
    /*  6 */ T_RPAREN,
    /*  7 */ T_LPAREN,
    /*  8 */ T_PLUS,
    /*  9 */ T_MINUS,
    /* 10 */ T_STAR,
    /* 11 */ T_SLASH,
    /* 12 */ T_TILDE,
    /* 13 */ T_NL,
    /* 14 */ T_LITERAL,
    /* 15 */ T_LABEL,
    /* 16 */ T_LVR,
    /* 17 */ T_ADD,
    /* 18 */ T_SUB,
    /* 19 */ T_MUL,
    /* 20 */ T_DIV,
    /* 21 */ T_LFP,
    /* 22 */ T_LPI,
    /* 23 */ T_LPO,
    /* 24 */ T_SLV,
    /* 25 */ T_VTS,
    /* 26 */ T_VFS,
    /* 27 */ T_COM
} tok_e;

struct {
    char* name;
} g_symtable[ 512 ];
int g_syms = 0;
struct {
    tok_e tok;
    char* str;
    long long val;
} g_token;
char g_putback = 0;

int my_atoi( char c ) {
    for( int i = 48; i < 57; i++ ) {
        if( c == i ) return i - 48;
    }
    return -1;
}

char next( void ) {
    if( g_putback ) {
        char c = g_putback;
        g_putback = 0;
        return c;
    }
    return *input++;
}
char skip( void ) {
    char c = next();
    while( c == ' ' || c == '\t' ) {
        c = next();
    }
    return c;
}
void putback( char c ) {
    g_putback = c;
}
int scan_int( char c ) {
    int val = 0;
    while( isdigit( c ) ) {
        val = val * 10 + my_atoi( c );
        c = next();
    }
    putback( c );
    return val;
}
int scan_sym( char c, char* buf, int lim ) {
    int i = 0;
    while( isalpha( c ) || isdigit( c ) || '_' == c ) {
        if( i == lim-1 ) {
            printf( "Identifier too long.\n" );
            exit( 1 );
        } else if( i < lim-1 ) {
            buf[ i++ ] = c;
        }
        c = next();
    }
    putback( c );
    buf[ i ] = 0;
    return i;
}
void validate_label( char* s ) {
    int len = strlen( s );
    if( s[ len-1 ] != ':' ) {
        printf( "Invalid label: '%s'\n", s );
        exit( 1 );
    }
}
int keyword( char* s ) {
    switch( *s ) {
        case 'a':
            if( strcmp( s, "add" ) == 0 ) return T_ADD;
            break;
        case 'c':
            if( strcmp( s, "com" ) == 0 ) return T_COM;
            break;
        case 'd':
            if( strcmp( s, "div" ) == 0 ) return T_DIV;
            break;
        case 'l':
            if( strcmp( s, "lfp" ) == 0 ) return T_LFP;
            if( strcmp( s, "lpi" ) == 0 ) return T_LPI;
            if( strcmp( s, "lpo" ) == 0 ) return T_LPO;
            if( strcmp( s, "lvr" ) == 0 ) return T_LVR;
            break;
        case 'm':
            if( strcmp( s, "mul" ) == 0 ) return T_MUL;
            break;
        case 's':
            if( strcmp( s, "slv" ) == 0 ) return T_SLV;
            break;
        case 'v':
            if( strcmp( s, "vfs" ) == 0 ) return T_VFS;
            if( strcmp( s, "vts" ) == 0 ) return T_VTS;
            break;
    }
    return 0;
}
int scan( void ) {
    char c = skip();
    //printf( "c = '%c'\n", c );
    switch( c ) {
        case 0:
            g_token.tok = T_EOF;
            return 0;
        case '%':
            g_token.tok = T_PERCENT;
            break;
        case '$':
            g_token.tok = T_DOLLAR;
            break;
        case ',':
            g_token.tok = T_COMMA;
            break;
        case '[':
            g_token.tok = T_RBRACKET;
            break;
        case ']':
            g_token.tok = T_LBRACKET;
            break;
        case '(':
            g_token.tok = T_RPAREN;
            break;
        case ')':
            g_token.tok = T_LPAREN;
            break;
        case '+':
            g_token.tok = T_PLUS;
            break;
        case '-':
            g_token.tok = T_MINUS;
            break;
        case '*':
            g_token.tok = T_STAR;
            break;
        case '/':
            g_token.tok = T_SLASH;
            break;
        case '~':
            g_token.tok = T_TILDE;
            break;
        case '\n':
            g_token.tok = T_NL;
            break;
        default:
            if( isalpha( c ) || c == '_' ) {
                char* tmp = calloc( 128, sizeof( *tmp ) );
                if( !tmp ) {
                    printf( "scan: tmp failed to allocate.\n" );
                    exit( 1 );
                }
                scan_sym( c, tmp, 128 );
                int tok_type = keyword( tmp );
                if( tok_type ) {
                    g_token.tok = tok_type;
                    break;
                }
                g_token.tok = T_LABEL;
                strcpy( tmp, g_token.str );
                validate_label( tmp );
                break;
            }
            printf( "Unknown character processed: '%c'\n", c );
            exit( 1 );
    }
    return 1;
}
int scan_reg( void ) {
    char* tmp = calloc( 8, sizeof( *tmp ) );
    if( !tmp ) {
        printf( "scan_reg: tmp failed to allocate.\n" );
        exit( 1 );
    }
    int tmp_i = 0;
    char c = next();
    while( isdigit( c ) || isalpha( c ) ) {
        tmp[ tmp_i++ ] = c;
        c = next();
    }
    putback( c );
    tmp[ tmp_i ] = 0;
    for( int i = 0; i < NUM_REGS; i++ ) {
        //printf( "comparing '%s' and '%s'\n", tmp, valid_regs[ i ] );
        if( strcmp( tmp, valid_regs[ i ] ) == 0 ) {
            return i;
        }
    }
    return -1;
}

int main( void ) {
    g_token.str = calloc( 128, sizeof( *g_token.str ) );
    if( !g_token.str ) {
        printf( "g_tok_str failed to allocate.\n" );
        exit( 1 );
    }
    int scan_result = scan();
    // 0 - instruction
    // 1 - first arg
    // 2 - comma (if applicable)
    // 3 - second arg
    // 4 - comma (if applicable)
    // 5 - third arg
    // instruction_syntax_place
    int isp = 0;
    int isp_max = 0;
    while( scan_result ) {
        if( isp > isp_max ) {
            printf( "isp reached max\n" );
            isp = 0;
            isp_max = 0;
            continue;
        }
        switch( g_token.tok ) {
            case T_PERCENT:
                int scan_reg_result = scan_reg();
                if( scan_reg_result < 0 ) {
                    printf( "Failed to find register.\n" );
                    exit( 1 );
                }
                printf( "found register %d (%s)\n", scan_reg_result, valid_regs[ scan_reg_result ] );
                isp++;
                break;
            case T_DOLLAR:
                g_token.val = scan_int( next() );
                break;
            case T_COMMA:
                if( isp == 2 || isp == 4 ) {
                    isp++;
                    break;
                }
                printf( "Comma found when not expected.\n" );
                exit( 1 );
            case T_NL:
                printf( "NL found\n" );
                break;
            default: // be sure to handle every other case because this will apply to instructions
                bool l_found = false;
                for( int i = 0; i < 12; i++ ) {
                    if( ( g_token.tok - 15 ) == instr[ i ].id ) {
                        isp_max = instr[ i ].numargs_isp;
                        isp++;
                        l_found = true;
                        break;
                    }
                }
                if( l_found ) break;
                printf( "Invalid instruction token found: %d\n", g_token.tok - 15 );
                exit( 1 );
        }
        printf( "g_token.tok = %d (%s)\n", g_token.tok, tok_e_str[ g_token.tok ] );
        printf( "g_token.val = %d\n", g_token.val );
        printf( "g_token.str = '%s'\n", g_token.str );
        printf( "end token\n\n" );
        scan_result = scan();
    }
    printf( "end scanning\n" );
    free( g_token.str );
    return 0;
}
