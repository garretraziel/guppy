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

int main(void)
{

    int x = program(stdin);
    
    // predpoklada se, ze chyby jsou jen syntakticke
    // lexikalni se testuji jinde
    // semanticke jeste testovat neumim
    if(x < 0)
        Error("Na radku %d: %s\n", line, SYN_ERRORS[(-x)%100]);
    
    // casem se napise nejake to udelatko na vsechny chyby

    return 0;
}
