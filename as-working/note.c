#define MAGIC_INSTR_START 16
#define NUM_INSTR 47
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
    { "sfl",  42, 4 },
    { "sflx", 43, 4 },
    { "sfr",  44, 4 },
    { "sfrx", 45, 4 }
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
    /* 58 */ T_SFL,
    /* 59 */ T_SFLX,
    /* 60 */ T_SFR,
    /* 61 */ T_SFRX
} tok_e;

int keyword( char* s ) {
    switch( *s ) {
        case 'a':
        {
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
        }
        case 'c':
        {
            if( strcmp( s, "com" ) == 0 ) {
                //printf( "instruction = com\n" );
                return T_COM;
            }
            if( strcmp( s, "comx" ) == 0 ) {
                //printf( "instruction = comx\n" );
                return T_COMX;
            }
            break;
        }
        case 'd':
        {
            if( strcmp( s, "div" ) == 0 ) {
                //printf( "instruction = div\n" );
                return T_DIV;
            }
            if( strcmp( s, "divx" ) == 0 ) {
                //printf( "instruction = divx\n" );
                return T_DIVX;
            }
            break;
        }
        case 'i':
        {
            if( strcmp( s, "int" ) == 0 ) {
                //printf( "instruction = int\n" );
                return T_INT;
            }
            break;
        }
        case 'j':
        {
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
        }
        case 'l':
        {
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
        }
        case 'm':
        {
            if( strcmp( s, "mul" ) == 0 ) {
                //printf( "instruction = mul\n" );
                return T_MUL;
            }
            if( strcmp( s, "mulx" ) == 0 ) {
                //printf( "instruction = mulx\n" );
                return T_MULX;
            }
            break;
        }
        case 'o':
        {
            if( strcmp( s, "or" ) == 0 ) {
                //printf( "instruction = or\n" );
                return T_OR;
            }
            if( strcmp( s, "orx" ) == 0 ) {
                //printf( "instruction = orx\n" );
                return T_ORX;
            }
            break;
        }
        case 'r':
        {
            if( strcmp( s, "ret" ) == 0 ) {
                //printf( "instruction = ret\n" );
                return T_RET;
            }
            break;
        }
        case 's':
        {
            if( strcmp( s, "sfl" ) == 0 ) {
                //printf( "instruction = sfl\n" );
                return T_SFL;
            }
            if( strcmp( s, "sflx" ) == 0 ) {
                //printf( "instruction = sflx\n" );
                return T_SFLX;
            }
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
        }
        case 'v':
        {
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
        }
        case 'x':
        {
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
    }
    return 0;
}
