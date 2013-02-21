../hgm_jack-n --idata tmp-idata3.txt >cc-00.txt
diff cc-00.txt tmp-data3-out2.txt
echo "Remove cc-* if there is no difference."