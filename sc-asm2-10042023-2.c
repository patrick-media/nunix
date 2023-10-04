#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<ctype.h>

#define NUM_INSTR 27
const char* tok_e_str[ NUM_INSTR ] = { "T_EOF", "T_PERCENT", "T_DOLLAR", "T_COMMA",
                            "T_RBRACKET", "T_LBRACKET", "T_RPAREN", "T_LPAREN",
                            "T_PLUS", "T_MINUS", "T_STAR", "T_SLASH", "T_TILDE",
                            "T_LITERAL", "T_LABEL", "T_LVR", "T_ADD", "T_SUB", "T_MUL",
                            "T_DIV", "T_LFP", "T_LPI", "T_LPO", "T_SLV", "T_VTS", "T_VFS", "T_COM" };
#define NUM_REGS 16
const char* valid_regs[ NUM_REGS ] = { "r0", "r1", "r2", "r3", "r4",
                                        "r5", "r6", "r7", "r8", "r9",
                                        "r10", "r11", "r12", "r13", "r14", "r15", };

typedef struct {
    char* name;
    int id;
    int numargs_isp;
} instr_t;

instr_t instr[ 13 ] = {
    { "nop", 0, 0 },
    { "lvr", 1, 3 },
    { "add", 2, 5 },
    { "sub", 3, 5 },
    { "mul", 4, 5 },
    { "div", 5, 5 },
    { "lfp", 6, 3 },
    { "lpi", 7, 3 },
    { "lpo", 8, 3 },
    { "slv", 9, 3 },
    { "vts", 10, 1 },
    { "vfs", 11, 1 },
    { "com", 12, 3 },
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
    /* 13 */ T_LITERAL,
    /* 14 */ T_LABEL,
    /* 15 */ T_LVR,
    /* 16 */ T_ADD,
    /* 17 */ T_SUB,
    /* 18 */ T_MUL,
    /* 19 */ T_DIV,
    /* 20 */ T_LFP,
    /* 21 */ T_LPI,
    /* 22 */ T_LPO,
    /* 23 */ T_SLV,
    /* 24 */ T_VTS,
    /* 25 */ T_VFS,
    /* 26 */ T_COM
} tok_e;

#define G_SYMTABLE_MAX 512
struct {
    char* name;
} g_symtable[ G_SYMTABLE_MAX ];
int g_syms = 0;
struct {
    tok_e tok;
    long long val;
} g_token;
char g_putback = 0;

char* g_input;

int my_atoi( char c ) {
    for( int i = 48; i < 57; i++ ) {
        if( c == i ) return i - 48;
    }
    return -1;
}

int findsym( char* s ) {
    for( int i = 0; i < g_syms; i++ ) {
        if( strcmp( s, g_symtable[ i ].name ) == 0 ) {
            return i;
        }
    }
    return -1;
}
int addsym( char* name ) {
    int y;
    y = findsym( name );
    if( y != -1 ) return y;
    y = g_syms++;
    if( y >= G_SYMTABLE_MAX ) {
        printf( "Too many label symbols.\n" );
        exit( 1 );
    }
    g_symtable[ y ].name = strdup( name );
    return y;
}

