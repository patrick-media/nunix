#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<ctype.h>
#include<string.h>

const char T_OPS[ 6 ] = { 'E', '+', '-', '*', '/', 'L' };
#define FMT_I   0x1
#define FMT_R   0x2
#define FMT_RI  0x3
#define FMT_IR  0x4
#define FMT_FI  0x5
#define FMT_FR  0x6
#define FMT_FRI 0x7
#define FMT_FIR 0x8
#define FMT_RDF 0x9
//const int T_INSTR_FMT[ 10 ] = { 0, FMT_I, FMT_R, FMT_RI, FMT_IR, FMT_FI, FMT_FR, FMT_FRI, FMT_FIR, FMT_RDF };
typedef struct {
    enum {
        T_EOF = 0,
        T_PLUS,     // 1
        T_MINUS,    // 2
        T_STAR,     // 3
        T_SLASH,    // 4
        T_LIT,      // 5
        T_NL,       // 6
        T_PERCENT,  // 7
        T_DOLLAR,   // 8
        T_COMMA,    // 9
        T_INSTR_LVR // 10
    } type;
    int val;
} token_t;
typedef struct tnode_s tnode_t;
typedef struct tnode_s {
    enum {
        OP_EOF = 0,
        OP_ADD,
        OP_SUB,
        OP_MUL,
        OP_DIV,
        OP_LIT,
        OP_REG,
        OP_LVR  // 7
    } op;
    tnode_t* lvalue;
    tnode_t* rvalue;
    long long intvalue;
} tnode_t;

//const char* input = "2+3*5-8/3\0";
//const char* input = "2 + 3 * 5 - 8 / 3\0";
//const char* input = "13 -6+  4*\n5\n+\n08 / 3\0";
const char* input = "lvr %r0, $4\0";
token_t g_token = { 0 };
char g_putback = 0;
int g_op_prec[] = { 0, 10, 10, 20, 20, 0 };
char g_text[ 512 ];

