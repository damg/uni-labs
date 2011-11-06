#!/bin/sh

TESTFILE="testtext"
URL="http://www.gnu.org/licenses/gpl-2.0.txt"
SUBDIRS="lookahead yyless reject"

rm $TESTFILE
wget -O$TESTFILE $URL

for e in $SUBDIRS; do
    rm -f test_${e}
done

for e in $SUBDIRS; do
    cat $TESTFILE | $e/lexer > test_${e}
done

for e in $SUBDIRS; do
    for f in $SUBDIRS; do
	if [ $e != $f ]; then
	    cmp test_$e test_$f
	    if [ "$?" -ne 0 ]; then
		echo "Difference between outputs of lexers $e and $f"
		exit 1
	    fi
	fi
    done
done

echo "All tested lexers produced the same output"
echo "The outputs are in the files test_{$SUBDIRS}"
