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

/// Vrati substring ze stringu podle zadanych indexu
/// POZOR: ocekava indexy v C, pole od 0. o prevod do
/// toho pseudocislovani by se mela postarat wrapper funkce
char *substr_c(char *str, int len, int from, int to)
{
    int len2 = abs(from-to) + 2; //TODO: toto neplati pro zaporne indexy a jeste buhvi pro co vsechno

    char *retstr = malloc(sizeof(char)*len2);

    if (retstr == NULL) return NULL;


    ///TODO: tady probehne reseni "nestandardnich" situaci
    
    int j = 0;
    for (int i = from; i<to; i++) {
	///TODO: toto proste dopsat, kopirovat a soustredit se na konce poli
    }
    
    return retstr;
}
