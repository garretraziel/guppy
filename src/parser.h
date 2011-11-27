/*
 * projekt: interpret jazyka IFJ11 
 * soubor: parser.h
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#ifndef IFJ_PARSER_H
#define IFJ_PARSER_H

enum {
    ERROR_SYN_UX_TOKEN = -201,
    ERROR_SYN_X_SMCLN = -202,
    ERROR_SYN_X_EOF = -203,
    ERROR_SYN_X_FUNC = -204,
    ERROR_SYN_X_IDENT = -205,
    ERROR_SYN_X_END = -206,
    ERROR_SYN_X_ASGN = -207,
    ERROR_SYN_X_LBRC = -208,
    ERROR_SYN_X_RBRC = -209,
    ERROR_SYN_X_THEN = -210,
    ERROR_SYN_X_ELSE = -211,
    ERROR_SYN_X_DO = -212,
    ERROR_SYN_X_OPRTR = -213,
} ESynErrors;

// Zpracuje pocatecni neterminalni symbol
int program(FILE *);

extern const char *SYN_ERRORS[];

#endif
