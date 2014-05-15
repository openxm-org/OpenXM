#! /bin/sh

for input in \
same_non_diag-2.txt  \
same_non_diag-3.txt  \
same_non_diag-4.txt  \
same_non_diag-5.txt  \
same_non_diag-6.txt  \
same_non_diag-7.txt  \
same_non_diag-8.txt  \
same_non_diag-9.txt  \
same_non_diag-10.txt \
same_non_diag-20.txt 
do
	echo $input
	output_time=$input.time
	output_log=$input.log
	/usr/bin/time -pv --output=$output_time ./hgm_ko_orthant -f Testdata/$input > $output_log
done
grep User `ls *time` > tmp
grep prob `ls same_non_diag*log` >> tmp

