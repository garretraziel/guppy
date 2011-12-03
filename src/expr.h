/*
 * projekt: interpret jazyka IFJ11 
 * soubor: expr.h
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#ifndef GUPPY_EXPR_H
#define GUPPY_EXPR_H

enum {
    LT, // less than
    GT, // greater than
    EQ, // equal
    OO, // nic
} VTabValues;

int expression(void);

#endif
