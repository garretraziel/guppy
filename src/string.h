/*
 * projekt: interpret jazyka IFJ11 
 * soubor: string.h
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#ifndef GUPPY_STRING_H
#define GUPPY_STRING_H

#define STR_INIT_LEN 16

/// Struktura pro "nekonecny" string
typedef struct {
    char *str;
    unsigned int length;
    unsigned int alloc;
} string;


/// Vytvori novou instanci stringu
int str_new(string *, int);

/// Vytvori novy string a rovnou do nej ulozi nejaky pocatecni obsah
int str_init(string *, const char *);

/// Prida na konec stringu znak
int str_push(string *, char);

/// Destruktor
void str_free(string *);

/// Zkrati string na delku 0
void str_clean(string *);

/// Vrati substring ze stringu podle zadanych indexu
/// POZOR: ocekava indexy v C, pole od 0. o prevod do
/// toho pseudocislovani by se mela postarat wrapper funkce
char *substr_c(char *, int, int, int);

/// Vrati substring ze stringu podle zadanych indexu
char *substr(string *, int, int);

/// Spoji dva stringy do jednoho, ten vrati, stringy free
char *concat(char *, char *);

#endif
