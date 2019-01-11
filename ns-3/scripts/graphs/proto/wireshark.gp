#!/usr/bin/gnuplot
reset
load '../style.gp'

set output "wireshark.pdf"

set xlabel "Time[s]"
set ylabel "Bandwidth[B]"

#set yrange[0:]
set key top right


plot \
"wireshark.dat" using 1:2 title 'Total' with lines ls 1, \
"wireshark.dat" using 1:3 title 'Sent' with lines ls 2, \
"wireshark.dat" using 1:4 title 'Received' with lines ls 3, \
