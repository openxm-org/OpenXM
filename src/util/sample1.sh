#!/bin/csh
oxreplace --oldx 'bbb2b9cdcab8b8a5' --new 'References' $*
oxreplace --oldx 'a4b3a4cecab8bdf1a4cba4c4a4a4a4c62e2e2e' --new 'About this document' $1
oxreplace --oldx 'cabfc0ae' --new 'Heisei ' $*
oxreplace --oldx 'c7af' --new '.' $*
oxreplace --oldx 'b7ee' --new '.' $*
oxreplace --oldx 'c6fc' --new '.' $*

# bbb2b9cdcab8b8a5 参考文献 (EUC)
# a4b3a4cecab8bdf1a4cba4c4a4a4a4c62e2e2e この文書について...
# cabfc0ae 平成 
# c7af 年 
# b7ee 月
# c6fc 日
