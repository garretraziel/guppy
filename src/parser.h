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
    ERROR_SYN_UX_TOKEN = -1,
    ERROR_SYN_X_SMCLN = -2,
    ERROR_SYN_X_EOF = -3,
    ERROR_SYN_X_FUNC = -4,
    ERROR_SYN_X_IDENT = -5,
    ERROR_SYN_X_END = -6,
    ERROR_SYN_X_ASGN = -7,
    ERROR_SYN_X_LBRC = -8,
    ERROR_SYN_X_RBRC = -9,
    ERROR_SYN_X_THEN = -10,
    ERROR_SYN_X_ELSE = -11,
    ERROR_SYN_X_DO = -12,
    ERROR_SYN_X_OPRTR = -13,
} ESynErrors;

// Zpracuje pocatacni neterminalni symbol
int program(FILE *);

extern const char *SYN_ERRORS[];

#endif
