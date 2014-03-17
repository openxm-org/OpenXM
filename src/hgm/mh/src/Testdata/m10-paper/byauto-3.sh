echo "Execute this by nohup sh ./byauto-3.sh --automatic 1 is default, but it is specified."
`which time` -v ../../hgm_jack-n --idata id.txt --automatic 1 --degree 24 --assigned_series_error 1e-8 >tmp-t.txt
`which time` -v ../../hgm_w-n --idata tmp-t.txt --dataf tmp-out.txt --gnuplotf tmp-graph --verbose
echo "gnuplot -persist <tmp-graph-gp.txt"
