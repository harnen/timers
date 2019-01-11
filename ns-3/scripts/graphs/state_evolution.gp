#!/usr/bin/gnuplot
reset
load 'style.gp'
set output "state_evolution.pdf"

set xlabel "Time[s]"
set ylabel "PIT size"

#set yrange[0:]
set xrange[0:20]
#set key center right
set key at screen 0.95,0.5


plot \
"state_evolution_thunks.dat" using 1:2 title 'Thunks' with lines ls 1, \
"state_evolution_net.dat" using 1:2 title 'Net Timescale' with lines ls 2, \
"state_evolution_app.dat" using 1:2 title 'App Timescale' with lines ls 3, \
"state_evolution_ack.dat" using 1:2 title 'ACK' with lines ls 4 \
