#!/bin/bash

#
# Checks output from heat application to confirm
# linear, steady state
#

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
        errbnd=$(perl -e "print - $2")
        relerr=1
    fi
fi

#
# Default boundary conditions
#
Len=1
bc0=0
bc1=1

#
# Handle non-default boundary conditions
#
if [[ -n "$3" ]]; then
    if [[ -z "$4" ]]; then
        echo "Missing left-end boundary condition arg"
        exit 1
    fi
    if [[ -z "$5" ]]; then
        echo "Missing right-end boundary condition arg"
        exit 1
    fi
    Len=$3
    bc0=$4
    bc1=$5
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
        t=$(perl -e "print abs($bc0-$yval)>($2)")
        if [[ "$t" -eq 1 ]]; then
            echo "Check failed at $xval $yval ($bc0)"
            exit 1
        fi
        continue
    fi
    exact_yval=$(perl -e "print $bc0 + ($bc1-$bc0)*$xval/$Len")
    if [[ $relerr -eq 1 ]]; then
        diff=$(perl -e "print abs(($yval-$exact_yval)/$exact_yval)<=($errbnd)")
    else
        diff=$(perl -e "print abs($yval-$exact_yval)<=($errbnd)")
    fi
    if [[ $diff -ne 1 ]]; then
        echo "Check failed at $xval $yval ($exact_yval)"
        exit 1
    fi
done < $rdfile
exit 0
