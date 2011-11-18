#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ial.h"
#include "string.h"


void substr_test(string str, char*str2, int from, int to, const char *expect, int len)
{
    char *vysledek, *vysledek2;

    vysledek = substr(&str, from, to);
    vysledek2 = substr_c(str2, len, (from<0)?from:(from-1), (to<0)?to:(to-1));

    printf("Retezec od %d do %d vcetne\n"
           " melo by byt \"%s\" \n"
           " vyslo       \"%s\" \n"
           " a taky      \"%s\" \n", from, to, expect, vysledek, vysledek2);
    free(vysledek);
    free(vysledek2);
}


int main(void)
{

    string str;
    str_init(&str, "Hello, world!");
    char *str2 = "Hello, world!";
    int len = strlen(str2);

    printf("Pracuji se retezcem: \"%s\"\n",str.str);

    substr_test(str, str2, 1, 6, "Hello,", len);
    substr_test(str, str2, 8, 12, "world", len);
    substr_test(str, str2, 1, 13, "Hello, world!", len);
    substr_test(str, str2, 1, 20, "Hello, world!", len);
    substr_test(str, str2, -6, -1, "world!", len);
    substr_test(str, str2, 5, 4, "", len);
    substr_test(str, str2, -4, -5, "", len);
    substr_test(str, str2, -1, 3, "", len);
    substr_test(str, str2, 10, -6, "", len);

    str_free(&str);
    str_init(&str,"abc");

    str2 = "abc";
    len = strlen(str2);
    
    printf("\nPracuji se retezcem: \"%s\"\n",str.str);

    substr_test(str, str2, -4, -4, "", len);
    substr_test(str, str2, -3, -3, "a", len);
    substr_test(str, str2, -4, -3, "a", len);
    substr_test(str, str2, 3, 6, "c", len);
    substr_test(str, str2, 4, 4, "", len);

    str_free(&str);

    return 0;
}
