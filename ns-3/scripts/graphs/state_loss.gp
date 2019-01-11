#!/usr/bin/gnuplot
reset
load 'style.gp'

set output "state_loss.pdf"

set xlabel "Packet Loss Ratio"
set ylabel "State size"

#set yrange[0:]
set key at screen 0.95,0.6



plot \
"state_loss.dat" using 1:2 title 'Thunks' with linespoints ls 1, \
"state_loss.dat" using 1:3 title 'Net Timescale' with linespoints ls 2, \
"state_loss.dat" using 1:4 title 'App Timescale' with linespoints ls 3, \
"state_loss.dat" using 1:5 title 'ACK' with linespoints ls 4, \
