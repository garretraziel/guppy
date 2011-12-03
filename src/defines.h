/*
 * projekt: interpret jazyka IFJ11 
 * soubor: defines.h
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#ifndef GUPPY_DEFINES_H
#define GUPPY_DEFINES_H

// [1]
enum {
    ERROR_LEX_NUMBER = -101,
    ERROR_LEX_UX_CHAR = -102,
    ERROR_LEX_ESC_SEC = -103,
} ELexErrors;

// [2]
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
    ERROR_SYN_EXP_FAIL = -214,
    ERROR_SYN_FUNC_REDEF = -215,
    ERROR_SYN_MAIN= -216,
} ESynErrors;

// [3]
enum {
    ERROR_SEM_VAR_UND = -301,
} ESemErrors;

// [4]
enum {
    ERROR_INT_X = -401,
} EIntErrors;

// [5]
enum {
    ERROR_GEN_PARAM = -501,
    ERROR_GEN_FILE = -502,
    ERROR_GEN_MEM = -503,
} EGenErrors;

// Pole vsech chybovych zprav
extern const char **ERROR_MSG[];


// Vycet datovych typu
enum {
    T_NUMBER,
    T_STRING,
    T_BOOLEAN,
    T_NIL,
} EDataTypes;

#endif
