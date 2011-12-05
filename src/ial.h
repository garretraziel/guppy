/*
 * projekt: interpret jazyka IFJ11 
 * soubor: ial.h
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#ifndef GUPPY_IAL_H
#define GUPPY_IAL_H

#include "string.h"
#include "exec.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/// Seradi znaky v retezci vzestupne podle ordinalni hodnoty
void sort(string *);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

typedef union {
    int log;
    double num;
    char *str;
    void *adr;
} univalue;

// univerzalni datova polozka
typedef struct {
    int type;
    univalue value;
} Data;

// uzel tabulky symbolu funkce
typedef struct localnode {
    char *name;
    int offset;
    struct localnode *left, *right;
} LocalTree;

// uzel tabulky funkci
typedef struct funcnode {
    char *name;
    LocalTree *symbols;
    int params, vars;
    PTapeItem adr;
    struct funcnode *left, *right;
} FunctionTree;

// uzel tabulky literalu
typedef struct literalnode {
    int key;
    Data data;
    struct literalnode *left, *right;
} LiteralTree;


int insert_function(char *, PTapeItem);
int insert_literal(Data);
int insert_local_param(char *);
int insert_local_var(char *);

void drop_functions(void);
void drop_literals(void);

void recount_offsets(void);

FunctionTree * find_function(char *);
LocalTree * find_local(char *);

// globalni tabulky
extern FunctionTree *functions_table;
extern LiteralTree *literals_table;

// posledni funkce
extern FunctionTree *last_function;
// posledni lokalni promenna
extern LocalTree *last_local;
// posledni litral
extern LiteralTree *last_literal;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/// Vyhleda podretezec v retezci pomoci metody Knuth-Morrison-Pratt
int find(string *, string *);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif
