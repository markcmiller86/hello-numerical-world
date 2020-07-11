#!/bin/bash

if [[ -z "$1" ]]; then
    echo "Specify input file as 1rst arg"
    exit 1
fi
rdfile=$1

#
# Use absolute error by default. Relative
# error if specified tolerance is negative.
#
relerr=0
errbnd=1e-7
if [[ -n "$2" ]]; then
    errbnd=$2
    t=$(perl -e "print $2<0")
    if [[ "$t" == "1" ]]; then
        errbnd="-$2"
        relerr=1
    fi
fi

#
# Loop over lines. Use perl's arithmetic functions to
# do needed arithmetic operations. Handle absolute and
# relative diffs and special case at zero.
#
while IFS= read line; do
    if [[ "$line" =~ "#" ]]; then
        continue
    fi
    xval=$(echo $line | tr -s ' ' | cut -d' ' -f1)
    yval=$(echo $line | tr -s ' ' | cut -d' ' -f2)
    if [[ "$xval" == "0" ]]; then
        t=$(perl -e "print $yval>($2)")
        if [[ "$t" -eq 1 ]]; then
            echo "Check failed at $xval $yval"
            exit 1
        fi
        continue
    fi
    ratio=$(perl -e "print abs(1-($yval/$xval))<=($errbnd)")
    if [[ $relerr -eq 1 ]]; then
        diff=$(perl -e "print abs(2*($yval-$xval)/($yval+$xval))<=($errbnd)")
    else
        diff=$(perl -e "print abs($yval-$xval)<=($errbnd)")
    fi
    if [[ $diff -ne 1 ]]; then
        echo "Check failed at $xval $yval"
        exit 1
    fi
done < $rdfile
exit 0
