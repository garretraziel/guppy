#include <stdio.h>
#include <stdarg.h>

#include "parser.h"
#include "lexical.h"
#include "defines.h"
#include "expr.h"
#include "exec.h"

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

    input = stdin;
    int x = parser();
    
    if(x < 0) {
        Error("Na radku %d: %s\n", line, ERROR_MSG[(-x)/100][(-x)%100]);
        return -x/100;
    }
    
    return 0;
}
