../hgm_w-n --idata tmp-idata2.txt --dataf bb --strategy 1 --abserr 0.0 --relerr 1e-10
diff bb* tmp-data2-a-out.txt
echo "Remove bb-* if there is no difference."
