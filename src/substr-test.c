#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ial.h"
#include "string.h"

int main(void)
{
    char *vysledek;

    string str;
    str_init(&str, "Hello, world");
    

    printf("Pracuji se retezcem: \"%s\"\n",str.str);

    vysledek = substr(&str, 1, 6);

    printf("Retezec od 1 do 6 vcetne, melo by byt \"Hello,\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr(&str, 8, 12);

    printf("Retezec od 8 do 12 vcetne, melo by byt \"world\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr(&str, 1, 13);

    printf("Retezec od 1 do 13 vcetne, melo by byt \"Hello, world!\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr(&str, 1, 20);

    printf("Retezec od 1 do 20 vcetne, melo by byt \"Hello, world!\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr(&str, -6, -1);

    printf("Retezec od -6 do -1 vcetne, melo by byt \"world!\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr(&str, 5, 4);

    printf("Retezec od 5 do 4 vcetne, melo by byt \"\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr(&str, -4, -5);

    printf("Retezec od -4 do -5 vcetne, melo by byt \"\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr(&str, -1, 3);

    printf("Retezec od -1 do 3 vcetne, melo by byt \"\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr(&str, 10, -6);

    printf("Retezec od 10 do -6 vcetne, melo by byt \"\": %s\n",  vysledek);
    free(vysledek);

    str_free(&str);
    str_init(&str,"abc");

    vysledek = substr(&str, -4, -4);

    printf("Retezec od -4 do -4 vcetne, melo by byt \"\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr(&str, -3, -3);

    printf("Retezec od -3 do -3 vcetne, melo by byt \"a\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr(&str, -4, -3);

    printf("Retezec od -4 do -3 vcetne, melo by byt \"a\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr(&str, 3, 6);

    printf("Retezec od 2 do 5 vcetne, melo by byt \"c\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr(&str, 4, 4);

    printf("Retezec od 4 do 4 vcetne, melo by byt \"\": %s\n",  vysledek);
    free(vysledek);

    str_free(&str);

    return 0;
}
