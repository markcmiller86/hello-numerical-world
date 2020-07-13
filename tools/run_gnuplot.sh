#!/bin/sh

tmpfil=$1/$1_soln_final.curve
lenx=$(tail -1 $1/$1_soln_final.curve | tr -s ' ' | cut -d' ' -f2)

# Compute midpoint through wall
lenx2=$(perl -e "print $lenx/2")

# Compute left bound of pipe's width
p0=$(perl -e "print $lenx2-0.05")

# Compute right bound of pipe's width
p1=$(perl -e "print $lenx2+0.05")

gnuplot << EOF 1>/dev/null 2>&1 &
set xlabel "Distance (meters)"
set ylabel "Temperature (Kelvin)"
set arrow 27 from 0,273 to $lenx,273 nohead lc rgb "blue"
set arrow 28 from $p0,273 to $p1,273 nohead lc rgb "black" lw 4
plot "$tmpfil" with lines
pause 100
pause mouse keypress "Type a letter from A-F in the active window"
EOF
echo "Type a letter from A-F in the active window to exit"
exit 0
