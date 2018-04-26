#!/usr/bin/gnuplot
reset
load 'style.gp'

set output "psatisfaction_time.pdf"

set xlabel "Data Generation Time[s]"
set ylabel "Average Interest Satisfaction Time[s]"

#set yrange[0:]
set key bottom left


plot \
"psatisfaction_time.dat" using 1:2 title 'Thunks' with linespoints ls 1, \
"psatisfaction_time.dat" using 1:3 title 'Net Time' with linespoints ls 2, \
