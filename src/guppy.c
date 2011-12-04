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
#include "defines.h"
#include "exec.h"
#include "ial.h"

/// Zprava napovedy.
const char HELP_MSG[] =
    "ifj11 - interpret jazyka ifj11\n"
    "pouziti:\n"
    "    ifj11 SOUBOR \n";


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
    // nastaveni vstupu
    input = file;
    // inicializace pasky instrukci
    init_tape();
    int x = parser();
    if(x < 0) {
        Error(x);
        fclose(file);
        delete_tape();
        drop_functions();
        drop_literals();
        return (-x) / 100;
    }
    fclose(file);
    
#ifdef DEBUG
    print_tape();
#endif
    // vyhledove
    execute();

    // uklid tabulky funkci
    drop_functions();
    // uklid tabulky literalu
    drop_literals();
    // uklid seznamu instrukci
    return 0;
}
