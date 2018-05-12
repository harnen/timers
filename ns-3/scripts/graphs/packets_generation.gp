#!/usr/bin/gnuplot
reset
load 'style.gp'

set output "packets_generation.pdf"

set xlabel "Data Generation Time[s]"
set ylabel "Packets Sent"

#set yrange[0:]
set key top left


plot \
"packets_generation.dat" using 1:2 title 'Thunks' with linespoints ls 1, \
"packets_generation.dat" using 1:3 title 'Net Timescale' with linespoints ls 2, \
"packets_generation.dat" using 1:4 title 'App Timescale' with linespoints ls 3, \
"packets_generation.dat" using 1:5 title 'ACKs' with linespoints ls 4, \
