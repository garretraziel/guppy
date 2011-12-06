/*
 * projekt: interpret jazyka IFJ11 
 * soubor: errors.c 
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#include "defines.h"
#include "guppy.h"

// [1] Chybova hlaseni k lexikalnim chybam
const char *LEX_ERRORS[] = {
    [(-ERROR_LEX_NUMBER) % 100] = "Neplatny ciselny literal",
    [(-ERROR_LEX_UX_CHAR) % 100] = "Neocekavany znak",
    [(-ERROR_LEX_ESC_SEC) % 100] = "Neplatna escape sekvence v retezci",
    [(-ERROR_LEX_X_CMNT_END) % 100] = "Neukonceny komentar",
};

// [2] Chybova hlseni syntaktickych chyb
const char *SYN_ERRORS[] = {
    [(-ERROR_SYN_UX_TOKEN) % 100] = "Neocekavany token",
    [(-ERROR_SYN_X_EOF) % 100] = "Ocekavan konec zdrojoveho souboru",
    [(-ERROR_SYN_X_FUNC) % 100] = "Ocekavana definice funkce",
    [(-ERROR_SYN_X_IDENT) % 100] = "Ocekavan identifikator",
    [(-ERROR_SYN_X_SMCLN) % 100] = "Ocekavan znak ;",
    [(-ERROR_SYN_X_END) % 100] = "Ocekavano END",
    [(-ERROR_SYN_X_ASGN) % 100] = "Ocekavan operator =",
    [(-ERROR_SYN_X_LBRC) % 100] = "Ocekavan znak (",
    [(-ERROR_SYN_X_RBRC) % 100] = "Ocekavan znak )",
    [(-ERROR_SYN_X_THEN) % 100] = "Ocekavano THEN",
    [(-ERROR_SYN_X_ELSE) % 100] = "Ocekavano ELSE",
    [(-ERROR_SYN_X_DO) % 100] = "Ocekavano DO",
    [(-ERROR_SYN_X_OPRTR) % 100] = "Ocekavan operator",
    [(-ERROR_SYN_EXP_FAIL) % 100] = "Neplatny vyraz",
    [(-ERROR_SYN_FUNC_REDEF) % 100] = "Funkce je jiz definovana",
    [(-ERROR_SYN_MAIN) % 100] = "Funkce main neni definovana nebo neni definovana jako posledni",
    [(-ERROR_SYN_X_UNTIL) % 100] = "Ocekavano UNTIL",
};


// [3] Chybove zpravy semanticke analyzy
const char *SEM_ERRORS[] = {
     [(-ERROR_SEM_VAR_UND) % 100] = "Nedeklarovana promenna",
     [(-ERROR_SEM_VAR_REDEF) % 100] = "Promenna je jiz definovana",
     [(-ERROR_SEM_WRONG_TYPES) % 100] = "Nekompatibilni typy operandu ve vyrazu",
     [(-ERROR_SEM_CALL_VAR) % 100] = "Nelze volat promenne",
     [(-ERROR_SEM_FUNC_VAR) % 100] = "Funkce nelze pouzivat jako promenne",
};

// [4] Obecne chybove zpravy interpretace
const char *INT_ERRORS[] = {
     [(-ERROR_INT_X) % 100] = "Chyba interpretace",
};

// [5] Obecne chybove zpravy
const char *GEN_ERRORS[] = {
    [(-ERROR_GEN_PARAM) % 100] = "Chybny pocet parametru.\n",
    [(-ERROR_GEN_FILE) % 100] = "Nepodarilo se otevrit soubor %s",
    [(-ERROR_GEN_MEM) % 100] = "Nedostatek pameti",
};

// Pole vsech chybovych zprav
const char **ERROR_MSG[] = {
    [ERROR_LEX] = LEX_ERRORS,
    [ERROR_SYN] = SYN_ERRORS,
    [ERROR_SEM] = SEM_ERRORS,
    [ERROR_INT] = INT_ERRORS,
    [ERROR_GEN] = GEN_ERRORS,
};
