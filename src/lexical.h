/*
 * projekt: interpret jazyka IFJ11 
 * soubor: lexical.h
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#ifndef GUPPY_LEXICAL_H
#define GUPPY_LEXICAL_H

#include "string.h"


#define ASCII_CONTROLL 32

/// Vycet navratovych kodu lexikalniho analyzatoru
enum {
    NOTHING = 0, // znaci EOF

    NUMBER,
    STRING,
    IDENTIFIER,

    SEMICOLON,

    PLUS,
    MINUS,
    DIV,
    MUL,
    POWER,
    STRCONCAT,

    ASSIGN,
    LESS,
    GREAT,
    LESS_EQ,
    GREAT_EQ,
    EQUAL,
    NOT_EQUAL,

    COMMA,
    LBRAC,
    RBRAC,    

// klicova slova
    DO,
    ELSE,
    END,
    FALSE,
    FUNCTION,
    IF,
    LOCAL,
    NIL,
    READ,
    RETURN,
    THEN,
    TRUE,
    WHILE,
    WRITE,

// rezervovana slova
    AND,
    BREAK,
    ELSEIF,
    FOR,
    IN,
    NOT,
    OR,
    REPEAT,
    UNTIL,
} TToken;



/// Vrati nasledujici token ze souboru
void get_token(void);

/// globalni pocitadlo radku
extern int line;

/// globalni reprezentace prave nacteneho tokenu
extern int token;
extern string str;

/// vstupni soubor
extern FILE *input;

#endif
