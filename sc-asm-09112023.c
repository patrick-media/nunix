#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<ctype.h>

const char T_OPS[ 6 ] = { 'E', '+', '-', '*', '/', 'L' };
typedef struct {
    enum {
        T_EOF = 0,
        T_PLUS,   // 1
        T_MINUS,  // 2
        T_STAR,   // 3
        T_SLASH,  // 4
        T_LIT     // 5
    } token;
    int val;
} token_t;

const char* input = "5 + 9  -4";
token_t g_token = { 0 };
char g_putback = 0;

int next( void ) {
    //if( g_putback ) return g_putback;
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
            token->token = T_EOF;
            return 1;
            break;
        case '+':
            token->token = T_PLUS;
            return 1;
            break;
        case '-':
            token->token = T_MINUS;
            return 1;
            break;
        case '*':
            token->token = T_STAR;
            return 1;
            break;
        case '/':
            token->token = T_SLASH;
            return 1;
            break;
        default:
            if( isdigit( c ) ) {
                int val = 0;
                while( isdigit( c ) ) {
                    val = val * 10 + my_atoi( c );
                    c = next();
                }
                g_putback = c;
                token->token = T_LIT;
                token->val = val;
                return 1;
            }
            return 0;
    }
    return 1;
}

int main( void ) {
    int scanret = scan( &g_token );
    while( scanret ) {
        printf( "Token data:\n" );
        printf( "\ttoken = %d (%c)\n", g_token.token, T_OPS[ g_token.token ] );
        printf( "\tintval = %d\n", g_token.val );
        // DEBUG
        printf( "\tscanret = %d\n", scanret );
        scanret = scan( &g_token );
    }
    return 0;
}
