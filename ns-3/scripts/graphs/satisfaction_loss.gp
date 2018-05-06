#!/usr/bin/gnuplot
reset
load 'style.gp'

set output "satisfaction_loss.pdf"

set xlabel "Packet Loss Ratio"
set ylabel "Average Interest Satisfaction Time[s]"

#set yrange[0:]
set key bottom left


plot \
"satisfaction_loss.dat" using 1:2 title 'Thunks' with linespoints ls 1, \
"satisfaction_loss.dat" using 1:3 title 'Net Time' with linespoints ls 2, \
