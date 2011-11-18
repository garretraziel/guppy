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

#include "lexical.h"
#include "parser.h"

#define STR_INIT_LEN 16

// TODO chybi generovani vnitrniho kodu
//  chybi jeho implementace

// TODO: ono to nepozna, ze je konec souboru driv nez ma
//  povazuje to za neocekavany token

// Chybova hlseni syntaktickych chyb
const char *SYN_ERRORS[] = {
    [0] = "<null>",
    [(-ERROR_SYN_UX_TOKEN) % 100] = "Neocekavany token",
    [(-ERROR_SYN_X_EOF) % 100] = "Ocekavan konec zdrojoveho souboru",
    [(-ERROR_SYN_X_FUNC) % 100] = "Ocekavana definice funkce",
    [(-ERROR_SYN_X_IDENT) % 100] = "Ocekavan identifikator",
    [(-ERROR_SYN_X_SMCLN) % 100] = "Ocekavan znak ;",
    [(-ERROR_SYN_X_END) % 100] = "Ocekavano END",
    [(-ERROR_SYN_X_ASGN) % 100] = "Ocekavan operator =",
    [(-ERROR_SYN_X_LBRC) % 100] = "Ocekavan znak (",
    [(-ERROR_SYN_X_RBRC) % 100] = "Ocekavan znak )",
    [(-ERROR_SYN_X_THEN) % 100] = "Ocekavano THEN",
    [(-ERROR_SYN_X_ELSE) % 100] = "Ocekavano ELSE",
    [(-ERROR_SYN_X_DO) % 100] = "Ocekavano DO",
    [(-ERROR_SYN_X_OPRTR) % 100] = "Ocekavan operator",
};


// globalni promenne, buh kvuli nim zabiji kotatka, je mi jich lito
int token;
string str;
FILE *input;

// prototypy funci, asi nepatri do hlavicky, nejsou soucasti rozhrani
int functions_seq();
int function();
int formal_parametr_seq();
int formal_parametr_seq_z();
int local_declaration_seq();
int local_declaration_z();
int literal();
int statement_seq();
int statement();
int assign_z();
int literal_identifier_list();
int literal_identifier_list_a();
int literal_identifier_list_z();

int expression_seq();
int expression_seq_z();
int expression();


// P -> DF DFS ; EOF
int program(FILE *in)
{
    input = in;
    int x;
    if(str_new(&str, STR_INIT_LEN) == 0)
        return 0; // TODO generovat chybu

    // nacteni uplne prvniho tokenu
    // to ma byt predem, ale to by musel byt token uplne globalni
    token = get_token(input, &str);

    // prvni funkce
    if(token != FUNCTION)
        return (token < 0) ? token : ERROR_SYN_X_FUNC;
    x = function();
    if(x < 0)
        return x;

    // ostatni funkce
    x = functions_seq();
    if(x < 0)
        return x;

    // strednik za posledni funkci
    if(token != SEMICOLON)
        return (token < 0) ? token : ERROR_SYN_X_SMCLN;

    // EOF (ukoncivac dolar)
    token = get_token(input, &str);
    if(token != NOTHING)
        return (token < 0) ? token : ERROR_SYN_X_EOF;

    str_free(&str);
    return 1;
}

// DFS -> DF DFS
// DFS -> epsilon
int functions_seq()
{
    int x;

    // funkce
    if(token == FUNCTION) {
        x = function();
        if(x < 0)
            return x;
        x = functions_seq();
        return x;
    }
    else // epsilon
        return 1;
}

// DF -> function id ( FPS ) SDS SPS end
int function()
{
    int x;
    // function
    if(token != FUNCTION)
        return (token < 0) ? token : ERROR_SYN_X_FUNC;
    token = get_token(input, &str);

    // identifier
    if(token != IDENTIFIER)
        return (token < 0) ? token : ERROR_SYN_X_IDENT;
    token = get_token(input, &str);

    // left bracket
    if(token != LBRAC)
        return (token < 0) ? token : ERROR_SYN_X_LBRC;
    token = get_token(input, &str);

    // formal parametr seq
    x = formal_parametr_seq();
    if(x < 0)
        return x;

    // right bracket
    if(token != RBRAC)
        return (token < 0) ? token : ERROR_SYN_X_RBRC;
    token = get_token(input, &str);

    // local declaration seq
    x = local_declaration_seq();
    if(x < 0)
        return x;

    // statement seq
    x = statement_seq();
    if(x < 0)
        return x;

    // end
    if(token != END)
        return (token < 0) ? token : ERROR_SYN_X_END;
    token = get_token(input, &str);

    return 1;
}

