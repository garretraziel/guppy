/*
 * projekt: interpret jazyka IFJ11 
 * soubor: ial.c
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#include "quicksort.h"
#include "string.h"

/** Funkce radici znaky ve stringu metodou quicksort
 *
 * @param *str ukazatel na string, ktery ma byt serazen
 * @param a znak v intervalu <a,b> budou razeny
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
        // nalezeni prvku v vymene
        while(str[l] < pivot) l++;
        while(str[r] > pivot) r--;
        // pokud nejsou prekrizene, tak vymena
        if(l <= r) {
            // vymena
            tmp = str[l];
            str[l] = str[r];
            str[r] = tmp;
            l++;
            r--;
        }
    }
    if(a < r)
        quicksort(str, a, r);
    if(l < b)
        quicksort(str, l, b);
}

/// Wrapper funkce pro quicksort
void sort(string *str)
{
    printf("Quick zacina pro [%s]\n", str->str);
    quicksort(str->str, 0, str->length-1);
}
