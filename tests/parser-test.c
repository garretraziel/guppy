#include <stdio.h>
#include <stdarg.h>

#include "parser.h"
#include "lexical.h"

void Error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "CHYBA: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
}

const char **ERROR_MSG[] = {
    [1] = LEX_ERRORS,
    [2] = SYN_ERRORS,
};

int main(void)
{

    int x = program(stdin);
    
    if(x < 0) {
        Error("Na radku %d: %s\n", line, ERROR_MSG[(-x)/100][(-x)%100]);
        return -x/100;
    }
    
    return 0;
}
