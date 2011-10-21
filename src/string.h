/*
 * projekt: interpret jazyka IFJ11 
 * soubor: string.h
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#ifndef IJC_STRING_H
#define IJC_STRING_H

#define STR_DEFAULT 255

/// Struktura pro "nekonecny" string
typedef struct {
    char *str;
    unsigned int length;
    unsigned int alloc;
} string;


/// Vytvori novou instanci stringu
int str_new(string *, int);

/// Prida na konec stringu znak
int str_push(string *, char);

/// Vrati ukazatel na uzitecna data NOTE: tohle smazat, imho zbytecne
char *str_read(string *);

/// Destruktor
void str_free(string *);

/// Zkrati string na delku 0
void str_clean(string *);

#endif
