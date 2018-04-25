#set terminal pdfcairo font "Gill Sans,9" linewidth 4 rounded fontscale 1.0
reset
set terminal pdfcairo size 8,5 font "Gill Sans,9" linewidth 4 rounded fontscale 1.34

# Line style for axes
set style line 80 lt rgb "#808080"

# Line style for grid
set style line 81 lt 0  # dashed
set style line 81 lt rgb "#808080"  # grey

set grid back linestyle 81
set border 3 back linestyle 80 # Remove border on top and right.  These
             # borders are useless and make it harder
             # to see plotted lines near the border.
    # Also, put it in grey; no need for so much emphasis on a border.
set xtics nomirror font "Gill Sans, 6"
set ytics nomirror font "Gill Sans, 7"

set style line 1 lt 1 lc rgb "#A00000" lw 1 pt 1 ps 2
set style line 2 lt 1 lc rgb "#00A000" lw 1 pt 6 ps 2
set style line 20 lt 2 lc rgb "#00A000" lw 1 pt 6 ps 2
set style line 3 lt 1  lc rgb "#5060D0" lw 1 pt 2 ps 2
set style line 30 lt 2 lc rgb "#5060D0" lw 1 pt 2 ps 2
set style line 4 lt 1 lc rgb "#F25900" lw 1 pt 9 ps 2
set style line 5 lt 1 lc rgb "#FF33FF" lw 1 pt 5 ps 2

#set key reverse Left width 0


# Make the x axis labels easier to read.
#set xtics rotate out
# Select histogram data
#set style data histogram
# Give the bars a plain fill pattern, and draw a solid line around them.
#set style fill solid border -1

