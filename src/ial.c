/*
 * projekt: interpret jazyka IFJ11 
 * soubor: ial.c
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#include <stdlib.h>
#include "ial.h"
#include "string.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** Funkce radici znaky ve stringu metodou quicksort
 *
 * @param *str ukazatel na pole znaku, ktery ma byt serazeno
 * @param a znaky v intervalu <a,b> budou razeny
 * @param b
 */
void quicksort(char *str, int a, int b)
{
    int l = a;
    int r = b;

    // TODO: vymyslet lepsi pivot
    int pivot = str[a];
    char tmp;

    // dokud se neprekrizi ukazatele
    while(l < r) {
        // nalezeni prvku k vymene
        while(str[l] < pivot) l++;
        while(str[r] > pivot) r--;
        // pokud nejsou prekrizene...
        if(l <= r) {
            // ...tak vymena...
            tmp = str[l];
            str[l] = str[r];
            str[r] = tmp;
            // ...a nezapomenout posunout ukazatele
            l++;
            r--;
        }
    }
    // ukoncovaci podminky a rekurze
    if(a < r)
        quicksort(str, a, r);
    if(l < b)
        quicksort(str, l, b);
}

/** Wrapper funkce pro quicksort
 *
 * @param *str ukazatel na string, jehoz znaky budou razeny
 */
void sort(string *str)
{
    quicksort(str->str, 0, str->length-1);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// TODO: tabulka symbolu pomoci BST

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// TODO: vyhledavani podretezce (Knuth-Moris-Pratt)

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
