#include <stdio.h>
#define NS (char *)NULL
typedef enum {start,output,setdic,cleardic} prAction;
char *KakPrint(prAction act,char *lw,char *rw);
int KasirKanlex0(prAction act);
void KasirKanParseAstring(char *s);
void KasirKanFsappendc(int c);

#define BSIZE 100000
static char KakOut[BSIZE];

char *KasirKanConvert(char *ans) {
  char *akOut;
  KasirKanlex0(start);  /* Initialize  lex() */
  KakPrint(start,NS,NS);     /* Initialize output buffer */
  KasirKanParseAstring(ans);
  KasirKanFsappendc(';'); 
  KasirKanparse();
  akOut = KakPrint(output,NS,NS);
  return(akOut);
}

