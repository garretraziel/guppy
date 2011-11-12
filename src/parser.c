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
        return 0;

    // EOF
    token = get_token(input, &str);
    if(token != NOTHING)
        return 0;

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
        return 0;
    // identifier
    token = get_token(input, &str);
    if(token != IDENTIFIER)
        return 0;
    // left bracket
    token = get_token(input, &str);
    if(token != LBRAC)
        return 0;
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
            return 0;
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
            return formal_parametr_seq_z();
        default:
            return 0;
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
            return 0;
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
            return 0;
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
                return 0;
            // assign_z
            return assign_z();

        case WRITE:
            // left bracket
            token = get_token(input, &str);
            if(token != LBRAC)
                return 0;
            // expression list
            x = expression_seq();
            if(x < 0)
                return 0;
            // right bracket
            token = get_token(input, &str);
            if(token != RBRAC)
                return 0;

        case IF:
            // vyraz
            x = expression();
            if(x < 0)
                return x;
            // then
            token = get_token(input, &str);
            if(token != THEN)
                return 0;
            // sekvence bez end FIXME
            x = statement_seq();
            if(x < 0)
                return x;
            // else
            token = get_token(input, &str);
            if(token != ELSE)
                return 0;
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
                return 0;
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
            return 0;
    }
}

// jeste chybi parametry funkci nejak osefovat
