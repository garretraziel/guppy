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

#include "guppy.h"
#include "lexical.h"

#define STR_INIT_LEN 16

// TODO chybi generovani vnitrniho kodu
//  chybi jeho implementace

// Chybova hlseni syntaktickych chyb
const char *SYN_ERRORS[] = {
    [0] = "<null>",
    [-ERROR_SYN_UX_TOKEN] = "Neocekavany token",
    [-ERROR_SYN_X_EOF] = "Ocekavan konec zdrojoveho souboru",
    [-ERROR_SYN_X_FUNC] = "Ocekavana definice funkce",
    [-ERROR_SYN_X_IDENT] = "Ocekavan identifikator",
    [-ERROR_SYN_X_SMCLN] = "Ocekavan znak ;",
    [-ERROR_SYN_X_END] = "Ocekavano END",
    [-ERROR_SYN_X_ASGN] = "Ocekavan operator =",
    [-ERROR_SYN_X_LBRC] = "Ocekavan znak (",
    [-ERROR_SYN_X_RBRC] = "Ocekavan znak )",
    [-ERROR_SYN_X_THEN] = "Ocekavano THEN",
    [-ERROR_SYN_X_ELSE] = "Ocekavano ELSE",
    [-ERROR_SYN_X_DO] = "Ocekavano DO",
    [-ERROR_SYN_X_OPRTR] = "Ocekavan operator",
};

// TODO: nepozirat lexikalni chyby

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
int expression_seq();
int expression();
int operator();


// program -> definice_funkci ; EOF
int program(FILE *in)
{
    input = in;
    int x;
    str_new(&str, STR_INIT_LEN); // TODO kontrolovat

    // definice funkci
    x = functions_seq();
    if(x < 0)
        return x;

    // strednik
    token = get_token(input, &str);
    if(token != SEMICOLON)
        return ERROR_SYN_X_SMCLN;

    // EOF
    token = get_token(input, &str);
    if(token != NOTHING)
        return ERROR_SYN_X_EOF;

    return 1;
}

//
int functions_seq()
{
    int x;
    // funkce
    x = function();
    if(x < 0)
        return x;

    // definice_funkci_z, bud dalsi funkce nebo cokoliv dalsiho FIXME
    token = get_token(input, &str);
    unget_token(token);
    if(token == FUNCTION)
        return functions_seq();
    else
       return 1;
}

//
int function()
{
    int x;
    // function
    token = get_token(input, &str);
    if(token != FUNCTION)
        return ERROR_SYN_X_FUNC;
    // identifier
    token = get_token(input, &str);
    if(token != IDENTIFIER)
        return ERROR_SYN_X_IDENT;
    // left bracket
    token = get_token(input, &str);
    if(token != LBRAC)
        return ERROR_SYN_X_LBRC;
    // formal parametr seq
    x = formal_parametr_seq();
    if(x < 0)
        return x;
    // local declaration seq
    x = local_declaration_seq();
    if(x < 0)
        return x;
    // statement seq
    x = statement_seq();
    if(x < 0)
        return x;
    return 1;
}

//
int formal_parametr_seq()
{
    token = get_token(input, &str);
    switch(token) {
        case RBRAC:
            return 1;
        case IDENTIFIER:
            // strcit si ho nekam
            return formal_parametr_seq_z();
        default:
            return ERROR_SYN_UX_TOKEN;
    }
}

//
int formal_parametr_seq_z()
{
    token = get_token(input, &str);
    switch(token) {
        case RBRAC:
            return 1;
        case COMMA:
            // identifikator
            token = get_token(input, &str);
            if(token != IDENTIFIER)
                return ERROR_SYN_X_IDENT;
            return formal_parametr_seq_z();
        default:
            return ERROR_SYN_UX_TOKEN;
    }
}

int local_declaration_seq()
{
    int x;
    // nic nebo local
    token = get_token(input, &str);
    if(token != LOCAL) {
        unget_token(token);
        return 1;
    }
    // identifier
    token = get_token(input, &str);
    if(token != IDENTIFIER)
        return ERROR_SYN_X_IDENT;
    // local declaration z
    x = local_declaration_z();
    if(x < 0)
        return x;
    return 1;
}

int local_declaration_z()
{
    int x;
    // strednik nebo rovnitko
    token = get_token(input, &str);
    if(token == SEMICOLON)
        // ulozit na stack s nil?
        return 1;
    else if(token == ASSIGN) {
        x = literal();
        if(x < 0)
            return x;
        // strednik
        token = get_token(input, &str);
        if(token != SEMICOLON)
            return ERROR_SYN_X_SMCLN;
        return 1;
    }
    else
        return 0;
}

int literal()
{
    token = get_token(input, &str);
    switch(token) {
        case NUMBER:
        case STRING:
        case NIL:
        case TRUE:
        case FALSE:
            // ulozit na stack s hodnotou
            return 1;
        default:
            return 0;
    }
}

int statement_seq()
{
    int x;
    token = get_token(input, &str);
    // bud end nebo prikaz
    if(token == END)
        // konec funkce, to bude asi znamenat skok do caller funkce
        return 1;
    else {
        unget_token(token);
        // statement
        x = statement();
        if(x < 0)
            return x;
        // end
        token = get_token(input, &str);
        if(token != END)
            return ERROR_SYN_X_END;
        // statement_seq
        return statement_seq();
    }
}

int statement()
{
    int x;
    token = get_token(input, &str);
    switch(token) {
        case IDENTIFIER:
            // rovnitko
            token = get_token(input, &str);
            if(token != ASSIGN)
                return ERROR_SYN_X_ASGN;
            // assign_z
            return assign_z();

        case WRITE:
            // left bracket
            token = get_token(input, &str);
            if(token != LBRAC)
                return ERROR_SYN_X_LBRC;
            // expression list
            x = expression_seq();
            if(x < 0)
                return x;
            // right bracket
            token = get_token(input, &str);
            if(token != RBRAC)
                return ERROR_SYN_X_RBRC;

        case IF:
            // vyraz
            x = expression();
            if(x < 0)
                return x;
            // then
            token = get_token(input, &str);
            if(token != THEN)
                return ERROR_SYN_X_THEN;
            // sekvence bez end FIXME
            x = statement_seq();
            if(x < 0)
                return x;
            // else
            token = get_token(input, &str);
            if(token != ELSE)
                return ERROR_SYN_X_ELSE;
            // sekvence prikazu
            x = statement_seq();
            return x;

        case WHILE:
            // vyraz
            x = expression();
            if(x < 0)
                return x;
            // do
            token = get_token(input, &str);
            if(token != DO)
                return ERROR_SYN_X_DO;
            // sekvence prikazu
            x = statement_seq();
            return x;

        case RETURN:
            // vyraz
            x = expression();
            return x;
            // konec funkce heh

        default:
            return 0;
    }
}

// TODO jeste chybi parametry funkci nejak osefovat
// a dokoncit co je dolu

int assign_z()
{
    // read
    // vyraz
    // identifier()
    return 1;
}

// to je asi jen pro write
int expression_seq()
{
    return 1;
}

int expression()
{
    // tohle vubec netusim, to se resi jinak
    return 1;
}

int operator()
{
    token = get_token(input, &str);
    switch(token) {
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
            return 1;
        default:
            return ERROR_SYN_X_OPRTR;
    }
}

