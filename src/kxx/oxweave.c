/*&jp 
\documentclass{jarticle}
\title{oxweave のソースコードについての解説}
\author{} \date{}
\begin{document}
\maketitle
\section{前書き}
*/
/* $OpenXM: OpenXM/src/kxx/oxweave.c,v 1.1 1999/11/16 07:57:37 takayama Exp $ */
#include <stdio.h>
#define BSIZE 256
#define VSIZE 256
static int Debug = 0;
/*&jp \noindent
  {\tt Buf} は標準出力よりのデータを一時格納しておく領域.
  {\tt Head} が最初の文字をさし, {\tt Tail} は最後の文字をさす.
  {\tt Buf} をリング状に使用するので, リングバッファとよぶ
ことにする.
 */
int Buf[BSIZE];
int Head = 0;
int Tail = 0;
char *Tag = NULL;
/*&jp \noindent {\tt OutputNoTaggedSegment = 1}
       なら コメント記号の外は verbatim を用いて出力. 
      {\tt --source} オプションでこの変数を1にできる.
*/
int OutputNoTaggedSegment = 0;
/*&jp \noindent 1 ならタグのついた場所を出力中. */
int OutputtingTaggedSegment = 0;
int BeginVerbatim = 0;

/*&jp \section{プログラム本体} */
main(int argc,char *argv[]) {
  int c;
  int tagc,i;
  char *tagv[VSIZE];
  int tagc2;
  char *tagv2[VSIZE];
  char *s = "/*\x026";  /* 0x26 = & */
  Head = Tail = 0; Buf[0] = ' ';  /* initialize */

  /* コメントのおわりの記号. sm1 なら 0xa である. */
  tagc2 = 1; tagv2[0] = "*/";
  /*&jp  {\tt tagv[]} にタグのあつまりをいれる. */
  tagc = 0;
  if (argc <= 1 || argc >= VSIZE) {
    usage();
    exit();
  }else{
    for (i=1; i< argc ; i++) {
      if (strcmp(argv[i],"--source") == 0) {
	OutputNoTaggedSegment = 1;
      }else{
	tagv[tagc] = (char *) malloc(sizeof(char)*(strlen(argv[i])+4));
	strcpy(tagv[tagc],s);
	strcat(tagv[tagc],argv[i]);
	tagc++;
      }
    }
  }
  /*&jp プログラムは３つの状態を持つ. 状態 0 はタグ付きコメント記号の外.
   状態 1 は指定されたタグの付いたコメントの中.
   状態 2 は指定されていないタグの付いたコメントの中
   (状態２にあるときは印刷されない.) */
  /*
       state 0  -- / * & jp  --->  state 1  
            if ( BeginVerbatim & OutputNoTaggedSegment ) end-verbatim
                <---  * /    ---   state 1
            if ( OutputNoTaggedSegment ) begin-verbatim

       state 0  -- / * & unknown  --->  state 2  
                <---  * /    ---   state 2

       state 0  & OutputNoTaggedSegment  ==> putchar()
       state 1                           ==> putchar()
       state 2                           ==> skip
   */
  while (notEOF()) {
    /* We are in the state 0 */
    findNextTag(tagc,tagv,tagc2,tagv2);
    /* We are int the state 1 */
    findEndTag(tagc2,tagv2);
  }
  if (BeginVerbatim) {
    printf("\n\\end{verbatim\x07d}\n");
  }
  exit(0);
}

/*&jp \noindent 次の関数は利用法を表示する. */
usage() {
  fprintf(stderr,"oxweave [--source] [key1 key2 ...]\n");
  fprintf(stderr,"Example 1: oxweave --source jp <oxweave.c >t.tex\n");
  fprintf(stderr,"Example 2: oxweave  jp <oxweave.c >t.tex\n");
}

#define inc(a) ((a+1) % BSIZE)
/*&jp \noindent {\tt wread()} は 標準入力よりのデータを読めるだけ
リングバッファ {\tt Buf] へ読み込む.*/
wread() {
  int c,i;
  static int eof = 0;
  if (eof) return(-1);
  while (inc(Tail) != Head) {
    c = getchar();
    Tail = inc(Tail);
    Buf[Tail] = c;
    if (c == EOF) { eof = 1; return(-1); }
  }
  if (Debug == 1) {
    fprintf(stderr,"[Head=%d, Tail=%d, ",Head,Tail);
    for (i = Head; inc(i) != Tail; i = inc(i)) {
      fprintf(stderr,"%c",Buf[i]);
    }
    fprintf(stderr,"]\n");
  }
  return(0);
}

