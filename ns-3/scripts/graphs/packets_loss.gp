#!/usr/bin/gnuplot
reset
load 'style.gp'

set output "packets_loss.pdf"

set xlabel "Packet Loss Ratio"
set ylabel "Packets Sent"

#set yrange[0:]
set key bottom left


plot \
"packets_loss.dat" using 1:2 title 'Thunks' with linespoints ls 1, \
"packets_loss.dat" using 1:3 title 'Net Timescale' with linespoints ls 2, \
"packets_loss.dat" using 1:4 title 'App Timescale' with linespoints ls 3, \
"packets_loss.dat" using 1:5 title 'ACK' with linespoints ls 4, \
