echo "Execute this by nohup sh ./byauto-2.sh --automatic 1 is default, but it is specified."
`which time` -v ../../hgm_jack-n --idata id.txt --automatic 1 --assigned_series_error 1e-7 >tmp-t.txt
`which time` -v ../../hgm_w-n --idata tmp-t.txt --dataf tmp-out.txt --gnuplotf tmp-graph --verbose
echo "gnuplot -persist <tmp-graph-gp.txt"
