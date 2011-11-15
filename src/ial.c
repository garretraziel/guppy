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
#include <string.h> //TODO: pouzivat? nebo definovat vlastni strlen a strcmp?
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

/** Funkce, ktera pro zadany retezec vygeneruje delky predpon
 *
 * @param str ukazatel na string, pro ktery tabulku generuji
 * @param len delka predavaneho stringu
 *
 * @return ukazatel na tabulku s delkami predpon
 */
int *find_prefix(char *str, int len)
{
    int *tabulka = malloc(sizeof(int)*(len+1)); /// pro zjednoduseni nechame misto i pro '\0'

    if (tabulka == NULL) {
	return NULL;
    }
    
    int i = 0;
    int j = tabulka[0] = -1;
    
    while (i < len) {
	while (j > -1 && str[i] != str[j])
	    j = tabulka[j];
	i++;
	j++;
	if (str[i] == str[j])
	    tabulka[i] = tabulka[j];
	else
	    tabulka[i] = j;
    }
    
    return tabulka;
}

///FIXME: toto bude asi vysledny pouzity KMP algoritmus, ale zatim je to
// horsi jak cerna magie, pancto cerna magie aspon muze fungovat.
// ve zkratce je to opsany pochybny pascalovsky zapis, kde se vse cisluje
// od jednicky (i nula je cislovana od jednicky) a proto v nem nikdy nic nefunguje
int *find_prefix2(char *str, int len)
{
    int *tabulka = malloc(sizeof(int)*len);

    if (tabulka == NULL) return NULL;

    tabulka[0] = 0;

    for (int k = 1; k<len; k++) {
	int r = tabulka[k-1];
	while (r>0 && str[r]!=str[k-1]) r = tabulka[r];
	tabulka[k] = r+1;
    }

    return tabulka;
}

/** Funkce, ktera pro zadany string najde zadany podstring pomoci Knuth-Morrison-Prattovy metody
 *
 * @param str1 ukazatel na string, ve kterem budu vyhledavat
 * @param len1 delka prvniho stringu
 * @param str2 ukazatel na string, ktery budu hledat
 * @param len2 delka druheho stringu
 *
 * @return pozice, kde se vyskytuje dany podretezec v danem retezci \
 *         cislovano od 0. -1 pokud se v danem retezci nenachazi, -2 pri nedostatku pameti
 */
int find_kmp(char *str1, int len1, char *str2, int len2)
{
    int *tabulka = find_prefix(str2, len2);
    
    if (tabulka == NULL) return -2; //TODO: vracet opravdovou chybu
    
    int i = 0;
    int j = 0;
    
    while (j < len1) {
	while (i > -1 && str2[i] != str1[j])
	    i = tabulka[i];
	i++;
	j++;
	if (i >= len2) {
	    free(tabulka);
	    return j-i;
	}
    }
    
    free(tabulka);
    return -1;
}

///TODO: jak jsem jiz napsal v poslednim komentari zde^^^
int find_kmp2(char *str1, int len1, char *str2, int len2)
{
    int *tabulka = find_prefix2(str2, len2);

    if (tabulka == NULL) return -2;

    int i = 0;
    int j = 0;

    while (i<len1 && j<len2) {
	if (j == -1 || str1[i] == str2[j]) {
	    i++; j++;
	} else {
	    j = tabulka[i];
	}
    }

    if (j >= len2) return i - len2;
    else return -1;
}

/** Wrapper funkce find
 *
 * @param *str1 ukazatel na string, ve kterem hledam
 * @param *str2 ukazatel na string, ktery hledam
 *
 * @return pozice, kde se vyskytuje dany podretezec v danem retezci \
 *         cislovano od 1. -1 pokud se v danem retezci nenachazi, -2 pri nedostatku pameti
 */
int find(string *str1, string *str2)
{
  if (strcmp(str2->str,"") == 0) return 0;
  if (strcmp(str1->str,"") == 0) return -1; //TODO: vraci false
  int pozice = find_kmp(str1->str, str1->length, str2->str, str2->length);
  if (pozice == -1) return -1; //TODO: vraci false
  else if (pozice == -2) return -2; //TODO: vracet error o nedostatku pameti
  else return pozice+1;
}

///TODO: tady uz nemam co rict
int find2(string *str1, string *str2)
{
  if (strcmp(str2->str,"") == 0) return 0;
  if (strcmp(str1->str,"") == 0) return -1; //TODO: vraci false
  int pozice = find_kmp2(str1->str, str1->length, str2->str, str2->length);
  if (pozice == -1) return -1; //TODO: vraci false
  else if (pozice == -2) return -2; //TODO: vracet error o nedostatku pameti
  else return pozice+1;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
