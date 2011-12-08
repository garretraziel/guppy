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
#include "exec.h"
#include "expr.h"


// Pokud je token jiny nez ocekavany token, je vracen token,
// pokud byl sam zaporny, jinak zadany chybovy kod
#define check_token(exptok, errcode) do { if(token != exptok) \
    return (token < 0) ? token : errcode; } while(0)

#ifdef DEBUG
void print_functions(FunctionTree *);
#endif

// TODO kotrolovat navratovou hodnotu generate, makrem ? ]:->


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

#ifdef DEBUG
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

    // rezervovani jmen pro builtin funkce
    string str;
    try( str_init(&str, "sort") );
    try( insert_function(str.str, NULL) );
    last_function->params = 1;

    try( str_init(&str, "type") );
    try( insert_function(str.str, NULL) );
    last_function->params = 1;

    try( str_init(&str, "substr") );
    try( insert_function(str.str, NULL) );
    last_function->params = 3;
    
    try( str_init(&str, "find") );
    try( insert_function(str.str, NULL) );
    last_function->params = 2;

    // prvni funkce
    check_token(FUNCTION, ERROR_SYN_X_FUNC);
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

    // tohle je kod, kterym by melo zacit vykonavani
    // TODO: vratit na to nejak adresu
    main_pointer = generate(INOP, NULL);
    for(int i = 0, j = last_function->params; i < j; ++i)
        generate(IPUSHN, NULL);
    generate(ICALL, last_function);
    generate(IHALT, NULL);
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

    PTapeItem lab;
    lab = generate(INOP, NULL);

    // identifier
    check_token(IDENTIFIER, ERROR_SYN_X_IDENT);
    // funkce do tabulky
    try( insert_function(str.str, lab) );
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

    // konec definic lokalnich symbolu, musi se prepocitat offsety
    recount_offsets();

    // statement seq
    try( statement_seq() );

    // end
    check_token(END, ERROR_SYN_X_END);
    get_token();

    // pri neuvedeni return to tenhle zachrani
    generate(IPUSHN, NULL);
    generate(IRET, last_function);
    return 1;
}

// FPS -> id FPz
// epsilon
static int formal_parametr_seq(void)
{
    // identifikator
    if(token == IDENTIFIER) {
        // pridat parametr do tabulky symbolu aktualni funkce
        try( insert_local_param(str.str) );
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
        try( insert_local_param(str.str) ); // pridat do stromu
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
        try( insert_local_var(str.str) ); // pridat do tabulky
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

// SDSz -> = E ;
// SDSz -> ;
static int local_declaration_z(void)
{
    // strednik
    if(token == SEMICOLON) {
        // inicializace na nil
        generate(IPUSHN, NULL);
        generate(IPOPI, last_local);
        get_token();
        return 1;
    }
    // rovnitko
    else if(token == ASSIGN) {
        get_token();
        // vyraz
        try( expression() ); 
        generate(IPOPI, last_local);
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
    try( insert_literal(data) );
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
        case REPEAT:
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
    PTapeItem ptr1, ptr2;
    PTapeItem lab1, lab2;
    LocalTree *var;

    switch(token) {
        case IDENTIFIER:
            // kontrola, jestli je promenna definovana
            if((var = find_local(str.str)) == NULL)
                return ERROR_SEM_VAR_UND;
            get_token();
            // rovnitko
            check_token(ASSIGN, ERROR_SYN_X_ASGN);
            get_token();
            // assign_z
            try( assign_z() );
            // generovani presunu vysledku do promenne
            generate(IPOPI, var);
            return 1;

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
            ptr1 = generate(IJMPF, NULL); // skok na else pri false
            // then
            check_token(THEN, ERROR_SYN_X_THEN);
            get_token();
            // prikazy
            try( statement_seq() );
            ptr2 = generate(IJMP, NULL); // skok na konec
            // else
            check_token(ELSE, ERROR_SYN_X_ELSE);
            get_token();
            lab1 = generate(INOP, NULL); // zacatek else
            ptr1->adr = lab1; // doplneni
            // sekvence prikazu
            try( statement_seq() );
            // end
            check_token(END, ERROR_SYN_X_END);
            get_token();
            lab2 = generate(INOP, NULL); // konec
            ptr2->adr = lab2;
            return 1;

        case WHILE:
            get_token();
            // label
            lab1 = generate(INOP, NULL);
            // vyraz
            try( expression() );
            // skok
            ptr1 = generate(IJMPF, NULL);
            // do
            check_token(DO, ERROR_SYN_X_DO);
            get_token();
            // sekvence prikazu
            try( statement_seq() );
            // end
            check_token(END, ERROR_SYN_X_END);
            // jmp zpet a label konce
            generate(IJMP, lab1);
            lab2 = generate(INOP, NULL);
            ptr1->adr = lab2;
            get_token();
            return 1;

        case RETURN:
            get_token();
            // vyraz
            try( expression() );
            // return
            generate(IRET, last_function);
            return 1;
            // konec funkce heh

        case REPEAT:
            get_token();
            lab1 = generate(INOP, NULL);
            // sekvence prikazu
            try( statement_seq() );
            // until
            check_token(UNTIL, ERROR_SYN_X_UNTIL);
            get_token();
            // vyraz a skok
            try( expression() );
            generate(IJMPF, lab1);
            return 1;


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
        // parametr - literal
        try( literal() );
        // instrukce read
        generate(IREAD, last_literal);
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
    // pro prvni parametr write
    generate(IWRITE, NULL);
    return expression_seq_z();
}

// ESz -> , E ESz
// ESz -> epsilon
static int expression_seq_z(void)
{
    if(token == COMMA) {
        get_token();
        try( expression() );
        // pro kazdy parametr write
        generate(IWRITE, NULL);
        return expression_seq_z();
    }
    // epsilon
    else
        return 1;
}
