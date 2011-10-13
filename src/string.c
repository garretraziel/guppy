/*
 * projekt: interpret jazyka IFJ11 
 * soubor: string.c
 * autori:
 *   xbrabe09 Brabec Lukas
 *   xdujic01 Dujicek Ales
 *   xmokra00 Mokra Radka
 *   xsedla85 Sedlak Jan
 */

#include <stdlib.h>

#include "string.h"


int str_new(string *str, int size)
{
    str->str = malloc(size * sizeof(char));
    if(str->str == NULL)
        return 0;
    str->alloc = size;
    str->length = 0;
    str->str[0] = 0;
    return 1;
}

int str_push(string *str, char c)
{
    //ptr = realloc(ptr, size)
    return 0;
}

char *str_read(string *str)
{
    return str->str;
}
