#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<ctype.h>
#include<string.h>

typedef struct {
    char* title;
    char** options;
    uint16_t num_options;
    int8_t selected;
} menu_t;

menu_t* create_menu( char* title, char** options, uint16_t num_options ) {
    menu_t* this_menu = calloc( 1, sizeof( *this_menu ) );
    if( !this_menu ) {
        printf( "Failed to allocate memory for menu object.\n" );
        exit( 1 );
    }
    
    //printf( "title alloc\n" );
    this_menu->title = calloc( 32, sizeof( *this_menu->title ) );
    if( !this_menu->title ) {
        printf( "Failed to allocate memory for menu title '%s'.\n", title );
        exit( 1 );
    }
    strcpy( this_menu->title, title );

    //printf( "options container alloc\n" );
    this_menu->options = calloc( num_options, sizeof( *this_menu->options ) );
    if( !this_menu->options ) {
        printf( "Failed to allocate memory for menu options container.\n" );
        exit( 1 );
    }
    for( int i = 0; i < num_options; i++ ) {
        //printf( "options alloc %d\n", i );
        this_menu->options[ i ] = calloc( 64, sizeof( *this_menu->options[ i ] ) );
        if( !this_menu->options[ i ] ) {
            printf( "Failed to allocate memory for options string '%s'\n", options[ i ] );
            exit( 1 );
        }
        //printf( "options strcpy\n" );
        strcpy( this_menu->options[ i ], options[ i ] );
    }

    this_menu->num_options = num_options;
    this_menu->selected = 0;
    return this_menu;
}

int main( void ) {
    char* testoptions[ 4 ] = { "one", "two", "three", "four" };
    menu_t* testmenu = create_menu( "test1", testoptions, 4 );
    printf( "title: '%s'\n", testmenu->title );
    printf( "options:\n" );
    for( int i = 0; i < testmenu->num_options; i++ ) {
        printf( "\t%d: '%s'\n", i, testmenu->options[ i ] );
    }
    printf( "num_options: %d\n", testmenu->num_options );
    printf( "selected: %d\n", testmenu->selected );
    return 0;
}
