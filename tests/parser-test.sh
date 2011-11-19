#!/bin/bash

# Jen kontroluje syntatickou spravnost

#vypocet sloupcu
STAT_COL=$(tput cols)
 
yell() {
    printf "\e[F"
    printf "\e[$(( STAT_COL - 6 ))G"
    printf "$1 \n"
}


# Kontrola existente binarky
if [ ! -x "parser-test" ] ; then
    printf "Chybi spustitelny soubor\n"
    exit 1
fi


# tyhle testy maji skoncit OK
for file in programs/*ok; do
    echo "Testuji  $file "
    ./parser-test < $file && yell "\e[32mOK\e[0m" || yell "\e[31mFAIL\e[0m"
done

printf "* * * * * * * * * * * * \n"

# tyhle naopak maji koncit chybou
for file in programs/*fail; do
    echo "Testuji  $file "
    ./parser-test < $file && yell "\e[31mFAIL\e[0m" || yell "\e[32mOK\e[0m"
done
