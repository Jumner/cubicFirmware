set datafile separator ','
set xdata time # set x axis
set timefmt "%S" # use seconds
set format x "%S"
#set timefmt "%.1f s" # use seconds
set key autotitle columnhead # Use first line for header names
set multiplot layout 2,2
#plot filename using 1:2 with lines, '' using 1:3 with lines, '' using 1:4 with lines, '' using 1:5 with lines, '' using 1:6 with lines, '' using 1:7 with lines, '' using 1:8 with lines, '' using 1:9 with lines, '' using 1:10 with lines, '' using 1:11 with lines, '' using 1:12 with lines, '' using 1:13 with lines
plot filename using 1:2 with lines, '' using 1:3 with lines, '' using 1:4 with lines
plot filename using 1:5 with lines, '' using 1:6 with lines, '' using 1:7 with lines
plot filename using 1:8 with lines, '' using 1:9 with lines, '' using 1:10 with lines
plot filename using 1:11 with lines, '' using 1:12 with lines, '' using 1:13 with lines
