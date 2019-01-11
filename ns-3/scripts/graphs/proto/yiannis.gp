#!/usr/bin/gnuplot
reset
load 'style.gp'

set output "yiannis.pdf"

set xlabel "Number of Episodes per month"
set ylabel "CDN cost (in $k per month)"

#set yrange[0:]
set xrange[1:]
set key top left


plot \
"yiannis.dat" using 1:3 title '2M' with lines ls 1, \
"yiannis.dat" using 1:2 title '1M' with lines ls 2, \
"yiannis.dat" using 1:4 title '0.5M' with lines ls 3, \
