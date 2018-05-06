#!/usr/bin/gnuplot
reset
load 'style.gp'

set output "satisfaction_generation.pdf"

set xlabel "Data Generation Time[s]"
set ylabel "Average Interest Satisfaction Time[s]"

#set yrange[0:]
set key bottom left


plot \
"satisfaction_generation.dat" using 1:2 title 'Thunks' with linespoints ls 1, \
"satisfaction_generation.dat" using 1:3 title 'Net Time' with linespoints ls 2, \
