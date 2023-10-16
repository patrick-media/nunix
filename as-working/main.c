#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<ctype.h>

#define NUM_TOK_STR 60
const char* tok_e_str[ NUM_TOK_STR ] = { "T_EOF", "T_PERCENT", "T_DOLLAR", "T_COMMA",
                            "T_RBRACKET", "T_LBRACKET", "T_RPAREN", "T_LPAREN", "T_PLUS",
                            "T_MINUS", "T_STAR", "T_SLASH", "T_TILDE", "T_LITERAL",
                            "T_DFLITERAL", "T_LABEL", "T_LVR", "T_LVRX", "T_ADD", "T_ADDX",
                            "T_SUB", "T_SUBX", "T_MUL", "T_MULX", "T_DIV", "T_DIVX", "T_LFP",
                            "T_LFPX", "T_INT", "T_LPI", "T_LPIX", "T_LPO", "T_LPOX", "T_SLV",
                            "T_SLVX", "T_VTS", "T_VTSX", "T_VFS", "T_VFSX", "T_RET", "T_JMP",
                            "T_JPR", "T_JEQ", "T_JLS", "T_JGR", "T_JLE", "T_JGE", "T_JZO", "T_JCR",
                            "T_JNE", "T_COM", "T_COMX", "T_AND", "T_ANDX", "T_OR", "T_ORX", "T_XOR",
                            "T_XORX", "T_SFR", "T_SFRX" };
#define NUM_REGS 16
const char* valid_regs[ NUM_REGS ] = { "r0", "r1", "r2", "r3", "r4",
                                        "r5", "r6", "r7", "r8", "r9",
                                        "r10", "r11", "r12", "r13", "r14", "r15", };

typedef struct {
    char* name;
    int id;
    int numargs_isp;
} instr_t;

