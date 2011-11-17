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
#ifdef DEBUG
#include <stdio.h>
#endif

#include "string.h"

// interni funkce, zjisti, zda jsou indexy spravne
int index_sane(int x, int y, int len);

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
    char *retstr;
    /// zkontroluji, zda jsou zadany spravne argumenty
    if (!index_sane(from, to, len)) {
	retstr = malloc(sizeof(char));
	if (retstr == NULL) return NULL;
	retstr[0] = '\0';
	return retstr;
    }

    // nyni uz vim, ze argumenty jsou spravne, tak mohu v klidu vypisovat
    int x = from;
    int y = to;

    //TODO: vymyslet a osetrit vsechny hovadiny, treba prvni index mensi jak druhy a podobne
    if (from < 0) {
	x = len + from; // prepocitam index pro zaporne cislo
	if (x < 0) x = 0;
    }

    if (to < 0) {
	y = len + to; // prepocitam index pro zaporne cislo
	if (y < 0) {
#ifdef DEBUG
	    fpritnf(stderr,"Warning: substr by vyjel ze stringu doleva.\n");
#endif
	    y = 0;
	}
    }

    /* if (y < x) { // jeste musim mit mensi index jako prvni ///FIXME: nejspis neni potreba !! */
    /* 	int temp = x; */
    /* 	x = y; */
    /* 	y = temp; */
    /* } */

    if (y >= len) y = len - 1;

    int retlen = y - x + 1; // velikost x az y vcetne

    //TODO: zkontrolovat jestli opravdu alokuju spravne
    retstr = malloc(sizeof(char)*(retlen+1)); // jeden kvuli \0

    if (retstr == NULL) return NULL;
    
    int j = 0;
    for (int i = x; i<=y; i++) {
	if (str[i] == '\0') {
	    /// pro jistotu, sem by se program nemel dostat
#ifdef DEBUG
	    fprintf(stderr,"Warning: substr by vyjel ze stringu.\n");
#endif
	    retstr[j] = '\0';
	    return retstr;
	}
	retstr[j++] = str[i];
    }

    retstr[j] = '\0';
    
    return retstr;
}

// interni funkce, zjisti, zda jsou indexy spravne
int index_sane(int x, int y, int len)
{
    if (x >= 0 && y >= 0 && x > y) return 0; // napriklad substring("neco",5,4)
    if (x < 0 && y < 0 && x > y) return 0; // napriklad substring("neco",-2,-3)
    if (x < 0 && y >= 0 && (x + len) > y) return 0; // napriklad substring("neco",-1, 2) //TODO: zkontrolovat, treba jestli se muzou i rovnat a tak
    if (x >= 0 && y < 0 && x > (y + len)) return 0; // napriklad substring("neco",3,-2) //TODO: taktez
    if (x < 0 && y < 0 && (x + len) < 0 && (y + len) < 0) return 0;
    if (x >= len) return 0;

    //TODO: zkontrolovat, zda nemaji byt nejaka dalsi pravidla
    
    return 1;
}
