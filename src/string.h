/*
 * projekt: interpret jazyka IFJ11 
 * soubor:  
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#ifndef IJC_STRING_H
#define IJC_STRING_H

#define STR_DEFAULT 255

typedef struct {
    char *str;
    int length;
    int alloc;
} string;


int str_new(string *, int);
int str_push(string *, char);
char *str_read(string *);

#endif
