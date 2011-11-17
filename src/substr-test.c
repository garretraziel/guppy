#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ial.h"
#include "string.h"

int main(void)
{
    char *retezec = "Hello, world!";
    char *vysledek;
    int len = strlen(retezec);

    printf("Pracuji se retezcem: \"%s\"\n",retezec);

    vysledek = substr_c(retezec, len, 0, 5);

    printf("Retezec od 0 do 5 vcetne, melo by byt \"Hello,\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr_c(retezec, len, 7, 11);

    printf("Retezec od 7 do 11 vcetne, melo by byt \"world\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr_c(retezec, len, 0, 12);

    printf("Retezec od 0 do 12 vcetne, melo by byt \"Hello, world!\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr_c(retezec, len, 0, 20);

    printf("Retezec od 0 do 20 vcetne, melo by byt \"Hello, world!\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr_c(retezec, len, -6, -1);

    printf("Retezec od -6 do -1 vcetne, melo by byt \"world!\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr_c(retezec, len, 5, 4);

    printf("Retezec od 5 do 4 vcetne, melo by byt \"\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr_c(retezec, len, -4, -5);

    printf("Retezec od -4 do -5 vcetne, melo by byt \"\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr_c(retezec, len, -1, 3);

    printf("Retezec od -1 do 3 vcetne, melo by byt \"\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr_c(retezec, len, 10, -6);

    printf("Retezec od 10 do -6 vcetne, melo by byt \"\": %s\n",  vysledek);
    free(vysledek);

    char *retezec2 = "abc";
    len = strlen(retezec2);

    vysledek = substr_c(retezec2, len, -4, -4);

    printf("Retezec od -4 do -4 vcetne, melo by byt \"\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr_c(retezec2, len, -3, -3);

    printf("Retezec od -3 do -3 vcetne, melo by byt \"a\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr_c(retezec2, len, -4, -3);

    printf("Retezec od -4 do -3 vcetne, melo by byt \"a\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr_c(retezec2, len, 2, 5);

    printf("Retezec od 2 do 5 vcetne, melo by byt \"c\": %s\n",  vysledek);
    free(vysledek);

    vysledek = substr_c(retezec2, len, 4, 4);

    printf("Retezec od 4 do 4 vcetne, melo by byt \"\": %s\n",  vysledek);
    free(vysledek);


    return 0;
}
