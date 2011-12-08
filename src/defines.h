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
    ERROR_LEX_X_CMNT_END = -104,
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
    ERROR_SYN_MAIN= -215,
    ERROR_SYN_X_UNTIL= -216,
} ESynErrors;

// [3]
enum {
    ERROR_SEM_VAR_UND = -301,
    ERROR_SEM_VAR_REDEF = -302,
    ERROR_SEM_WRONG_TYPES = -303,
    ERROR_SEM_CALL_VAR = -304,
    ERROR_SEM_FUNC_VAR = -305,
    ERROR_SEM_WRITE_WRPR = -306,
    ERROR_SEM_READ_WRPR = -307,
    ERROR_SYN_FUNC_REDEF = -308,
} ESemErrors;

// [4]
enum {
    ERROR_INT_EMPTY_STACK = -401,
    ERROR_INT_DIVZERO = -402,
    ERROR_INT_BADPARAM = -403,
    ERROR_INT_INCOMP_TYPES = -404,
    ERROR_INT_BAD_INSTR = -405,
    ERROR_INT_BAD_VAL = -406,
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

#define STRUE 1
#define SFALSE 0

#define STACK_DEFAULT_SIZE 1000 //TODO: chci slyset vase navrhy

// Makro ktere provede prikaz, pokud skonci zapornym navratovym kodem, bude
// proveden return teto hodnoty, jinak nic
#define try(command) do { int x = command; if(x<0) return x; } while(0)


#endif
