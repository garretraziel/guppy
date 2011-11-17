/*
 * projekt: interpret jazyka IFJ11 
 * soubor: guppy.c
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#include <stdio.h>
#include <stdarg.h>

#include "guppy.h"
#include "lexical.h"
#include "parser.h"

/// Zprava napovedy.
const char HELP_MSG[] =
    "ifj11 - interpret jazyka ifj11\n"
    "pouziti:\n"
    "    ifj11 SOUBOR \n";


// Chybove zpravy
const char *GEN_ERRORS[] = {
    [(-ERROR_GEN_PARAM) % 100] = "Chybny pocet parametru.\n",
    [(-ERROR_GEN_FILE) % 100] = "Nepodarilo se otevrit soubor %s",
    [(-ERROR_GEN_MEM) % 100] = "Nedostatek pameti",
};


// Pole vsech chybovych zprav
const char **ERROR_MSG[] = {
    [ERROR_LEX] = LEX_ERRORS,
    [ERROR_SYN] = SYN_ERRORS,
//    [ERROR_SEM] = SEM_ERRORS,  TODO: odkomentovat, az budou
//    [ERROR_INT] = INT_ERRORS,  TODO: odkomentovat, az budou
    [ERROR_GEN] = GEN_ERRORS,
};


/// Funkce vypisuje chybova hlaseni
void Error(int errcode, ...)
{
    va_list args;
    va_start(args, errcode);
    fprintf(stderr, "CHYBA: ");
    int type = (-errcode) / 100;
    if(type < ERROR_GEN)
        fprintf(stderr, "Na radku %d: ", line);
    vfprintf(stderr, ERROR_MSG[type][(-errcode) % 100], args);
    fprintf(stderr, "\n");
    va_end(args);
}

/// Funkce main
int main(int argc, char *argv[])
{
    if(argc != 2) {
        Error(ERROR_GEN_PARAM);
        fprintf(stderr, HELP_MSG);
        return ERROR_GEN;
    }

    FILE *file = fopen(argv[1], "r");
    if(file == NULL) {
        Error(ERROR_GEN_FILE, argv[1]);
        return ERROR_GEN;
    }

    int x = program(file);
    if(x < 0) {
        Error(x);
        return (-x) / 100;
    }

    fclose(file);

    return 0;
}
