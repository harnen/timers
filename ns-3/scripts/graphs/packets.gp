#!/usr/bin/gnuplot
reset
load 'style.gp'

set output "packets.pdf"

set xlabel "Packet Loss Ratio"
set ylabel "Packets Sent"

#set yrange[0:]
set key bottom left


plot \
"packets.dat" using 1:2 title 'Thunks' with linespoints ls 1, \
"packets.dat" using 1:3 title 'Net Time' with linespoints ls 2, \
