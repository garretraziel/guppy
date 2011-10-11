#! /bin/sh

CESTA_GUPPY="../bin/guppy"
CESTA_START="./start-program.sh"
CESTA_TESTS="./src/*"
CESTA_INPUTS="./src/in"

if [ $# -eq 1 ]; then
	CESTA_GUPPY="$1"
fi

CERVENA='\033[0;31m'
NC='\033[0m'
ZELENA='\033[0;32m'

for test in $CESTA_TESTS; do
    if [ ! -d $test ]; then
        echo "--- Testing $test"
        jmeno=`echo $test | gawk '{FS="/";$0=$0;print $NF}'`
        if [ -f "$CESTA_INPUTS/in.$jmeno" ]; then
            $CESTA_START $test > out1.$jmeno < "$CESTA_INPUTS/in.$jmeno"
            $CESTA_GUPPY $test > out2.$jmeno < "$CESTA_INPUTS/in.$jmeno"
        else
            $CESTA_START $test > out1.$jmeno
            $CESTA_GUPPY $test > out2.$jmeno
        fi
        diff out1.$jmeno out2.$jmeno > /dev/null
        if [ $? -eq 0 ]; then
            echo "[ ${ZELENA}PASSED${NC} ] in $test"
        else
           echo "[ ${CERVENA}FAILED!${NC} ] in $test"
        fi
        rm -f out1.$jmeno out2.$jmeno
    fi
done
