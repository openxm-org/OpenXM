  fprintf(stderr,"oxweave [--plain] [--source] [--recursive] [key1 key2 ...]\n");
  fprintf(stderr,"oxweave reads bytes from the standarndard input stream \n");
  fprintf(stderr,"and outputs them if one of the following conditions\n");
  fprintf(stderr,"is satisfied:\n");
  fprintf(stderr,"(1) hogehoge is output if hogehoge is enclosed \n");
  fprintf(stderr,"    like /*&xyz  hogehoge */ and xyz matches with one of the keys\n");
  fprintf(stderr,"(2) hogehoge is output if hogehoge comes after //&xyz \n");
  fprintf(stderr,"    followed with the newline and xyz matches with one of the keys\n");
  fprintf(stderr,"(3) If the option --plain is given, all texts out of /*&xyz */ and //&xyz \n");
  fprintf(stderr,"    are output.\n");
  fprintf(stderr,"(4) If the option --recursive is given, the symbol */ is treated recursively.\n");
  fprintf(stderr,"Example 1: oxweave --source jp <oxweave.c >t.tex\n");
  fprintf(stderr,"Example 2: oxweave  jp <oxweave.c >t.tex\n");
  fprintf(stderr,"Example 3: oxweave --plain  <oxweave.c >t.tex\n");





