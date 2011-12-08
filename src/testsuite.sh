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
if [ ! -x "guppy" ] ; then
    printf "Chybi spustitelny soubor\n"
    exit 1
fi


# testy vseho
for i in {0..5}; do
    for file in ../tests/testsuite/*-$i; do
        # pokud nejsou soubory, vynechat
        if [ ! -e "$file" ]; then
            continue
        fi
        printf "\e[32mTestuji $file \e[0m \n"
        printf "\e[37m "
        grep -e "^--" $file
        printf "\e[0m"
        # pokud existuje vstup, pouzit
        if [ -e $file.in ]; then
            ./guppy $file < $file.in
        else
            ./guppy $file
        fi
        # report vysledku
        if [ "$?" == "$i" ]; then
            yell "\e[32m OK\e[0m"
        else
            yell "\e[31mFAIL\e[0m"
        fi
        printf "\n"
    done
done

# pokud je para --full, tak se pokracuje dale
if [ ! "$1" == "--full" ]; then
    exit 0
fi

# tyhle testy maji skoncit OK, jedou pres valgrind
for file in ../tests/programs/0-syn-*; do
    echo "Testuji  $file "
    valgrind --leak-check=full --show-reachable=yes -q ./guppy $file && yell "\e[32mOK\e[0m" || yell "\e[31mFAIL\e[0m"
done
