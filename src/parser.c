/*
 * projekt: interpret jazyka IFJ11 
 * soubor: parser.c
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "lexical.h"
#include "parser.h"
#include "guppy.h"
#include "expr.h"
#include "ial.h"

#define STR_INIT_LEN 16


// Makro ktere provede prikaz, pokud skonci zapornym navratovym kodem, bude
// proveden return teto hodnoty, jinak nic
#define try(command) do { int x = command; if(x<0) return x; } while(0)

// Pokud je token jiny nez ocekavany token, je vracen token,
// pokud byl sam zaporny, jinak zadany chybovy kod
#define check_token(exptok, errcode) do { if(token != exptok) \
    return (token < 0) ? token : errcode; } while(0)

#ifdef NDEBUG
void print_functions(FunctionTree *);
#endif


// prototypy funci, asi nepatri do hlavicky, nejsou soucasti rozhrani
static int program(void);
static int functions_seq(void);
static int function(void);
static int formal_parametr_seq(void);
static int formal_parametr_seq_z(void);
static int local_declaration_seq(void);
static int local_declaration_z(void);
static int literal(void);
static int statement_seq(void);
static int statement(void);
static int assign_z(void);

static int expression_seq(void);
static int expression_seq_z(void);



/*
 * Spousti syntaktickou analyzu
 */
int parser(void)
{
    try( str_new(&str, STR_INIT_LEN) );

    int x = program();

#ifdef NDEBUG
    print_functions(functions_table);
#endif

    str_free(&str);
    return x;
}




// P -> DF DFS ; EOF
static int program(void)
{
    // nacteni uplne prvniho tokenu
    // to ma byt predem, ale to by musel byt token uplne globalni
    get_token();

    // prvni funkce
    if(token != FUNCTION) {
        str_free(&str);
        return (token < 0) ? token : ERROR_SYN_X_FUNC;
    }
    try( function() );

    // ostatni funkce
    try( functions_seq() );

    // strednik za posledni funkci
    check_token(SEMICOLON, ERROR_SYN_X_SMCLN);

    // EOF (ukoncivac dolar)
    get_token();
    check_token(NOTHING, ERROR_SYN_X_EOF);

    // kontrola, zda je main posledni
    if(strcmp(last_function->name, "main") != 0)
        return ERROR_SYN_MAIN;
    return 1;
}

// DFS -> DF DFS
// DFS -> epsilon
static int functions_seq(void)
{
    // funkce
    if(token == FUNCTION) {
        try ( function() );
        return functions_seq();
    }
    else // epsilon
        return 1;
}

// DF -> function id ( FPS ) SDS SPS end
static int function(void)
{
    // function
    check_token(FUNCTION, ERROR_SYN_X_FUNC);
    get_token();

    // identifier
    check_token(IDENTIFIER, ERROR_SYN_X_IDENT);
    // funkce do tabulky
    try( insert_function(str.str) );
    // potreba novy string, puvodni je v tabulce
    try( str_new(&str, STR_INIT_LEN) ); 
    get_token();

    // left bracket
    check_token(LBRAC, ERROR_SYN_X_LBRC);
    get_token();

    // formal parametr seq
    try( formal_parametr_seq() );

    // right bracket
    check_token(RBRAC, ERROR_SYN_X_RBRC);
    get_token();

    // local declaration seq
    try( local_declaration_seq() );

    // statement seq
    try( statement_seq() );

    // end
    check_token(END, ERROR_SYN_X_END);
    get_token();

    return 1;
}

// FPS -> id FPz
// epsilon
static int formal_parametr_seq(void)
{
    // identifikator
    if(token == IDENTIFIER) {
        // pridat parametr do tabulky symbolu aktualni funkce
        // defaultni hodnota je nil
        try( insert_local(str.str) );
        // potreba novy string
        try( str_new(&str, STR_INIT_LEN) );
        get_token();
        return formal_parametr_seq_z();
    }
    else // epsilon
        return 1;
}

// FPz -> , id FPz
// FPz -> epsilon
static int formal_parametr_seq_z(void)
{
    // carka
    if(token == COMMA) {
        get_token();
        // identifikator
        check_token(IDENTIFIER, ERROR_SYN_X_IDENT);
        try( insert_local(str.str) ); // pridat do stromu
        try( str_new(&str, STR_INIT_LEN) ); // novy string
        get_token();
        return formal_parametr_seq_z();
    }
    else // epsilon
        return 1;
}

// SDS -> local id SDSz SDS
// SDS -> epsilon
static int local_declaration_seq(void)
{
    // local
    if(token == LOCAL) {
        get_token();
        // identifier
        check_token(IDENTIFIER, ERROR_SYN_X_IDENT);
        try( insert_local(str.str) ); // pridat do tabulky
        try( str_new(&str, STR_INIT_LEN) ); // novy string
        get_token();
        // local declaration z
        try( local_declaration_z() );
        // local declaration seq
        return local_declaration_seq();
    }
    else // epsilon
        return 1;
}

