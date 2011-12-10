/*
 * projekt: interpret jazyka IFJ11 
 * soubor: expr.h
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#ifndef GUPPY_EXPR_H
#define GUPPY_EXPR_H

// Symboly v precedencni tabulce
enum {
    LT, // <
    GT, // >
    EQ, // =
    OO, // nic
} VTabValues;


// Symboly se kterymi pracuje zasobnikovy automat
enum {
    E_POW,
    E_MUL,
    E_DIV,
    E_PLUS,
    E_MINUS,
    E_STRCONCAT,
    E_LESS,
    E_GREAT,
    E_LESSEQ,
    E_GREATEQ,
    E_NOTEQ,
    E_EQUAL,
    E_IDENT,
    E_NUM,
    E_STR,
    E_BOOL,
    E_NIL,
    E_LBRAC,           
    E_RBRAC,
    E_COMMA,
    E_DOLLAR,
/*    E_MOD,
    E_STRLEN,
    E_AND,
    E_OR,
    E_NOT, */ // Tohle je do rozsireni, zatim nic

    // Znacka zacatku handle ( < do zasobniku )
    E_MARK, // 21

    // jeste jsou potreba neterminaly
    E_NET_E,
    E_NET_P,

    // tohle jsou jen pomocne konstanty pro oobely-boo a spol.
    E_OP, // obecne operator
    E_LIT, // obecne literal
    E_UNKNOWN, // neznamy typ
    E_VAR, // identifikator promenne
    E_FUNC, // identifikator funkce
    E_FIND, // vestavene funkce
    E_SORT,
    E_SUBSTR,
    E_TYPE,
    // NOTE: zadne dalsi konstanty nepridavat, kvuli kontrole >= E_FUNC
} ESymbols;

int expression(void);

#endif
