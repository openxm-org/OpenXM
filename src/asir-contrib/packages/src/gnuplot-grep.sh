#!/bin/sh
FILES=$*
export FILES
grep gnuplot_find_proc $FILES
grep gnuplot_start $FILES
grep gnuplot_start_unix $FILES
grep gnuplot_start_windows $FILES
grep gnuplot_isAlive $FILES
grep gnuplot $FILES
grep gnuplot_demo0 $FILES
grep gnuplot_clean $FILES
grep gnuplot_check_arg_plotDots $FILES
grep gnuplot_plotDots $FILES
grep gnuplot_plot_dots $FILES
grep gnuplot_plotRoots $FILES
grep gnuplot_demo2 $FILES
grep gnuplot_heat $FILES
grep gnuplot_output $FILES
grep gnuplot_to_gnuplot_format_old $FILES
grep gnuplot_to_gnuplot_format $FILES
grep gnuplot_plot_function $FILES
grep openfile $FILES
grep closefile $FILES
grep writeString $FILES
grep complexToVec $FILES
grep Gnuplot_ $FILES
