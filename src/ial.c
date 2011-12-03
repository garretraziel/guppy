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
#include <string.h>
#include "ial.h"
#include "string.h"
#include "defines.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/** Funkce radici znaky ve stringu metodou quicksort
 *
 * @param *str ukazatel na pole znaku, ktery ma byt serazeno
 * @param a znaky v intervalu <a,b> budou razeny
 * @param b
 */
static inline void quicksort(char *str, int a, int b)
{
    int l = a;
    int r = b;

    // TODO: vymyslet lepsi pivot
    int pivot = str[(a+b)/2];
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


// Globalni tabulky symbolu
FunctionTree *functions_table = NULL;
LiteralTree *literals_table = NULL;

// Ukazatel na posledni pridanou funkci
FunctionTree *last_function = NULL;
LocalTree *last_local = NULL;

/*
 * Prida funckci do tabulky funkci,
 * pokud jiz takova funkce existuje, vraci chybu, jinak 0
 */
static inline int insert_function__(FunctionTree **root, char *str)
{
    int cmp;
    if(*root == NULL) {
        FunctionTree *new = malloc(sizeof(FunctionTree));
        if(new == NULL)
            return ERROR_GEN_MEM;
        *root = new;
        new->name = str; // nebo kopirovat?
        new->symbols = NULL;
        new->syms = 0;
        new->left = NULL;
        new->right = NULL;
        last_function = new;
        return 0;
    }
    cmp = strcmp(str, (*root)->name);
    if(cmp < 0)
        return insert_function__(&(*root)->left, str);
    else if(cmp > 0)
        return insert_function__(&(*root)->right, str);
    else // == 0
        return ERROR_SYN_FUNC_REDEF;
}


/*
 * Wrapper
 */
int insert_function(char *str)
{
    return insert_function__(&functions_table, str);
}



/*
 * Vlozi literal do tabulky literalu
 * Vklada se s nahodnym klicem s virou, ze to bude vytvaret nahodne dobre
 * vyvazeny strom
 */
static inline int insert_literal__(LiteralTree **root, int key, Data data, LiteralTree **dst)
{
    if(*root == NULL) {
        LiteralTree *new = malloc(sizeof(LiteralTree));
        if(new == NULL)
            return ERROR_GEN_MEM;
        *root = new;
        new->key = key;
        new->data = data;
        new->left = NULL;
        new->right = NULL;
        *dst = new;
        return 1;
    }
    if(key < (*root)->key)
        return insert_literal__(&(*root)->left, key, data, dst);
    else if(key > (*root)->key)
        return insert_literal__(&(*root)->right, key, data, dst);
    else
        return insert_literal__(&(*root)->left, key-1, data, dst);
}

/*
 * Wrapper pro vlozeni literalu do tabulky
 *
 * do *dst je ulozen ukazatel na vlozeny prvek
 */
int insert_literal(Data data, LiteralTree **dst)
{
    return insert_literal__(&literals_table, rand(), data, dst);
}


/*
 * Prida do tabulky symbolu jmeno indentifikatoru
 * kazda funkce ma svuj
 */
static inline int insert_local__(LocalTree **root, char *str, int offset)
{
    int cmp;
    if(*root == NULL) {
        LocalTree *new = malloc(sizeof(LocalTree));
        if(new == NULL)
            return ERROR_GEN_MEM;
        *root = new;
        new->name = str;
        new->offset = offset;
        new->left = NULL;
        new->right = NULL;
        last_local = new;
        return 1;
    }
    cmp = strcmp(str, (*root)->name);
    if(cmp < 0)
        return insert_local__(&(*root)->left, str, offset);
    else if(cmp > 0)
        return insert_local__(&(*root)->right, str, offset);
    else // == 0
        return ERROR_SEM_VAR_REDEF;
}

int insert_local(char *str)
{
    if(find_function(str) != NULL)
        return ERROR_SEM_VAR_REDEF;
    return insert_local__(&last_function->symbols, str, ++last_function->syms);
}



/*
 * Rekurzivni pruchod stromem a uvolneni kazdeho uzlu
 */
static inline void drop_locals(LocalTree *root)
{
    if(root != NULL) {
        drop_locals(root->left);
        drop_locals(root->right);
        free(root->name);
        free(root);
    }
}

/*
 * Rekurzivni pruchod stromem a uvolneni kazdeho uzlu
 */
static inline void drop_functions__(FunctionTree *root)
{
    if(root != NULL) {
        drop_functions__(root->left);
        drop_functions__(root->right);
        drop_locals(root->symbols);
        free(root->name);
        free(root);
    }
}

/*
 * Smaze strom funkci, nenastavuje null
 */
void drop_functions(void)
{
    drop_functions__(functions_table);
}


/*
 * Rekurzivni pruchod stromem, uvolneni kazdeho uzlu
 */
static inline void drop_literals__(LiteralTree *root)
{
    if(root != NULL) {
        drop_literals__(root->left);
        drop_literals__(root->right);
        if(root->data.type == T_STRING)
            free(root->data.value.str);
        free(root);
    }
}

/*
 * Wrapper pro smazani stromu literalu
 */
void drop_literals(void)
{
    drop_literals__(literals_table);
}


/*
 * Vyhleda v tabulce funkci funkci a vrati ukazatel na ni
 * pokud nenajde, vraci NULL
 */
static inline FunctionTree * find_function__(FunctionTree *root, char *str)
{
    if(root == NULL)
        return NULL; // tohle je, ze nenasel
    int cmp = strcmp(str, root->name);
    if(cmp == 0)
        return root;
    else if(cmp < 0)
        return find_function__(root->left, str);
    else // cmp > 0
        return find_function__(root->right, str);
}

/*
 * Wrapper pro vyhledani funkce v tabulce
 */
FunctionTree * find_function(char *str)
{
    return find_function__(functions_table, str);
}


/*
 * Vyhleda v tabulce posledni funkce lokalni promennou a vrati ukazatel na ni
 * pokud nenajde, vraci NULL
 */
static inline LocalTree * find_local__(LocalTree *root, char *str)
{
    if(root == NULL)
        return NULL; // tohle je, ze nenasel
    int cmp = strcmp(str, root->name);
    if(cmp == 0)
        return root;
    else if(cmp < 0)
        return find_local__(root->left, str);
    else // cmp > 0
        return find_local__(root->right, str);
    return 0;
}

/*
 * Wrapper pro hledani lokalnich promennych
 */
LocalTree * find_local(char *str)
{
    return find_local__(last_function->symbols, str);
}

#ifdef DEBUG

#include <stdio.h>
void print_locals(LocalTree *root)
{
    if(root == NULL)
        return;
    print_locals(root->left);
    printf("  %s (%2d)\n", root->name, root->offset);
    print_locals(root->right);
}

void print_functions(FunctionTree *root)
{
    if(root == NULL)
        return;
    print_functions(root->left);
    printf("%s\n", root->name);
    print_locals(root->symbols);
    print_functions(root->right);
}

#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

///FIXME: toto bude asi vysledny pouzity KMP algoritmus, ale zatim je to
// horsi jak cerna magie, pancto cerna magie aspon muze fungovat.
// ve zkratce je to opsany pochybny pascalovsky zapis, kde se vse cisluje
// od jednicky (i nula je cislovana od jednicky) a proto v nem nikdy nic nefunguje
/** Funkce, ktera pro zadany retezec vygeneruje delky predpon
 *
 * @param str ukazatel na string, pro ktery tabulku generuji
 * @param len delka predavaneho stringu
 *
 * @return ukazatel na tabulku s delkami predpon
 */
int *find_prefix(char *str, int len)
{
    int *tabulka = malloc(sizeof(int)*len); //TODO: v minulem algoritmu pouzili pole o jedna vetsi. proc?

    if (tabulka == NULL) return NULL;

    tabulka[0] = -1;

    for (int k = 1; k<len; k++) {
        int r = tabulka[k-1];
        while (r>-1 && str[r]!=str[k-1]) r = tabulka[r];
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

    if (tabulka == NULL) return -2;

    int i = 0;
    int j = 0;

    while (i<len1 && j<len2) {
        if ((j == -1) || (str1[i] == str2[j])) {
            i++; j++;
        } else {
            j = tabulka[j];
        }
    }

    free(tabulka);
    
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
    if (str2->length == 0) return 0;
    if (str1->length == 0) return -1; //TODO: vraci false
    int pozice = find_kmp(str1->str, str1->length, str2->str, str2->length);
    if (pozice == -1) return -1; //TODO: vraci false
    else if (pozice == -2) return -2; //TODO: vracet error o nedostatku pameti
    else return pozice+1;
}

//FIXME: nasledujici funkce je deprecated. az si budu jist s p. JMH, smazu to
int *find_prefix2(char *str, int len)
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

//FIXME: nasledujici funkce je deprecated. az si budu jist s p. JMH, smazu to
int find_kmp2(char *str1, int len1, char *str2, int len2)
{
    int *tabulka = find_prefix2(str2, len2);
    
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

//FIXME: nasledujici funkce je deprecated. az si budu jist s p. JMH, smazu to
int find2(string *str1, string *str2)
{
    if (str2->length == 0) return 0;
    if (str1->length == 0) return -1; //TODO: vraci false
    int pozice = find_kmp2(str1->str, str1->length, str2->str, str2->length);
    if (pozice == -1) return -1; //TODO: vraci false
    else if (pozice == -2) return -2; //TODO: vracet error o nedostatku pameti
    else return pozice+1;
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
