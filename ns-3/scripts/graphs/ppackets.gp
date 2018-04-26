#!/usr/bin/gnuplot
reset
load 'style.gp'

set output "ppackets.pdf"

set xlabel "Data Generation Time[s]"
set ylabel "Packets Sent"

#set yrange[0:]
set key bottom left


plot \
"ppackets.dat" using 1:2 title 'Thunks' with linespoints ls 1, \
"ppackets.dat" using 1:3 title 'Net Time' with linespoints ls 2, \
