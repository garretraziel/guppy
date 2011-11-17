/*
 * projekt: interpret jazyka IFJ11 
 * soubor: guppy.h
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#ifndef IFJ_GUPPY_H
#define IFJ_GUPPY_H

/// Navratove kody programu
enum {
    ERROR_LEX = 1,
    ERROR_SYN,
    ERROR_SEM,
    ERROR_INT,
    ERROR_GEN
} EErrors;

// Kody chyb
enum {
    ERROR_GEN_PARAM = -501,
    ERROR_GEN_FILE = -502,
    ERROR_GEN_MEM = -503,
} EGenErrors;

#endif
