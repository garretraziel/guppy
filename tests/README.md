Testy
=====

start-program.sh
----------------

Pro testování testovacích dat slouží program start-program.sh
použití:

    start-program.sh program1 [program2 program3...]

- program vloží pro Lua potřebnou hlavičku a poté celý program spustí

run-tests.sh
------------

Testovací sada. Bere soubory ze vstupní složky (defaultně ./src, jsou to programy v pseudo-lua) a pro každý srovná výstup z guppy s výstupem z opravdové Lua. Pokud potřebuje nějaký vstup, může být ve složce ./src/in ve formátu in.nazev-souboru-v-src.

Ostatní
-------

Viz [Wiki](https://github.com/garretraziel/guppy/wiki/Automatické-testy).
