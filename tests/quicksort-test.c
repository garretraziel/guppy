#include <stdio.h>
#include "ial.h"
#include "string.h"

int main(void)
{
    int c;
    string str;
    if(str_new(&str, 255) == 0)
        return 1;
    while((c = getchar()) != EOF) {
        if(c == '\n'){
            sort(&str);
            printf("%s\n-----\n", str.str);
            str_clean(&str);
        } else
            str_push(&str, c);
    }
    return 0;
}
