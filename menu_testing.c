#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<ctype.h>
#include<string.h>

typedef void ( **menufuncs_t )( void );
typedef struct {
    char* title;
    char** options;
    uint16_t num_options;
    int8_t selected;
    menufuncs_t menufuncs;
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
    
    this_menu->menufuncs = calloc( num_options, sizeof( *this_menu->menufuncs ) );
    if( !this_menu->menufuncs ) {
        printf( "Failed to allocate memory for menu functions container.\n" );
        exit( 1 );
    }
    for( int i = 0; i < this_menu->num_options; i++ ) {
        this_menu->menufuncs[ i ] = NULL;
    }
    
    return this_menu;
}
void add_menu_func( menu_t* menu, void ( *func )( void ) ) {
    static int menufuncs_p = 0;
    if( menufuncs_p >= menu->num_options ) return;
    menu->menufuncs[ menufuncs_p++ ] = func;
}
void delete_menu( menu_t* menu ) {
    for( int i = 0; i < menu->num_options; i++ ) {
        free( menu->options[ i ] );
    }
    free( menu->menufuncs );
    free( menu->options );
    free( menu->title );
    free( menu );
}

void options_tmode( void ) {
    printf( "FUNCTION: Text mode options selected.\n" );
}
void options_gmode( void ) {
    printf( "FUNCTION: Graphics mode options selected.\n" );
}
void options_shell( void ) {
    printf( "FUNCTION: Shell selected.\n" );
}
void options_exit( void ) {
    printf( "FUNCTION: Exit selected.\n" );
    exit( 1 );
}

int main( void ) {
    char* testoptions[ 4 ] = { "Text Mode Options", "Graphics Mode Options", "Shell", "Exit" };
    menu_t* mainmenu = create_menu( "Select Option", ( char*[ 4 ] ){ "Text Mode Options", "Graphics Mode Options", "Shell", "Exit" }, 4 );
    add_menu_func( mainmenu, options_tmode );
    add_menu_func( mainmenu, options_gmode );
    add_menu_func( mainmenu, options_shell );
    add_menu_func( mainmenu, options_exit );
    
    while( true ) {
        printf( "%s\n---------------------------------------------\n ", mainmenu->title );
        for( int i = 0; i < mainmenu->num_options; i++ ) {
            if( i == mainmenu->selected ) printf( ">" );
            else printf( " " );
            printf( "%d. %s\n ", i, mainmenu->options[ i ] );
        }
            
        printf( "> " );
        char* input = calloc( 4, sizeof( *input ) );
        if( !input ) {
            printf( "Failed to allocate memory for input buffer.\n" );
            exit( 1 );
        }
        scanf( " %[^\n]s", input );
        switch( *input ) {
            case 'd':
                if( mainmenu->selected < mainmenu->num_options - 1 ) mainmenu->selected++;
                break;
            case 'u':
                if( mainmenu->selected > 0 ) mainmenu->selected--;
                break;
            case 'e':
                mainmenu->menufuncs[ mainmenu->selected ]();
                break;
            default:
                printf( "Invalid input entered. Please try again.\n" );
                break;
        }
    }
    delete_menu( mainmenu );
    return 0;
}
