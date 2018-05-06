#!/usr/bin/gnuplot
reset
load 'style.gp'
exit
set output "state.pdf"

set xlabel "Time"
set ylabel "PIT size"

#set yrange[0:]
set key bottom left


plot \
"state.dat" using 1:2 title 'Thunks' with steps ls 1, \