char next( void ) {
    if( g_putback ) {
        char c = g_putback;
        g_putback = 0;
        return c;
    }
    return *g_input++;
}
char skip( void ) {
    char c = next();
    while( c == ' ' || c == '\t' || c == '\n' ) {
        c = next();
    }
    return c;
}
void putback( char c ) {
    g_putback = c;
}
int scan_int( char c ) {
    int val = 0;
    char* tmp = calloc( 64, sizeof( *tmp ) );
    if( !tmp ) {
        printf( "scan_int: tmp failed to allocate.\n" );
        exit( 1 );
    }
    int tmp_i = 0;
    while( isdigit( c ) || isxdigit( c ) || c == 'x' ) {
        tmp[ tmp_i++ ] = c;
        c = next();
    }
    tmp[ tmp_i ] = 0;
    if( tmp[ 0 ] == '0' && tmp[ 1 ] == 'x' ) {
        val = strtol( tmp, NULL, 16 );
    } else {
        val = strtol( tmp, NULL, 10 );
    }
    /*
    if( c == '0' ) {
        c = next();
        if( c == 'x' ) {
            if( isxdigit( c ) ) {
                strtol( &c, NULL, 16 );
            }
        }
        if( !isdigit( c ) ) {
            printf( "Invalid hexadecimal number format: %c\n", c );
            exit( 1 );
        }
    }
    while( isdigit( c ) ) {
        val = val * 10 + my_atoi( c );
        c = next();
    }
    */
    putback( c );
    return val;
}
int scan_sym( char c, char* buf, int lim ) {
    int i = 0;
    while( isalpha( c ) || isdigit( c ) || c == '_' || c == ':' ) {
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
    s[ len-1 ] = 0;
    addsym( s );
}
int keyword( char* s ) {
    switch( *s ) {
        case 'a':
            if( strcmp( s, "add" ) == 0 ) {
                //printf( "instruction = add\n" );
                return T_ADD;
            }
            break;
        case 'c':
            if( strcmp( s, "com" ) == 0 ) {
                //printf( "instruction = com\n" );
                return T_COM;
            }
            break;
        case 'd':
            if( strcmp( s, "div" ) == 0 ) {
                //printf( "instruction = div\n" );
                return T_DIV;
            }
            break;
        case 'l':
            if( strcmp( s, "lfp" ) == 0 ) {
                //printf( "instruction = lpf\n" );
                return T_LFP;
            }
            if( strcmp( s, "lpi" ) == 0 ) {
                //printf( "instruction = lpi\n" );
                return T_LPI;
            }
            if( strcmp( s, "lpo" ) == 0 ) {
                //printf( "instruction = lpo\n" );
                return T_LPO;
            }
            if( strcmp( s, "lvr" ) == 0 ) {
                //printf( "instruction = lvr\n" );
                return T_LVR;
            }
            break;
        case 'm':
            if( strcmp( s, "mul" ) == 0 ) {
                //printf( "instruction = mul\n" );
                return T_MUL;
            }
            break;
        case 's':
            if( strcmp( s, "slv" ) == 0 ) {
                //printf( "instruction = slv\n" );
                return T_SLV;
            }
            break;
        case 'v':
            if( strcmp( s, "vfs" ) == 0 ) {
                //printf( "instruction = vfs\n" );
                return T_VFS;
            }
            if( strcmp( s, "vts" ) == 0 ) {
                //printf( "instruction = vts\n" );
                return T_VTS;
            }
            break;
    }
    return 0;
}
int scan( void ) {
    char c = skip();
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
        default:
            if( isalpha( c ) || c == '_' ) {
                char* tmp = calloc( 128, sizeof( *tmp ) );
                if( !tmp ) {
                    exit( 1 );
                }
                scan_sym( c, tmp, 128 );
                int tok_type = keyword( tmp );
                if( tok_type ) {
                    g_token.tok = tok_type;
                    break;
                }
                g_token.tok = T_LABEL;
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

int main( int argc, char** argv ) {
    if( argc != 2 ) {
        printf( "Invalid number of arguments: %d\n", argc );
        exit( 1 );
    }
    FILE* fp = fopen( argv[ 1 ], "r" );
    if( !fp ) {
        printf( "Failed to open file '%s'.\n", argv[ 1 ] );
        exit( 1 );
    }
    g_input = calloc( 4096, sizeof( *g_input ) );
    if( !g_input ) {
        printf( "input failed to allocate.\n" );
        exit( 1 );
    }
    char file_contents = fgetc( fp );
    int input_i = 0;
    while( file_contents != EOF ) {
        g_input[ input_i++ ] = file_contents;
        file_contents = fgetc( fp );
    }
    g_input[ input_i ] = 0;
    fclose( fp );

    int scan_result = scan();
    // 0 - instruction
    // 1 - first arg
    // 2 - comma (if applicable)
    // 3 - second arg
    // 4 - comma (if applicable)
    // 5 - third arg
    // instruction_syntax_place
    int isp = 0;
    int isp_max = -1;
    while( scan_result ) {
        if( isp == isp_max ) {
            printf( "isp reached max\n" );
            isp = 0;
            isp_max = -1;
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
                printf( "Comma found when not expected. isp = %d, isp_max = %d\n", isp, isp_max );
                exit( 1 );
            case T_TILDE:
                char c = next();
                if( c != '~' ) {
                    printf( "Invalid comment: '~~' expected, found '%c' as second character.\n", c );
                    exit( 1 );
                }
                while( c && c != '\n' ) {
                    c = next();
                }
                putback( c );
                break;
            case T_LABEL:
                break;
            default: // be sure to handle every other case because this will apply to instructions
                bool l_found = false;
                for( int i = 0; i < 12; i++ ) {
                    if( ( g_token.tok - 14 ) == instr[ i ].id ) {
                        //printf( "accessing instruction %d '%s'...\n", i, instr[ i ].name );
                        isp_max = instr[ i ].numargs_isp;
                        isp++;
                        l_found = true;
                        break;
                    }
                }
                if( l_found ) break;
                printf( "Invalid instruction token found: %d\n", g_token.tok - 14 );
                exit( 1 );
        }
        printf( "g_token.tok = %d (%s)\n", g_token.tok, tok_e_str[ g_token.tok ] );
        printf( "g_token.val = %d\n", g_token.val );
        printf( "end token\n\n" );
        scan_result = scan();
    }
    printf( "end scanning\n\n" );
    printf( "SYMTABLE:\n" );
    for( int i = 0; i < g_syms; i++ ) {
        printf( "symtable entry %d: '%s'\n", i, g_symtable[ i ].name );
    }
    printf( "\n\n" );
    return 0;
}
