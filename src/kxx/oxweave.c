/*&eg
\documentclass{article}
\title{On oxweave.c}
\author{} \date{}
\begin{document}
\maketitle
\section{Introduction}
*/
/*&jp 
\documentclass{jarticle}
\title{oxweave のソースコードについての解説}
\author{} \date{}
\begin{document}
\maketitle
\section{前書き}
*/
/* $OpenXM: OpenXM/src/kxx/oxweave.c,v 1.4 1999/12/13 14:47:41 takayama Exp $ */
#include <stdio.h>

/* Modify here to change the begin tag and EndComment. Less than 9 characters.
*/
char *BeginTag0="/*\x026";  /* 0x26 = & */
char *BeginTag1="//\x026";  /* 0x26 = & */
char *EndComment0="*/";
char *EndComment1="\n";

#define BSIZE 256
#define VSIZE 256
static int Debug = 0;
static int Plain = 0;
/*&jp \noindent
  再帰 option を on にした場合 ({\tt Recursive = 1},
  {\tt LevelState1} で, 印刷すべき comment のネストのレベルを表す.
  {\tt LevelState2} で, 削除すべき comment のネストのレベルを表す.
*/
static int Recursive = 0;
static int LevelState1 = 0;
static int LevelState2 = 0;
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
  {\tt --plain} では,  verbatim を利用せずに生で出力.
  */
int OutputNoTaggedSegment = 0;
/*&jp \noindent 1 ならタグのついた場所を出力中. */
int OutputtingTaggedSegment = 0;
int BeginVerbatim = 0;