// SDSz -> = LIT ;
// SDSz -> ;
static int local_declaration_z(void)
{
    // strednik
    if(token == SEMICOLON) {
        // TODO, tady se musi generovat vlozeni inicializace na nil
        get_token();
        return 1;
    }
    // rovnitko
    else if(token == ASSIGN) {
        get_token();
        // literal
        try( literal() ); 
        // strednik
        check_token(SEMICOLON, ERROR_SYN_X_SMCLN);
        get_token();
        return 1;
    }
    else
        return (token < 0) ? token : ERROR_SYN_UX_TOKEN;
}

// LIT -> num
// LIT -> str
// LIT -> nil
// LIT -> true
// LIT -> false
static int literal()
{
    Data data;
    LiteralTree *lit;
    switch(token) {
        case NUMBER:
            data.type = T_NUMBER;
            data.value.num = strtod(str.str, NULL);
            break;
        case STRING:
            data.type = T_STRING;
            data.value.str = str.str;
            try( str_new(&str, STR_INIT_LEN) );
            break;
        case NIL:
            data.type = T_NIL;
            break;
        case TRUE:
            data.type = T_BOOLEAN;
            data.value.log = 1;
            break;
        case FALSE:
            data.type = T_BOOLEAN;
            data.value.log = 0;
            break;
        default:
            return (token < 0) ? token : ERROR_SYN_UX_TOKEN;
    }
    try( insert_literal(data, &lit) );
    // TODO instrukce na nacteni hodnoty do lok. promenne
    // instrukce bude rikat, ze do last_local se ma nacist z adresy lit
#ifdef NDEBUG
    // test, jak to vypada
    printf("mov [EBP + %2d], %p    // %s <- ", last_local->offset, (void*)lit, last_local->name);
    if(lit->data.type == T_NUMBER)
        printf("%g\n", lit->data.value.num);
    else if(lit->data.type == T_STRING)
        printf("%s\n", lit->data.value.str);
    else if(lit->data.type == T_BOOLEAN)
        printf("%s\n", lit->data.value.log?"true":"false");
    else if(lit->data.type == T_NIL)
        printf("nil\n");
#endif
    get_token();
    return 1;
}

// SPS -> S ; SPS
// SPS -> epsilon
static int statement_seq(void)
{
    switch(token) {
        case IDENTIFIER:
        case WRITE:
        case IF:
        case WHILE:
        case RETURN:
            // statement
            try( statement() );
            // strednik
            check_token(SEMICOLON, ERROR_SYN_X_SMCLN);
            get_token();
            // statement_seq
            return statement_seq();
            break;
        default:
            return 1;
            break;
    }
}

// S -> id = Az
// S -> write ( EL )
// S -> if E then SPS else SPS end
// S -> while E do SPS end
// S -> ret E
static int statement(void)
{
    int x;
    switch(token) {
        case IDENTIFIER:
            // kontrola, jestli je promenna definovana
            if(find_local(str.str) == NULL)
                return ERROR_SEM_VAR_UND;
            get_token();
            // rovnitko
            check_token(ASSIGN, ERROR_SYN_X_ASGN);
            get_token();
            // assign_z
            return assign_z();

        case WRITE:
            get_token();
            // left bracket
            check_token(LBRAC, ERROR_SYN_X_LBRC);
            get_token();
            // expression list
            try( expression_seq() );
            // right bracket
            check_token(RBRAC, ERROR_SYN_X_RBRC);
            get_token();
            return 1;

        case IF:
            get_token();
            // vyraz
            try( expression() );
            // then
            check_token(THEN, ERROR_SYN_X_THEN);
            get_token();
            // prikazy
            try( statement_seq() );
            // else
            check_token(ELSE, ERROR_SYN_X_ELSE);
            get_token();
            // sekvence prikazu
            try( statement_seq() );
            // end
            check_token(END, ERROR_SYN_X_END);
            get_token();
            return 1;

        case WHILE:
            get_token();
            // vyraz
            try( expression() );
            // do
            check_token(DO, ERROR_SYN_X_DO);
            get_token();
            // sekvence prikazu
            try( statement_seq() );
            // end
            check_token(END, ERROR_SYN_X_END);
            get_token();
            return 1;

        case RETURN:
            get_token();
            // vyraz
            x = expression();
            return x;
            // konec funkce heh

        default:
            return (token < 0) ? token : ERROR_SYN_UX_TOKEN;
    }
}

// Az -> read ( LIT )
// Az -> vyraz
static int assign_z(void)
{
    // read
    if(token == READ) {
        get_token();
        // leva zavorka
        check_token(LBRAC, ERROR_SYN_X_LBRC);
        get_token();
        // jeden parametr (string nebo cislo) FIXME
        if(token != STRING && token != NUMBER)
            return (token < 0) ? token : ERROR_SYN_UX_TOKEN;
        get_token();
        // prava zavorka
        check_token(RBRAC, ERROR_SYN_X_RBRC);
        get_token();
        return 1;
    }
    // vyraz
    else
        return expression();
}


// // // // // // // // // // // // // // // // // // // // // // // // //
// Co je dal uz nefunguje ani zatim nema:

// ES -> E ESz
static int expression_seq(void)
{
    try( expression() );
    return expression_seq_z();
}

// ESz -> , E ESz
// ESz -> epsilon
static int expression_seq_z(void)
{
    if(token == COMMA) {
        get_token();
        try( expression() );
        return expression_seq_z();
    }
    // epsilon
    else
        return 1;
}
