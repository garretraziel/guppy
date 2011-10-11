/*
 * projekt: interpret jazyka IFJ11 
 * soubor: main.c
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#include <stdio.h>
#include <stdarg.h>

#include "guppy.h"


/// Zprava napovedy.
const char HELP_MSG[] =
    "ifj11 - interpret jazyka ifj11\n"
    "pouziti:\n"
    "    ifj11 SOUBOR \n";


/// Funkce vypisuje chybova hlaseni
void Error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "CHYBA: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
}


/// Funkce main
int main(int argc, char *argv[])
{
    if(argc != 2) {
        Error("Chybny pocet parametru.\n\n");
        fprintf(stderr, HELP_MSG);
        return ERROR_FAIL;
    }

    FILE *file = fopen(argv[1], "r");
    if(file == NULL) {
        Error("Nepodarilo se otevrit soubor %s\n)", argv[1]);
        return ERROR_FAIL;
    }
    fclose(file);

    return 0;
}
