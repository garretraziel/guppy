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
if [ ! -x "lexical-test" ] ; then
    printf "Chybi spustitelny soubor\n"
    exit 1
fi


# tyhle testy maji skoncit OK
for file in programs/0-lex-*; do
    echo "Testuji  $file "
    if [ "$1" == "-q" ]; then
        ./lexical-test < $file 2&>/dev/null && yell "\e[32mOK\e[0m" || yell "\e[31mFAIL\e[0m"
    else
        ./lexical-test < $file && yell "\e[32mOK\e[0m" || yell "\e[31mFAIL\e[0m"
    fi
done

printf "* * * * * * * * * * * * \n"

# tyhle naopak maji koncit chybou
# 1-2 protoze to jsou lexikalni a syntakticke chyby, to je jedine, co to pozna
for i in {1..1}; do
    for file in programs/$i-lex-*; do
        echo "Testuji  $file "
        if [ "$1" == "-q" ]; then
            ./lexical-test < "$file" 2&>/dev/null
        else
            ./lexical-test < $file
        fi
        if [ "$?" -eq "$i" ]; then
            yell "\e[32mOK\e[0m"
        else
            yell "\e[31mFAIL\e[0m"
        fi
    done
done