/*&jp \noindent {\tt wgetc(p)} は
{\tt p} 文字読み込んで最後の 1 文字のアスキーコードを戻す.
 */
int wgetc(int p) {
  int c;
  int i;
  wread();
  if (p < 1) p = 1;
  for (i=0; i<p; i++) {
    c = Buf[Head];
    if (c == EOF) return(c);
    Head = inc(Head);
    wread();
  }
  return(c);
}

/*&jp  \noindent {\tt findNextTag()} は次の {\tt /\*\&} なるタグをさがす.
  {\tt OutputNoTaggedSegment} が 1 ならデータをそのままながす.
  無視すべきタグのときは, タグ内部をスキップしたのち
  {\tt findNextTag} を再帰的に呼ぶ.
  */
findNextTag(int tagc, char *tagv[],int tagc2,char *tagv2[]) {
  int i;
  int c,d;
  do {
    for (i=0; i<tagc; i++) {
      if (wcmp(tagv[i]) == 0) {
	wgetc(strlen(tagv[i]));
	if (OutputNoTaggedSegment == 1 && BeginVerbatim == 1) {
	  BeginVerbatim = 0;
	  printf("\\end{verbatim\x07d}\n");
	}
	OutputtingTaggedSegment = 1;
	return;  /* Now, state is 1. */
      }
    }
    /*&jp {\tt /\*\&} だけどどのタグにも一致しない */
    if (wcmp("/*\x026") == 1) {
      wgetc(3);
      while ((d=wgetc(1)) > ' ') ;
      /* We are in the state 2. */
      skipToEndTag(tagc2,tagv2);  
      /* We are in the state 0. */
      findNextTag(tagc,tagv,tagc2,tagv2);
      return;
    }
    /* We are in the state 0 */
    c = wgetc(1);
    if (OutputNoTaggedSegment) {
      putchar(c);
    }
  }while( c!= EOF);
  if (BeginVerbatim == 1) {
    printf("\n\\quad\n\\end{verbatim\x07d}\n");
  }
  exit(0);
}
      
/*&jp  \noindent {\tt findEndTag()} は次の {\tt \*\/} なるタグをさがす.
*/
findEndTag(int tagc,char *tagv[]) {
  int i;
  int c;
  /* We are in the state 1. */
  do {
    for (i=0; i<tagc; i++) {
      if (wcmp(tagv[i]) == 0) {
	wgetc(strlen(tagv[i]));
	OutputtingTaggedSegment = 0;
	if (OutputNoTaggedSegment) {
	  printf("\n{\\footnotesize \\begin{verbatim}\n");
	  BeginVerbatim = 1;
	}
	return;  /* Our state is 0. */
      }
    }
    /* Our state is 1. */
    c = wgetc(1);
    putchar(c);
  }while( c!= EOF);
  fprintf(stderr,"findEndTag: unexpected EOF.\n");
  irregularExit();
}

skipToEndTag(int tagc,char *tagv[]) {
  int i;
  int c;
  /* our state is 2. */
  do {
    for (i=0; i<tagc; i++) {
      if (wcmp(tagv[i]) == 0) {
	wgetc(strlen(tagv[i]));
	return;  /* our state is 0. */
      }
    }
    /* our state is 2. */
    c = wgetc(1);
  }while( c!= EOF);
  fprintf(stderr,"findEndTag: unexpected EOF.\n");
  irregularExit();
}
  
/*&jp \noindent {\tt wcmp(s)} は文字列 {\tt s} と {\tt Buf[Head]} から
はじまる文字列を比較する.
{\tt Buf[Head+strlen(s) % BSIZE]} が 0x20 以下であり, あとのバイトが
一致すれば 0 を戻す.
あとのバイトが 0x20 以下でないが, その他のバイトが一致するときは
1 を戻す.
以上二つの場合に合致しない場合は -1 を戻す.
*/    
wcmp(char *s) {
  int n;
  int i,j;
  wread();
  if (Debug == 2) fprintf(stderr,"[Checking %s]\n",s);
  n = strlen(s);
  j = Head;
  for (i=0; i<n; i++) {
    if (s[i] != Buf[j]) return(-1);
    j = inc(j);
  }
  if (Buf[j] <= ' ') {
    if (Debug == 2) fprintf(stderr,"[Matched %s]\n",s);
    return(0);
  } else return(1);
}

notEOF() {
  wread();
  if (Buf[Head] != -1) return(1);
  else return(0);
}

irregularExit() {
  if (BeginVerbatim == 1) {
    printf("\\end{verbatim\x07d}\n");
  }
  exit(-1);
}

/*&jp
\end{document}
*/