// FPS -> id FPz
// epsilon
int formal_parametr_seq()
{
    // identifikator
    if(token == IDENTIFIER) {
        token = get_token(input, &str);
        return formal_parametr_seq_z();
    }
    else // epsilon
        return 1;
}

// FPz -> , id FPz
// FPz -> epsilon
int formal_parametr_seq_z()
{
    // carka
    if(token == COMMA) {
        token = get_token(input, &str);
        // identifikator
        if(token != IDENTIFIER)
            return (token < 0) ? token : ERROR_SYN_X_IDENT;
        token = get_token(input, &str);
        return formal_parametr_seq_z();
    }
    else // epsilon
        return 1;
}

// SDS -> local id SDSz SDS
// SDS -> epsilon
int local_declaration_seq()
{
    int x;
    // local
    if(token == LOCAL) {
        token = get_token(input, &str);
        // identifier
        if(token != IDENTIFIER)
            return (token < 0) ? token : ERROR_SYN_X_IDENT;
        token = get_token(input, &str);
        // local declaration z
        x = local_declaration_z();
        if(x < 0)
            return x;
        // local declaration seq
        return local_declaration_seq();
    }
    else // epsilon
        return 1;
}

// SDSz -> = LIT ;
// SDSz -> ;
int local_declaration_z()
{
    int x;
    // strednik
    if(token == SEMICOLON) {
        token = get_token(input, &str);
        return 1;
    }
    // rovnitko
    else if(token == ASSIGN) {
        token = get_token(input, &str);
        // literal
        x = literal();
        if(x < 0)
            return x;
        // strednik
        if(token != SEMICOLON)
            return (token < 0) ? token : ERROR_SYN_X_SMCLN;
        token = get_token(input, &str);
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
int literal()
{
    switch(token) {
        case NUMBER:
        case STRING:
        case NIL:
        case TRUE:
        case FALSE:
            // ulozit na stack s hodnotou
            token = get_token(input, &str);
            return 1;
        default:
            return (token < 0) ? token : ERROR_SYN_UX_TOKEN;
    }
}

// SPS -> S ; SPS
// SPS -> epsilon
int statement_seq()
{
    int x;
    switch(token) {
        case IDENTIFIER:
        case WRITE:
        case IF:
        case WHILE:
        case RETURN:
            // statement
            x = statement();
            if(x < 0)
                return x;
            // strednik
            if(token != SEMICOLON)
                return (token < 0) ? token : ERROR_SYN_X_SMCLN;
            token = get_token(input, &str);
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
int statement()
{
    int x;
    switch(token) {
        case IDENTIFIER:
            token = get_token(input, &str);
            // rovnitko
            if(token != ASSIGN)
                return (token < 0) ? token : ERROR_SYN_X_ASGN;
            token = get_token(input, &str);
            // assign_z
            return assign_z();

        case WRITE:
            token = get_token(input, &str);
            // left bracket
            if(token != LBRAC)
                return (token < 0) ? token : ERROR_SYN_X_LBRC;
            token = get_token(input, &str);
            // expression list
            x = expression_seq();
            if(x < 0)
                return x;
            // right bracket
            if(token != RBRAC)
                return (token < 0) ? token : ERROR_SYN_X_RBRC;
            token = get_token(input, &str);
            return 1;

        case IF:
            token = get_token(input, &str);
            // vyraz
            x = expression();
            if(x < 0)
                return x;
            // then
            if(token != THEN)
                return (token < 0) ? token : ERROR_SYN_X_THEN;
            token = get_token(input, &str);
            // prikazy
            x = statement_seq();
            if(x < 0)
                return x;
            // else
            if(token != ELSE)
                return (token < 0) ? token : ERROR_SYN_X_ELSE;
            token = get_token(input, &str);
            // sekvence prikazu
            x = statement_seq();
            // end
            if(token != END)
                return (token < 0) ? token : ERROR_SYN_X_END;
            token = get_token(input, &str);
            return 1;

        case WHILE:
            token = get_token(input, &str);
            // vyraz
            x = expression();
            if(x < 0)
                return x;
            // do
            if(token != DO)
                return (token < 0) ? token : ERROR_SYN_X_DO;
            token = get_token(input, &str);
            // sekvence prikazu
            x = statement_seq();
            // end
            if(token != END)
                return (token < 0) ? token : ERROR_SYN_X_END;
            token = get_token(input, &str);
            return 1;

        case RETURN:
            token = get_token(input, &str);
            // vyraz
            x = expression();
            return x;
            // konec funkce heh

        default:
            return (token < 0) ? token : ERROR_SYN_UX_TOKEN;
    }
}

// Az -> read ( LIT )
// Az -> E // TODO
// Az -> id ( LIL )
int assign_z()
{
    int x;
    // read
    if(token == READ) {
        token = get_token(input, &str);
        // leva zavorka
        if(token != LBRAC)
            return (token < 0) ? token : ERROR_SYN_X_LBRC;
        token = get_token(input, &str);
        // jeden parametr (string nebo cislo) FIXME
        if(token != STRING && token != NUMBER)
            return (token < 0) ? token : ERROR_SYN_UX_TOKEN;
        token = get_token(input, &str);
        // prava zavorka
        if(token != RBRAC)
            return (token < 0) ? token : ERROR_SYN_X_RBRC;
        token = get_token(input, &str);
        return 1;
    }
    // volani funkce
    else if(token == IDENTIFIER) {
        token = get_token(input, &str);
        // leva zavorka
        if(token != LBRAC)
            return (token < 0) ? token : ERROR_SYN_X_LBRC;
        token = get_token(input, &str);
        // parametry
        x = literal_identifier_list(); // TODO
        if(x < 0)
            return x;
        // prava zavorka
        if(token != RBRAC)
            return (token < 0) ? token : ERROR_SYN_X_RBRC;
        token = get_token(input, &str);
        return 1;
    }
    // vyraz
    else
        return expression();
}

// LIL -> LIT LILa
// LIL -> id LILa
// LIL -> epsilon
int literal_identifier_list()
{
    int x;
    switch(token) {
        // literal
        case NUMBER:
        case STRING:
        case NIL:
        case TRUE:
        case FALSE:
            x = literal();
            if(x < 0)
                return x;
            return literal_identifier_list_a();
        case IDENTIFIER:
            token = get_token(input, &str);
            return literal_identifier_list_a();
        default:
            return 1;
    }

}

// LILa -> , LILz LILa
// LILa -> epsilon
int literal_identifier_list_a()
{
    int x;
    if(token == COMMA) {
        token = get_token(input, &str);
        x = literal_identifier_list_z();
        if(x < 0)
            return 0;
        return literal_identifier_list_a();
    }
    else
        return 1;
}

// LILz -> LIT
// LILz -> id
int literal_identifier_list_z()
{
    switch(token) {
        case NUMBER:
        case STRING:
        case NIL:
        case TRUE:
        case FALSE:
            return literal();
        case IDENTIFIER:
            token = get_token(input, &str);
            return 1;
            break;
        default:
            return 1;
    }

}

// // // // // // // // // // // // // // // // // // // // // // // // //
// Co je dal uz nefunguje ani zatim nema:

// ES -> E ESz
int expression_seq()
{
    int x;
    x = expression();
    if(x < 0)
        return x;
    return expression_seq_z();
}

// ESz -> , E Elz
// ESz -> epsilon
int expression_seq_z()
{
    int x;
    if(token == COMMA) {
        token = get_token(input, &str);
        x = expression();
        if(x < 0)
            return x;
        return expression_seq_z();
    }
    // epsilon
    else
        return 1;
}

// tohle je jen nouzovka, preskakuje to
int expression()
{
    // zatim se proste vyraz preskoci a bude se predpokladat jeho spravnost
    // nesmi v nem byt zavorky
    switch(token) {
        // literal
        case NUMBER:
        case STRING:
        case NIL:
        case FALSE:
        case TRUE:
        // ident
        case IDENTIFIER:
        // zavorky 
//        case LBRAC:
//        case RBRAC:
        // operatory
        case PLUS:
        case MINUS:
        case DIV:
        case MUL:
        case POWER:
        case STRCONCAT:
        case LESS:
        case GREAT:
        case LESS_EQ:
        case GREAT_EQ:
        case EQUAL:
        case NOT_EQUAL:
            token = get_token(input, &str);
            return expression();
            break;
        default:
            return 1;
    }
}
