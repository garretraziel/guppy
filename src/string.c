/*
 * projekt: interpret jazyka IFJ11 
 * soubor: string.c
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#include <stdlib.h>

#include "string.h"


// Inicializace noveho stringu
int str_new(string *str, int size)
{
    str->str = malloc(size * sizeof(char));
    if(str->str == NULL)
        return 0; // TODO celkem fatalni chyba :( jak to osetrit?
    str->alloc = size;
    str->length = 0;
    str->str[0] = 0;
    return 1;
}

/// Vlozi dalsi pismeno na konec stringu, pripadne zvetsi
int str_push(string *str, char c)
{
    char *tmp;
    // pokud neni dost mista, je potreba rozsirit
    if(str->alloc == str->length + 1) { 
        tmp = realloc(str->str, str->alloc << 1);
        if(tmp == NULL)
            return 0; // TODO celkem fatalni chyba :( jak to osetrit?
        str->str = tmp;
        str->alloc <<= 1;
    }
    str->str[str->length] = c;
    str->length += 1;
    str->str[str->length] = 0;
    return 1;
}

/// Uvolneni pameti stringu
void str_free(string *str)
{
    free(str->str);
    str->str = NULL;
    str->alloc = 0;
    str->length = 0;
}

/// Vymazani stringu
void str_clean(string *str)
{
    if(str->alloc > 0)
        str->str[0] = 0;
    str->length = 0;
}
