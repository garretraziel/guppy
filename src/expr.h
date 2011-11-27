/*
 * projekt: interpret jazyka IFJ11 
 * soubor: ial.h
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#ifndef IJC_EXPR_H
#define IJC_EXPR_H

enum {
    LT, // less than
    GT, // greater than
    EQ, // equal
} VTabValues;

int ParseExpr(void);

#endif
