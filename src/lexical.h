/*
 * projekt: interpret jazyka IFJ11 
 * soubor: lexical.h
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#ifndef IJC_LEXICAL_H
#define IJC_LEXICAL_H

#include "string.h"


// TODO: rozeznani ruznych typu chyb, co mohou nastat (LEX a pamet me napada)

enum {
    ERROR = 0,

    NOTHING = 0,

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
    TILDA,
    EQUAL,
    NOT_EQUAL,

    COMMA,
    LBRAC,
    RBRAC,    

    FUNCTION,
    END,
} TToken;

int get_token(FILE *, string *);

#endif
