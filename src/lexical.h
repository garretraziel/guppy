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

enum {
    NOTHING,
    NUMBER,
    OPERATOR,
    STRING,
    IDENTIFIER,
    SEMICOLON,
} TToken;

int get_token(FILE *);

#endif
