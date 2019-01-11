#!/usr/bin/gnuplot
reset
load '../estimate/style.gp'

set output "h_packets_loss.pdf"

set xlabel "Packet Loss Ratio"
set ylabel "Messages Sent"

#set yrange[0:]
set key top left


plot \
"h_packets_loss.dat" using 1:2 title '1xRTT' with linespoints ls 8, \
"h_packets_loss.dat" using 1:3 title '2xRTT' with linespoints ls 6, \
"h_packets_loss.dat" using 1:4 title '3xRTT' with linespoints ls 7, \
