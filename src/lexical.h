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


#define ASCII_CONTROLL 32

/// Vycet navratovych kodu lexikalniho analyzatoru
// zaporne hodnoty znaci chybu
// nula znamena EOF
// kladne hodnoty validni token
enum {
    ERROR_NUMBER = -1,
    ERROR_UX_CHAR = -2,
    ERROR_ESC_SEC = -3,

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


/// Chybova hlaseni k lexikalnim chybam
extern const char *LEX_ERRORS[];

/// Vrati nasledujici token ze souboru
int get_token(FILE *, string *);

/// Vrati token na vstup
int unget_token(int);

/// globalni pocitadlo radku
extern int line;

#endif
