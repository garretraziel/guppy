#!/bin/bash

# Jen kontroluje syntatickou spravnost

#vypocet sloupcu
STAT_COL=$(tput cols)
 
yell() {
    printf "\e[F"
    printf "\e[$(( STAT_COL - 6 ))G"
    printf "$1 \n"
}

TOTAL=0
ERRORS=0


# Kontrola existente binarky
if [ ! -x "../src/guppy" ] ; then
    printf "Chybi spustitelny soubor\n"
    exit 1
fi


# testy vseho
for i in {0..5}; do
    for file in testsuite/*-$i; do
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
            ../src/guppy $file < $file.in
        else
            ../src/guppy $file
        fi
        # report vysledku
        if [ "$?" == "$i" ]; then
            yell "\e[32m OK\e[0m"
        else
            yell "\e[31mFAIL\e[0m"
            let "ERRORS=$ERRORS+1"
        fi
        let "TOTAL=$TOTAL+1"
        printf "\n"
    done
done

printf "Shrnuti: \n\n"
printf "     $ERRORS chyb v $TOTAL testech \n"

# pokud je para --full, tak se pokracuje dale
if [ ! "$1" == "--full" ]; then
    exit 0
fi

# tyhle testy maji skoncit OK, jedou pres valgrind
for file in testsuite/*; do
    echo "Testuji  $file "
    valgrind --leak-check=full --show-reachable=yes -q ../src/guppy $file < /dev/null
done
