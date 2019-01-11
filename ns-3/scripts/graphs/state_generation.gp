#!/usr/bin/gnuplot
reset
load 'style.gp'

set output "state_generation.pdf"

set xlabel "Data Generation Time[ms]"
set ylabel "State size"

#set yrange[0:]
set key top left


plot \
"state_generation.dat" using 1:2 title 'Thunks' with linespoints ls 1, \
"state_generation.dat" using 1:3 title 'Net Timescale' with linespoints ls 2, \
"state_generation.dat" using 1:4 title 'App Timescale' with linespoints ls 3, \
"state_generation.dat" using 1:5 title 'ACK' with linespoints ls 4, \
