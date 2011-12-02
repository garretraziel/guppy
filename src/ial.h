/*
 * projekt: interpret jazyka IFJ11 
 * soubor: ial.h
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#ifndef IJC_IAL_H
#define IJC_IAL_H

#include "string.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/// Seradi znaky v retezci vzestupne podle ordinalni hodnoty
void sort(string *);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// univerzalni datova polozka
typedef struct {
    int type;
    union {
        int log;
        double num;
        char *str;
    } value;
} Data;

// uzel tabulky symbolu funkce
typedef struct localnode {
    char *name;
    int offset;
    Data data;
    struct localnode *left, *right;
} LocalTree;

// uzel tabulky funkci
typedef struct funcnode {
    char *name;
    LocalTree *symbols;
    struct funcnode *left, *right;
} FunctionTree;

// uzel tabulky literalu
typedef struct literalnode {
    int key;
    Data data;
    struct literalnode *left, *right;
} LiteralTree;


int insert_function(char *);
int insert_literal(int, Data);
int insert_local(LocalTree **, char *, int, Data);

int drop_functions(FunctionTree **);
int drop_literals(LiteralTree **);

int find_function(FunctionTree *, const char *);
int find_local(FunctionTree *, const char *);

// globalni tabulky
extern FunctionTree *functions_table;
extern LiteralTree *literals_table;

// posledni funkce
extern FunctionTree *last_function;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/// Vyhleda podretezec v retezci pomoci metody Knuth-Morrison-Pratt
int find(string *, string *);

//TODO: deprecated, odstranit
int find2(string *, string *);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif
