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

// TODO: tabulka symbolu pomoci BST

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/// Vyhleda podretezec v retezci pomoci metody Knuth-Morrison-Pratt
int find(string *, string *);

//TODO: deprecated, odstranit
int find2(string *, string *);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#endif