/*&jp \section{プログラム本体} */
main(int argc,char *argv[]) {
  extern char *BeginTag;
  extern char *EndComment0;
  extern char *EndComment1;
  extern int Plain;
  int c;
  int tagc,i;
  char *tagv[VSIZE];
  int tagc2;
  char *tagv2[VSIZE];
  int pos;
  Head = Tail = 0; Buf[0] = ' ';  /* initialize */

  /*&jp  {\tt tagv[]} にタグのあつまりをいれる. 
    {\tt tagv2[]} に対応するタグのおわりの記号をいれる.
    */
  tagc = tagc2 = 0;
  if (argc <= 1 || argc >= VSIZE) {
    usage();
    exit();
  }else{
    for (i=1; i< argc ; i++) {
      if (strcmp(argv[i],"--source") == 0) {
	OutputNoTaggedSegment = 1;
      }else if (strcmp(argv[i],"--plain") == 0) {
	Plain = 1; OutputNoTaggedSegment = 1;
      }else if (strcmp(argv[i],"--recursive") == 0) {
	Recursive = 1;
      } else{
	if (strcmp(argv[i]," ") == 0) {
	  argv[i] = "";
	}
	tagv[tagc] = (char *) malloc(sizeof(char)*(strlen(argv[i])+10));
	tagv2[tagc2] = (char *) malloc(sizeof(char)*10);
	strcpy(tagv[tagc],BeginTag0);
	strcat(tagv[tagc],argv[i]);
	tagv2[tagc] = EndComment0; 
	/* コメントのおわりの記号.  */
	tagc2++;
	tagc++;

	tagv[tagc] = (char *) malloc(sizeof(char)*(strlen(argv[i])+10));
	tagv2[tagc2] = (char *) malloc(sizeof(char)*10);
	strcpy(tagv[tagc],BeginTag1);
	strcat(tagv[tagc],argv[i]);
	tagv2[tagc] = EndComment1;
        tagc2++;
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
    /* We are in the state 0. */
    pos = findNextTag(tagc,tagv,tagc2,tagv2);
    /* printf(" ===pos=%d=== ",pos); */
    /* We are in the state 1. */
    findEndTag(tagc2,tagv2,pos);
  }
  if (BeginVerbatim) {
    if (!Plain) printf("\n\\end{verbatim\x07d}\n");
  }
  exit(0);
}

/*&jp \noindent 次の関数は利用法を表示する. */
usage() {
#include "oxweaveUsage.h"
}

#define inc(a) ((a+1) % BSIZE)
/*&jp \noindent {\tt wread()} は 標準入力よりのデータを読めるだけ
リングバッファ {\tt Buf} へ読み込む.*/
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

/*&jp  \noindent {\tt findNextTag()} は次の {\tt / *\&} なるタグをさがす.
   ( これは, {\tt BeginTag0} の値を変えると変更できる.)
  {\tt OutputNoTaggedSegment} が 1 ならデータをそのままながす.
  無視すべきタグのときは, タグ内部をスキップしたのち
  {\tt findNextTag} を再帰的に呼ぶ.
  */
findNextTag(int tagc, char *tagv[],int tagc2,char *tagv2[]) {
  int i;
  int c,d;
  extern char *BeginTag0;
  extern char *BeginTag1;
  do {
    for (i=0; i<tagc; i++) {
      /* fprintf(stderr,"\nChecking %s : ",tagv[i]); */
      if (wcmp(tagv[i]) == 0) {
	LevelState1++;
	/* fprintf(stderr," : matched."); */
	wgetc(strlen(tagv[i])+1);
	if (OutputNoTaggedSegment == 1 && BeginVerbatim == 1) {
	  BeginVerbatim = 0;
	  if (!Plain) printf("\\end{verbatim\x07d}\n");
	}
	OutputtingTaggedSegment = 1;
	return(i);  /* Now, state is 1. */
      }
    }
    /*&jp {\tt / *\&} だけどどのタグにも一致しない */
    if (wcmp(BeginTag0) == 1) {
      LevelState2++;
      wgetc(strlen(BeginTag0));
      while ((d=wgetc(1)) > ' ') ;
      /* We are in the state 2. */
      skipToEndTag(tagc2,tagv2,0);  
      /* We are in the state 0. */
      return(findNextTag(tagc,tagv,tagc2,tagv2));
    }else if (wcmp(BeginTag1) == 1) {
      LevelState2++;
      wgetc(strlen(BeginTag1));
      while ((d=wgetc(1)) > ' ') ;
      /* We are in the state 2. */
      skipToEndTag(tagc2,tagv2,1);  
      /* We are in the state 0. */
      return(findNextTag(tagc,tagv,tagc2,tagv2));
    }
    /* We are in the state 0 */
    c = wgetc(1);
    if (OutputNoTaggedSegment) {
      if (c != EOF) putchar(c);
    }
  }while( c!= EOF);
  if (BeginVerbatim == 1) {
    if (!Plain) printf("\n\\quad\n\\end{verbatim\x07d}\n");
  }
  exit(0);
}
      
/*&jp  \noindent {\tt findEndTag()} は次の {\tt * /} なるタグをさがす.
       ( これは, EndComment0 の値を変えると変更可能. )
      {\tt / /} で始まる場合は, 0xa がおわり. 
*/
findEndTag(int tagc,char *tagv[],int rule) {
  int i;
  int c;
  /* We are in the state 1. */
  do {
    i = rule;
    if (wcmp(tagv[i]) == 0) {
      LevelState1--;
      /* printf("LevelState1=%d\n",LevelState1);*/
      if (LevelState1 > 0 && Recursive) {
	wgetc(strlen(tagv[i]));
	printf("%s",tagv[i]);
	return(findEndTag(tagc,tagv,rule));
      }else{
	wgetc(strlen(tagv[i]));
	if (strcmp(tagv[i],"\n")==0) putchar('\n');
	OutputtingTaggedSegment = 0;
	if (OutputNoTaggedSegment) {
	  if (!Plain) printf("\n{\\footnotesize \\begin{verbatim}\n");
	  BeginVerbatim = 1;
	}
	return;			/* Our state is 0. */
      }
    }
    /* Our state is 1. */
    if (wcmp("/*") >= 0 ) {
      LevelState1++;
      /* printf("LevelState1++=%d\n",LevelState1); */
    }
    c = wgetc(1);
    putchar(c);
  }while( c!= EOF);
  fprintf(stderr,"findEndTag: unexpected EOF.\n");
  irregularExit();
}

skipToEndTag(int tagc,char *tagv[],int rule) {
  int i;
  int c;
  extern char *EndComment0;
  extern char *EndComment1;
  /* our state is 2. */
  do {
    if (rule == 0) {
      if (wcmp(EndComment0) == 0) {
	LevelState2--;
	if (LevelState2 > 0 && Recursive) {
	  wgetc(strlen(EndComment0));
	  return(skipToEndTag(tagc,tagv,rule));
	}else{
	  wgetc(strlen(EndComment0));
	  return;  /* our state is 0. */
	}
      }
    }else if (rule == 1) {
      if (wcmp(EndComment1) == 0) {
	LevelState2--;
	if (LevelState2 > 0 && Recursive) {
	  wgetc(strlen(EndComment0));
	  return(skipToEndTag(tagc,tagv,rule));
	}else{
	  wgetc(strlen(EndComment1));
	  return;  /* our state is 0. */
	}
      }
    }else{
      for (i=0; i<tagc; i++) {
	if (wcmp(tagv[i]) == 0) {
	  LevelState2--;
	  if (LevelState2 > 0 && Recursive) {
	    wgetc(strlen(EndComment0));
	    return(skipToEndTag(tagc,tagv,rule));
	  }else{
	    wgetc(strlen(tagv[i]));
	    return;  /* our state is 0. */
	  }
	}
      }

    }
    /* our state is 2. */
    if (wcmp("/*") >= 0) LevelState2++;
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
{\tt s} が 0xa,0 のときは, Buf[Head] が 0xa なら, 0 を戻す.
そうでないなら, -1 を戻す.
*/    
wcmp(char *s) {
  int n;
  int i,j;
  wread();
  if (Debug == 2) fprintf(stderr,"[Checking %s]\n",s);
  if (strcmp(s,"\n") == 0) {
    if (s[0] == Buf[Head]) return(0);
    else return(-1);
  }
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
    if (!Plain) printf("\\end{verbatim\x07d}\n");
  }
  exit(-1);
}


/*&jp
\end{document}
*/
/*&eg
\end{document}
*/




