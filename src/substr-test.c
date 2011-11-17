#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ial.h"
#include "string.h"

int main(void)
{
    char *vysledek;

    string str;
    str_init(&str, "Hello, world!");
    char *str2 = "Hello, world!";
    int len = strlen(str2);

    char *vysledek2;
    

    printf("Pracuji se retezcem: \"%s\"\n",str.str);

    vysledek = substr(&str, 1, 6);
    vysledek2 = substr_c(str2, len, 0, 5);

    printf("Retezec od 1 do 6 vcetne, melo by byt \"Hello,\": %s a %s\n",  vysledek, vysledek2);
    free(vysledek);
    free(vysledek2);

    vysledek = substr(&str, 8, 12);
    vysledek2 = substr_c(str2, len, 7, 11);

    printf("Retezec od 8 do 12 vcetne, melo by byt \"world\": %s a %s\n",  vysledek, vysledek2);
    free(vysledek);
    free(vysledek2);

    vysledek = substr(&str, 1, 13);
    vysledek2 = substr_c(str2, len, 0, 12);

    printf("Retezec od 1 do 13 vcetne, melo by byt \"Hello, world!\": %s a %s\n",  vysledek, vysledek2);
    free(vysledek);
    free(vysledek2);

    vysledek = substr(&str, 1, 20);
    vysledek2 = substr_c(str2, len, 0, 20);

    printf("Retezec od 1 do 20 vcetne, melo by byt \"Hello, world!\": %s a %s\n",  vysledek, vysledek2);
    free(vysledek);
    free(vysledek2);

    vysledek = substr(&str, -6, -1);
    vysledek2 = substr_c(str2, len, -6, -1);

    printf("Retezec od -6 do -1 vcetne, melo by byt \"world!\": %s a %s\n",  vysledek, vysledek2);
    free(vysledek);
    free(vysledek2);

    vysledek = substr(&str, 5, 4);
    vysledek2 = substr_c(str2, len, 5, 4);

    printf("Retezec od 5 do 4 vcetne, melo by byt \"\": %s a %s\n",  vysledek, vysledek2);
    free(vysledek);
    free(vysledek2);

    vysledek = substr(&str, -4, -5);
    vysledek2 = substr_c(str2, len, -4, -5);

    printf("Retezec od -4 do -5 vcetne, melo by byt \"\": %s a %s\n",  vysledek, vysledek2);
    free(vysledek);
    free(vysledek2);

    vysledek = substr(&str, -1, 3);
    vysledek2 = substr_c(str2, len, -1, 3);

    printf("Retezec od -1 do 3 vcetne, melo by byt \"\": %s a %s\n",  vysledek, vysledek2);
    free(vysledek);
    free(vysledek2);

    vysledek = substr(&str, 10, -6);
    vysledek2 = substr_c(str2, len, 10, -6);

    printf("Retezec od 10 do -6 vcetne, melo by byt \"\": %s a %s\n",  vysledek, vysledek2);
    free(vysledek);
    free(vysledek2);

    str_free(&str);
    str_init(&str,"abc");

    str2 = "abc";
    len = strlen(str2);

    vysledek = substr(&str, -4, -4);
    vysledek2 = substr_c(str2, len, -4, -4);

    printf("Retezec od -4 do -4 vcetne, melo by byt \"\": %s a %s\n",  vysledek, vysledek2);
    free(vysledek);
    free(vysledek2);

    vysledek = substr(&str, -3, -3);
    vysledek2 = substr_c(str2, len, -3, -3);

    printf("Retezec od -3 do -3 vcetne, melo by byt \"a\": %s a %s\n",  vysledek, vysledek2);
    free(vysledek);
    free(vysledek2);

    vysledek = substr(&str, -4, -3);
    vysledek2 = substr_c(str2, len, -4, -3);

    printf("Retezec od -4 do -3 vcetne, melo by byt \"a\": %s a %s\n",  vysledek, vysledek2);
    free(vysledek);
    free(vysledek2);

    vysledek = substr(&str, 3, 6);
    vysledek2 = substr_c(str2, len, 2, 5);

    printf("Retezec od 3 do 6 vcetne, melo by byt \"c\": %s a %s\n",  vysledek, vysledek2);
    free(vysledek);
    free(vysledek2);

    vysledek = substr(&str, 4, 4);
    vysledek2 = substr_c(str2, len, 3, 3);

    printf("Retezec od 4 do 4 vcetne, melo by byt \"\": %s a %s\n",  vysledek, vysledek2);
    free(vysledek);
    free(vysledek2);

    str_free(&str);

    return 0;
}
