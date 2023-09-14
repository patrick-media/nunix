#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<ctype.h>
#include<string.h>

const char T_OPS[ 6 ] = { 'E', '+', '-', '*', '/', 'L' };
typedef struct {
    enum {
        T_EOF = 0,
        T_PLUS,   // 1
        T_MINUS,  // 2
        T_STAR,   // 3
        T_SLASH,  // 4
        T_LIT     // 5
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
        OP_LIT
    } op;
    tnode_t* lvalue;
    tnode_t* rvalue;
    long long intvalue;
} tnode_t;

//const char* input = "2+3*5-8/3\0";
const char* input = "2 + 3 * 5 - 8 / 3\0";
//const char* input = "13 -6+  4*\n5\n+\n08 / 3\0";
token_t g_token = { 0 };
char g_putback = 0;
int g_op_prec[] = { 0, 10, 10, 20, 20, 0 };

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
    while( c == ' ' || c == '\n' || c == '\t' ) {
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
int scan( token_t* token ) {
    int c = skip();
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
        default:
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
int checkmath( int token ) {
    switch( token ) {
        case T_PLUS:
            //printf( "checkmath: returning OP_ADD\n", token );
            return OP_ADD;
        case T_MINUS:
            //printf( "checkmath: returning OP_SUB\n", token );
            return OP_SUB;
        case T_STAR:
            //printf( "checkmath: returning OP_MUL\n", token );
            return OP_MUL;
        case T_SLASH:
            //printf( "checkmath: returning OP_DIV\n", token );
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
    tnode_t *left, *right;
    int mathop, toktype;
    
    left = checkint();
    //int mathop = checkmath( g_token.type );
    toktype = g_token.type;
    if( toktype == T_EOF ) return left;
    while( op_prec( toktype ) > prec ) {
        scan( &g_token );
        right = expr( g_op_prec[ toktype ] );
        left = mknode( checkmath( toktype ), left, right, 0 );
        toktype = g_token.type;
        if( toktype == T_EOF ) return left;
    }
    return left;
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
            //printf( "interp: OP_ADD\n" );
            printf( "%d + %d = %d\n", left, right, left+right );
            return left+right;
        case OP_SUB:
            //printf( "interp: OP_SUB\n" );
            printf( "%d - %d = %d\n", left, right, left-right );
            return left-right;
        case OP_MUL:
            //printf( "interp: OP_MUL\n" );
            printf( "%d * %d = %d\n", left, right, left*right );
            return left*right;
        case OP_DIV:
            //printf( "interp: OP_DIV\n" );
            printf( "%d / %d = %d\n", left, right, left/right );
            return left/right;
        case OP_LIT:
            //printf( "interp: OP_LIT\n" );
            //printf( "%d (LIT)\n", node->intvalue );
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