int next( void ) {
    if( g_putback ) {
        char c = g_putback;
        g_putback = 0;
        return c;
    }
    return *input++;
}
int skip( void ) {
    char c = next();
    while( c == ' ' || c == '\t' ) {
        c = next();
    }
    return c;
}
int my_atoi( char c ) {
    return c - 48;
    for( int i = 57; i < 122; i++ ) {
        if( c == i ) return i - 48;
    }
    return -1;
}
int scankw( char c, char* buf, int len ) {
    int i = 0;
    while( isalpha( c ) || isdigit( c ) || c == '_' ) {
        if( i == len-1 ) {
            printf( "Keyword too long.\n" );
            exit( 1 );
        } else {
            buf[ i++ ] = c;
        }
        c = next();
    }
    g_putback = c;
    buf[ i ] = 0;
    return i;
}
static int kw( char* s ) {
    switch( *s ) {
        case 'l':
        case 'L':
            if( strcmp( s, "lvr" ) == 0 || strcmp( s, "LVR" ) ) {
                return T_INSTR_LVR;
            }
    }
}
int scan( token_t* token ) {
    int c = skip();
    //printf( "c = '%c'\n", c );
    switch( c ) {
        case '\0':
            token->type = T_EOF;
            return 1;
        case '+':
            token->type = T_PLUS;
            return 1;
        case '-':
            token->type = T_MINUS;
            return 1;
        case '*':
            token->type = T_STAR;
            return 1;
        case '/':
            token->type = T_SLASH;
            return 1;
        case '\n':
            token->type = T_NL;
            return 1;
        case ',':
            token->type = T_COMMA;
            return 1;
        case '$':
            token->type = T_DOLLAR;
            //c = next();
            return 1;
            // don't execute right now
            if( isdigit( c ) ) {
                int val = 0;
                while( isdigit( c ) ) {
                    val = val * 10 + my_atoi( c );
                    c = next();
                }
                g_putback = c;
                token->val = val;
                //printf( "g_token.val = %d\n", g_token.val );
            } else {
                printf( "Syntax error: $ not followed by int lit\n" );
            }
            return 1;
        case '%':
            token->type = T_PERCENT;
            c = next();
            // check register
            if( c == 'r' ) {
                char tmp[ 3 ];
                tmp[ 0 ] = c;
                c = next();
                if( isdigit( c ) ) {
                    tmp[ 1 ] = c;
                    tmp[ 2 ] = 0;
                    strcpy( g_text, tmp );
                    //printf( "g_text = %s\n", g_text );
                }
            }
            return 1;
        default:
            // keyword
            if( isalpha( c ) || c == '_' ) {
                scankw( c, g_text, 512 );
                int tokentype = kw( g_text );
                if( tokentype ) {
                    token->type = tokentype;
                    return 1;
                }
                printf( "Unrecognized symbol '%s'\n", g_text );
                exit( 1 );
            }
            //return 0;
            // number
            if( isdigit( c ) ) {
                int val = 0;
                while( isdigit( c ) ) {
                    val = val * 10 + my_atoi( c );
                    c = next();
                }
                g_putback = c;
                token->type = T_LIT;
                token->val = val;
                return 1;
            }
            return 0;
    }
    return 1;
}
tnode_t* mknode( int op, tnode_t* lvalue, tnode_t* rvalue, long long intvalue ) {
    tnode_t* retval = calloc( 1, sizeof( *retval ) );
    if( !retval ) {
        printf( "mknode: Failed to allocate memory for AST node.\n" );
        exit( 1 );
    }
    retval->op = op;
    retval->lvalue = lvalue;
    retval->rvalue = rvalue;
    retval->intvalue = intvalue;
    return retval;
}
tnode_t* checkint( void ) {
    tnode_t* retval;
    //printf( "g_token.type = %d\n", g_token.type );
    if( g_token.type == T_DOLLAR ) scan( &g_token );
    else {
        printf( "Syntax error: checkint() did not find $\n" );
        exit( 1 );
    }
    if( g_token.type == T_LIT ) {
        retval = mknode( OP_LIT, NULL, NULL, g_token.val );
        scan( &g_token );
        //printf( "checkint: value = %d\n", g_token.val );
        //printf( "checkint: return to expr\n" );
        return retval;
    } else {
        printf( "Syntax error: checkint()\n" );
        exit( 1 );
    }
}
tnode_t* checkreg( void ) {
    tnode_t* retval;
    if( g_token.type == T_PERCENT ) {
        retval = mknode( OP_REG, NULL, NULL, ( long long )g_text );
        scan( &g_token );
        return retval;
    } else {
        printf( "Syntax error: checkreg()\n" );
        exit( 1 );
    }
}
tnode_t* c_lvalue( void ) {
    tnode_t* retval;
    if( g_token.type != T_INSTR_LVR ) {
        printf( "Syntax error: primary() unknown token type %d\n", g_token.type );
        exit( 1 );
    }
    
    /*
    tnode_t* retval;
    if( g_token.type == T_INSTR_LVR ) {
        scan( &g_token );
        tnode_t* left = checkreg();
        if( g_token.type != T_COMMA ) {
            printf( "Syntax error: primary_lvalue() comma not found.\n" );
            exit( 1 );
        }
        scan( &g_token );
        tnode_t* right = checkint();
        retval = mknode( OP_LVR, left, right, g_token.val );
        return retval;
    } else {
        printf( "Syntax error: primary_lvalue() instruction not found.\n" );
        exit( 1 );
    }
    */
}
tnode_t* c_rvalue( void ) {
    tnode_t* retval;
    //if( g_token.type !=  )
}
int checkmath( int token ) {
    switch( token ) {
        case T_PLUS:
            return OP_ADD;
        case T_MINUS:
            return OP_SUB;
        case T_STAR:
            return OP_MUL;
        case T_SLASH:
            return OP_DIV;
        default:
            printf( "Syntax error: checkmath()\n" );
            exit( 1 );
    }
}
int op_prec( int toktype ) {
    if( g_op_prec[ toktype ] == 0 ) {
        printf( "Syntax error: op_prec().\n" );
        exit( 1 );
    }
    return g_op_prec[ toktype ];
}
tnode_t* expr( int prec ) {
    tnode_t *left, *right, *retval;
    int mathop, toktype;
    left = c_lvalue();
    //int mathop = checkmath( g_token.type );
    //toktype = g_token.type;
    if( toktype == T_NL ) return left;
    right = c_rvalue();
    /*
    while( op_prec( toktype ) > prec ) {
        scan( &g_token );
        right = expr( g_op_prec[ toktype ] );
        left = mknode( checkmath( toktype ), left, right, 0 );
        toktype = g_token.type;
        if( toktype == T_EOF ) return left;
    }
    */
    retval = mknode( 0, left, right, g_token.val );
    return retval;
}
int interp( tnode_t* node ) {
    int left, right;
    if( node->lvalue ) {
        left = interp( node->lvalue );
    }
    if( node->rvalue ) {
        right = interp( node->rvalue );
    }
    switch( node->op ) {
        case OP_ADD:
            printf( "%d + %d = %d\n", left, right, left+right );
            return left+right;
        case OP_SUB:
            printf( "%d - %d = %d\n", left, right, left-right );
            return left-right;
        case OP_MUL:
            printf( "%d * %d = %d\n", left, right, left*right );
            return left*right;
        case OP_DIV:
            printf( "%d / %d = %d\n", left, right, left/right );
            return left/right;
        case OP_LIT:
            return node->intvalue;
        default:
            printf( "Unknown operator: %d\n", node->op );
            exit( 1 );
    }
}

int main( void ) {
    scan( &g_token );
    tnode_t* l_ast = expr( 0 );
    printf( "\nBegin interp\n" );
    printf( "%d\n", interp( l_ast ) );
    return 0;
}
