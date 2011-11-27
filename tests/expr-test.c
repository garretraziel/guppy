#include <stdio.h>
#include <stdarg.h>

#include "parser.h"
#include "lexical.h"
#include "expr.h"
#include "defines.h"

void Error(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    fprintf(stderr, "CHYBA: ");
    vfprintf(stderr, fmt, args);
    va_end(args);
}


int main(void)
{

    int x = expression();
    
    if(x < 0) {
        Error("Na radku %d: %s\n", line, ERROR_MSG[(-x)/100][(-x)%100]);
        return -x/100;
    }
    
    return 0;
}