#define MAGIC_INSTR_START 16
#define NUM_INSTR 45
instr_t instr[ NUM_INSTR ] = {
    // 0 no args
    // 2 one arg
    // 4 two args
    // 6 three args
    { "lvr",  0, 4 },
    { "lvrx", 1, 4 },
    { "add",  2, 6 },
    { "addx", 3, 6 },
    { "sub",  4, 6 },
    { "subx", 5, 6 },
    { "mul",  6, 6 },
    { "mulx", 7, 6 },
    { "div",  8, 6 },
    { "divx", 9, 6 },
    { "lfp",  10, 4 },
    { "lfpx", 11, 4 },
    { "int",  12, 2 },
    { "lpi",  13, 4 },
    { "lpix", 14, 4 },
    { "lpo",  15, 4 },
    { "lpox", 16, 4 },
    { "slv",  17, 4 },
    { "slvx", 18, 4 },
    { "vts",  19, 2 },
    { "vtsx", 20, 2 },
    { "vfs",  21, 2 },
    { "vfsx", 22, 2 },
    { "ret",  23, 2 },
    { "jmp",  24, 2 },
    { "jpr",  25, 2 },
    { "jeq",  26, 2 },
    { "jls",  27, 2 },
    { "jgr",  28, 2 },
    { "jle",  29, 2 },
    { "jge",  30, 2 },
    { "jzo",  31, 2 },
    { "jcr",  32, 2 },
    { "jne",  33, 2 },
    { "com",  34, 4 },
    { "comx", 35, 4 },
    { "and",  36, 4 },
    { "andx", 37, 4 },
    { "or",   38, 4 },
    { "orx",  39, 4 },
    { "xor",  40, 4 },
    { "xorx", 41, 4 },
    { "sfr",  42, 4 },
    { "sfrx", 43, 4 }
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
    /* 14 */ T_LITERALDF,
    /* 15 */ T_LABEL,
    /* 16 */ T_LVR,
    /* 17 */ T_LVRX,
    /* 18 */ T_ADD,
    /* 19 */ T_ADDX,
    /* 20 */ T_SUB,
    /* 21 */ T_SUBX,
    /* 22 */ T_MUL,
    /* 23 */ T_MULX,
    /* 24 */ T_DIV,
    /* 25 */ T_DIVX,
    /* 26 */ T_LFP,
    /* 27 */ T_LFPX,
    /* 28 */ T_INT,
    /* 29 */ T_LPI,
    /* 30 */ T_LPIX,
    /* 31 */ T_LPO,
    /* 32 */ T_LPOX,
    /* 33 */ T_SLV,
    /* 34 */ T_SLVX,
    /* 35 */ T_VTS,
    /* 36 */ T_VTSX,
    /* 37 */ T_VFS,
    /* 38 */ T_VFSX,
    /* 39 */ T_RET,
    /* 40 */ T_JMP,
    /* 41 */ T_JPR,
    /* 42 */ T_JEQ,
    /* 43 */ T_JLS,
    /* 44 */ T_JGR,
    /* 45 */ T_JLE,
    /* 46 */ T_JGE,
    /* 47 */ T_JZO,
    /* 48 */ T_JCR,
    /* 49 */ T_JNE,
    /* 50 */ T_COM,
    /* 51 */ T_COMX,
    /* 52 */ T_AND,
    /* 53 */ T_ANDX,
    /* 54 */ T_OR,
    /* 55 */ T_ORX,
    /* 56 */ T_XOR,
    /* 57 */ T_XORX,
    /* 58 */ T_SFR,
    /* 59 */ T_SFRX
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
            if( strcmp( s, "addx" ) == 0 ) {
                //printf( "instruction = addx\n" );
                return T_ADDX;
            }
            if( strcmp( s, "and" ) == 0 ) {
                //printf( "instruction = and\n" );
                return T_AND;
            }
            if( strcmp( s, "andx" ) == 0 ) {
                //printf( "instruction = andx\n" );
                return T_ANDX;
            }
            break;
        case 'c':
            if( strcmp( s, "com" ) == 0 ) {
                //printf( "instruction = com\n" );
                return T_COM;
            }
            if( strcmp( s, "comx" ) == 0 ) {
                //printf( "instruction = comx\n" );
                return T_COMX;
            }
            break;
        case 'd':
            if( strcmp( s, "div" ) == 0 ) {
                //printf( "instruction = div\n" );
                return T_DIV;
            }
            if( strcmp( s, "divx" ) == 0 ) {
                //printf( "instruction = divx\n" );
                return T_DIVX;
            }
            break;
        case 'i':
            if( strcmp( s, "int" ) == 0 ) {
                //printf( "instruction = int\n" );
                return T_INT;
            }
            break;
        case 'j':
            if( strcmp( s, "jmp" ) == 0 ) {
                //printf( "instruction = jmp\n" );
                return T_JMP;
            }
            if( strcmp( s, "jpr" ) == 0 ) {
                //printf( "instruction = jpr\n" );
                return T_JPR;
            }
            if( strcmp( s, "jeq" ) == 0 ) {
                //printf( "instruction = jeq\n" );
                return T_JEQ;
            }
            if( strcmp( s, "jls" ) == 0 ) {
                //printf( "instruction = jls\n" );
                return T_JLS;
            }
            if( strcmp( s, "jgr" ) == 0 ) {
                //printf( "instruction = jgr\n" );
                return T_JGR;
            }
            if( strcmp( s, "jle" ) == 0 ) {
                //printf( "instruction = jle\n" );
                return T_JLE;
            }
            if( strcmp( s, "jge" ) == 0 ) {
                //printf( "instruction = jge\n" );
                return T_JGE;
            }
            if( strcmp( s, "jzo" ) == 0 ) {
                //printf( "instruction = jzo\n" );
                return T_JZO;
            }
            if( strcmp( s, "jcr" ) == 0 ) {
                //printf( "instruction = jcr\n" );
                return T_JCR;
            }
            if( strcmp( s, "jne" ) == 0 ) {
                //printf( "instruction = jne\n" );
                return T_JNE;
            }
            break;
        case 'l':
            if( strcmp( s, "lfp" ) == 0 ) {
                //printf( "instruction = lpf\n" );
                return T_LFP;
            }
            if( strcmp( s, "lfpx" ) == 0 ) {
                //printf( "instruction = lpfx\n" );
                return T_LFPX;
            }
            if( strcmp( s, "lpi" ) == 0 ) {
                //printf( "instruction = lpi\n" );
                return T_LPI;
            }
            if( strcmp( s, "lpix" ) == 0 ) {
                //printf( "instruction = lpix\n" );
                return T_LPIX;
            }
            if( strcmp( s, "lpo" ) == 0 ) {
                //printf( "instruction = lpo\n" );
                return T_LPO;
            }
            if( strcmp( s, "lpox" ) == 0 ) {
                //printf( "instruction = lpox\n" );
                return T_LPOX;
            }
            if( strcmp( s, "lvr" ) == 0 ) {
                //printf( "instruction = lvr\n" );
                return T_LVR;
            }
            if( strcmp( s, "lvrx" ) == 0 ) {
                //printf( "instruction = lvrx\n" );
                return T_LVRX;
            }
            break;
        case 'm':
            if( strcmp( s, "mul" ) == 0 ) {
                //printf( "instruction = mul\n" );
                return T_MUL;
            }
            if( strcmp( s, "mulx" ) == 0 ) {
                //printf( "instruction = mulx\n" );
                return T_MULX;
            }
            break;
        case 'o':
            if( strcmp( s, "or" ) == 0 ) {
                //printf( "instruction = or\n" );
                return T_OR;
            }
            if( strcmp( s, "orx" ) == 0 ) {
                //printf( "instruction = orx\n" );
                return T_ORX;
            }
            break;
        case 'r':
            if( strcmp( s, "ret" ) == 0 ) {
                //printf( "instruction = ret\n" );
                return T_RET;
            }
            break;
        case 's':
            if( strcmp( s, "sfr" ) == 0 ) {
                //printf( "instruction = sfr\n" );
                return T_SFR;
            }
            if( strcmp( s, "sfrx" ) == 0 ) {
                //printf( "instruction = sfrx\n" );
                return T_SFRX;
            }
            if( strcmp( s, "slv" ) == 0 ) {
                //printf( "instruction = slv\n" );
                return T_SLV;
            }
            if( strcmp( s, "slvx" ) == 0 ) {
                //printf( "instruction = slvx\n" );
                return T_SLVX;
            }
            break;
        case 'v':
            if( strcmp( s, "vfs" ) == 0 ) {
                //printf( "instruction = vfs\n" );
                return T_VFS;
            }
            if( strcmp( s, "vfsx" ) == 0 ) {
                //printf( "instruction = vfsx\n" );
                return T_VFSX;
            }
            if( strcmp( s, "vts" ) == 0 ) {
                //printf( "instruction = vts\n" );
                return T_VTS;
            }
            if( strcmp( s, "vtsx" ) == 0 ) {
                //printf( "instruction = vtsx\n" );
                return T_VTSX;
            }
            break;
        case 'x':
            if( strcmp( s, "xor" ) == 0 ) {
                //printf( "instruction = xor\n" );
                return T_XOR;
            }
            if( strcmp( s, "xorx" ) == 0 ) {
                //printf( "instruction = xorx\n" );
                return T_XORX;
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
            g_token.tok = T_LPAREN;
            break;
        case ')':
            g_token.tok = T_RPAREN;
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
        bool print_value = false;
        int print_value_type = 0;
        printf( "isp = %d isp_max = %d\n", isp, isp_max );
        if( isp == isp_max ) {
            printf( "isp reached max\n" );
            isp = 0;
            isp_max = -1;
            continue;
        }
        char c;
        switch( g_token.tok ) {
            case T_PERCENT:
                int scan_reg_result = scan_reg();
                if( scan_reg_result < 0 ) {
                    printf( "Failed to find register.\n" );
                    exit( 1 );
                }
                //printf( "found register %d (%s)\n", scan_reg_result, valid_regs[ scan_reg_result ] );
                printf( "%%%s\n", valid_regs[ scan_reg_result ] );
                isp++;
                break;
            case T_DOLLAR:
                print_value = true;
                print_value_type = 1; // print int lit
                g_token.val = scan_int( next() );
                isp++;
                break;
            case T_COMMA:
                if( isp == 2 || isp == 4 ) {
                    isp++;
                    break;
                }
                printf( "Comma found when not expected. isp = %d, isp_max = %d\n", isp, isp_max );
                exit( 1 );
            case T_TILDE:
                c = next();
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
            case T_LPAREN:
                print_value = true;
                print_value_type = 2; // print df lit
                c = next();
                if( c == '$' ) g_token.val = scan_int( next() );
                else {
                    if( scan_reg() < 0 ) {
                        printf( "Failed to find register.\n" );
                        exit( 1 );
                    }
                }
                c = next();
                if( c != ')' ) {
                    printf( "Invalid dereference token processed: '%c'\n", c );
                    exit( 1 );
                }
                isp++;
                break;
            default: // be sure to handle every other case because this will apply to instructions
                bool l_found = false;
                for( int i = 0; i < NUM_INSTR-1; i++ ) {
                    if( ( g_token.tok - MAGIC_INSTR_START ) == instr[ i ].id ) {
                        //printf( "accessing instruction %d '%s'...\n", i, instr[ i ].name );
                        isp_max = instr[ i ].numargs_isp;
                        isp++;
                        l_found = true;
                        break;
                    }
                }
                if( l_found ) break;
                printf( "Invalid instruction token found: %d\n", g_token.tok - MAGIC_INSTR_START );
                exit( 1 );
        }
        printf( "%s\n", tok_e_str[ g_token.tok ] );
        //printf( "isp = %d\n", isp );
        //printf( "isp_max = %d\n", isp_max );
        if( print_value ) {
            if( print_value_type == 1 ) printf( "$%d\n", g_token.val );
            else if( print_value_type == 2 ) printf( "0x%x\n", g_token.val );
            else {
                printf( "Error attempting to print int value: %d | 0x%x\n", g_token.val, g_token.val );
                exit( 1 );
            }
        }
        //printf( "end token\n\n" );
        scan_result = scan();
    }
    printf( "SYMTABLE:\n" );
    for( int i = 0; i < g_syms; i++ ) {
        printf( "symtable entry %d: '%s'\n", i, g_symtable[ i ].name );
    }
    printf( "\n\n" );
    return 0;
}
