echo "Execute this by nohup sh ./byauto.sh"
`which time` -v ../../hgm_jack-n --idata id.txt --automatic 1 >tmp-t.txt
`which time` -v ../../hgm_w-n --idata tmp-t.txt --dataf tmp-out.txt --gnuplotf tmp-graph --verbose
echo "gnuplot -persist <tmp-graph-gp.txt"
