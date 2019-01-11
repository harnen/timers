#!/usr/bin/gnuplot
reset
load './style.gp'

set output "satisfaction_estimate.pdf"

set xlabel "Estimation Error[ms]"
set ylabel "Interest Satisfaction Time[s]"

#set yrange[0:]
set xrange[-5000:5000]
set key top left


plot \
"satisfaction_estimate.dat" using 1:2 title 'Thunks' with linespoints ls 1, \
"satisfaction_estimate.dat" using 1:3 title 'Net Timescale' with linespoints ls 2, \
"satisfaction_estimate.dat" using 1:4 title 'App Timescale' with linespoints ls 3, \
"satisfaction_estimate.dat" using 1:5 title 'ACK' with linespoints ls 4, \
