/* $OpenXM: OpenXM/src/kan96xx/plugin/oxcgi.c,v 1.10 2005/07/03 11:08:54 ohara Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "file2.h"
#include "oxcgi.h"

static int ppp(char *s,int kstart,int kend, int vstart, int vend);
static int cgiHex(int p);

static void test1();
static void test2();
static void test3();
static void test4();

/* main() {KSstart();test4();} */

/*
  [["URL","http://www.openxm.org"],
   ["foo","value1"],                 <--- the first key value.
   ["hoge","value2"]
  ]
 */
struct object cgiUrlEncodingToKeyValuePair(char *s) {
  int i,n,start;
  int kstart,kend; /* start of key, end of key */
  int vstart,vend; /* start of value, end of value */
  int state;
  int nOfPairs;
  struct object rob = OINIT;
  struct object ob = OINIT;
  int k;
  n = strlen(s); start = -1;
  for (i=0; i<n; i++) {
    if (s[i] == '?') { start=i+1; break;}
  }
  if (start == -1) {
    start = 0;
    for (i=0; i<n; i++) {
      if (s[i] > ' ') { start = i; break; }
    }     
  }
  for (k=0; k<2; k++) {
    /* k==0 path one. Count nOfPairs. */
    /* k==1 path two. generate array. */
    nOfPairs = 0;
    i = start;
    /* state 0, 1 : key
       =    
       state 2, 3 : value
       &
       state 5    : after getting &
       state 4    : after getting eof
       state 6
    */
    state = 0;
    while (1) {
      switch(state) {
      case 0:
        kstart = kend = vstart = vend = -1;
        if (s[i] <= ' ') {
          state=6; break;
        } else {
          kstart = kend = i;
          nOfPairs++;
          i++;
          state=1;
          break;
        }
      case 1:
        if (s[i] <= ' ') {
          state=4; break;
        }else if (s[i] == '=') {
          state=2; i++; break;
        }else {
          kend = i; i++; break;
        }
      case 2:
        vstart = vend = -1;
        if (s[i] <= ' ') {
          state=4; break;
        }else if (s[i] == '&') {
          state=5; break;
        }else {
          state=3; vstart=vend=i; i++; break;
        }
      case 3:
        if (s[i] <= ' ') {
          state=4; break;
        }else if (s[i] == '&') {
          state=5; break;
        }else{
          vend=i; i++; break;
        }
      case 4:
        if (k == 1) {
          ob = newObjectArray(2);
          putoa(ob,0,urlEncodedStringToObj(s,kstart,kend,0));
          putoa(ob,1,urlEncodedStringToObj(s,vstart,vend,0));
          putoa(rob,nOfPairs,ob);
        }
        state = -1; break;
      case 5:
        if (k == 1) {
          ob = newObjectArray(2);
          putoa(ob,0,urlEncodedStringToObj(s,kstart,kend,0));
          putoa(ob,1,urlEncodedStringToObj(s,vstart,vend,0));
          putoa(rob,nOfPairs,ob);
        }
        i++; state = 0; break;
      case 6: state = -1; break;
      default: break;
      }
      if (state < 0) break;
      /* 
         if ((state == 4) || (state == 5)) {
         ppp(s,kstart,kend,vstart,vend);
         }
      */
    }
    if (k == 0) {
      char *stmp; int ii;
      rob = newObjectArray(nOfPairs+1);
      ob = newObjectArray(2);
      putoa(ob,0,KpoString("URL"));
      stmp = sGC_malloc(start+2);
      if (stmp == NULL) errorKan1("%s\n","No more memory.");
      stmp[0] = 0;
      for (ii=0; ii<start-1; ii++) {
        stmp[ii] = s[ii]; stmp[ii+1] = 0;
      }
      putoa(ob,1,KpoString(stmp));
      putoa(rob,0,ob);
    }
  }
  return rob;
}

/* . - _  A-Z a-z 0-9
   space --> +
*/
static int isUrlEncoding3(char s) {
  if ((s == '.') || (s == '-') || (s == '_')) return(0);
  if ((s >= 'A') && (s <= 'Z')) return(0); 
  if ((s >= 'a') && (s <= 'z')) return(0); 
  if ((s >= '0') && (s <= '9')) return(0);
  if (s == ' ') return(0);
  return(1);
} 

char *byteArrayToUrlEncoding(unsigned char *s,int size) {
  int n,i,j;
  char *r;
  n = 0;
  /* get Size */
  for (i=0; i<size; i++) {
    if (isUrlEncoding3((char)s[i])) n += 3;
    n++;
  }
  r = sGC_malloc(n+1);
  if (r == NULL) errorKan1("%s\n","No more memory.");
  r[0] = 0; r[n] = 0;
  i = 0; j = 0;
  while ((j < n) && (i<size)) {
    if (isUrlEncoding3((char)s[i])) {
      sprintf(&(r[j]),"%%%02X",s[i]); j += 3;
    }else{
      if ((char)s[i] == ' ') r[j]='+';
      else r[j] = s[i]; 
      j++; r[j] = 0;
    }
    i++;
  }
  return(r);
}
struct object urlEncodedStringToObj(char *s,int vstart,int vend,int mode)
     /*
       mode == 0.  Authmatically choose Sdollar or SbyteArray.
        [ not implemented yet. ]
      */
{
  struct object rob = OINIT;
  char *ts;
  char *ts2;
  int i,j;
  int p;
  if ((s == NULL) || (vstart < 0)) return(NullObject);
  if (vend+1-vstart <= 0) return(NullObject);
  ts = (char *) malloc(vend-vstart+1);
  if (ts == NULL) errorKan1("%s\n","Out of memory.");
  j = 0; ts[j] = 0;
  for (i=vstart; i<=vend; i++,j++) {
    ts[j] = 0;
    if (s[i] == '+') {
      ts[j] = ' '; ts[j+1] = 0;
    }else if (s[i] == '%') {
      p = cgiHex(s[i+1])*16+cgiHex(s[i+2]);
      i = i+2;
      ts[j] = p; ts[j+1] = 0;
    }else {
      ts[j] = s[i]; ts[j+1] = 0;
    }
  }
  ts2 = (char *)sGC_malloc(j);
  if (ts2 == NULL) errorKan1("%s\n","Out of memory.");
  for (i=0; i<j; i++) {
    ts2[i] = ts[i];
  }
  return KpoString(ts2);
}

static int cgiHex(int p) {
  if (p >= '0' && p <= '9') return (p-'0');
  if (p >= 'A' && p <= 'F') return (p-'A'+10);
  if (p >= 'a' && p <= 'f') return (p-'a'+10);
  errorKan1("%s\n","Invalid argument to cgiHex.");
}

/* for debug */
static int ppp(char *s,int kstart,int kend, int vstart, int vend) {
  int i;
  printf("%d %d %d %d\n",kstart,kend,vstart,vend);
  if (kstart >= 0) {
    printf("key=");
    for (i=kstart; i<=kend; i++) putchar(s[i]);
    printf("\n");
  }
  if (vstart >= 0) {
    printf("value=");
    for (i=vstart; i<=vend; i++) putchar(s[i]);
    printf("\n");
  }
}
static void test1() {
  char s[1000];
  cgiUrlEncodingToKeyValuePair("http://hoge.hoge?name=1231232&hoge=asdfsdf&foo=asdfasdf");
  cgiUrlEncodingToKeyValuePair("http://hoge.hoge?name=1231232&hoge=&foo=asdfasdf&");
  scanf("%s",s);
  cgiUrlEncodingToKeyValuePair(s);
}
static void test2() {
  char s[1000];
  struct object ob = OINIT;
  ob=cgiUrlEncodingToKeyValuePair("http://hoge.hoge?name=1231232&hoge=asdfsdf&foo=asdfasdf");
  printObject(ob,1,stdout);
  ob=cgiUrlEncodingToKeyValuePair("http://hoge.hoge?name=1231232&hoge=&foo=asdfasdf&hoge=A%41+%42%62y%21");
  printObject(ob,1,stdout);
  scanf("%s",s);
  ob=cgiUrlEncodingToKeyValuePair(s);
  printObject(ob,1,stdout);
}

static void test4() {
  char s[1000];
  struct object ob = OINIT;
  char *ts;
  int size;
  ob=cgiUrlEncodingToKeyValuePair("http://hoge.hoge?name=1231232&hoge=&foo=asdfasdf&hoge=A%41+%42%62y%21");
  printObject(ob,1,stdout);
  ts = cgiKeyValuePairToUrlEncoding(ob);
  printf("result=%s",ts);


  ts = "Pragma: no-cache\nContent-Length:  2915\nContent-Type: text/html\nConnection: close\n\n                <DIV class=Section1> \n    <P class=MsoNormal \n   style=\"mso-list: none; mso-list-ins: \" 19991102T2025\">&nbsp;\n      </P> ";

  ob=cgiHttpToKeyValuePair(ts,strlen(ts)); 
  printObject(ob,1,stdout);
  ts = cgiKeyValuePairToHttp(ob,&size);
  printf("result:\n%s",ts);

}
/* end for debug */


char *cgiKeyValuePairToUrlEncoding(struct object ob) {
  FILE2 *fp;
  int size;
  fp = fp2open(-1);
  if (fp == NULL) errorKan1("%s\n","cgiKeyValuePairToUrlEncoding: open error.");
  cgiKeyValuePairToUrlEncodingFile2(ob,fp);
  return fp2fcloseInString(fp,&size);
}
int checkKeyValuePairFormat(struct object ob,char *msg) {
  int i,n;
  struct object eob = OINIT;
  struct object eob0 = OINIT;
  struct object eob1 = OINIT;
  static char *fmt = NULL;
  int size;
  char *ss;
  size = 1024;
  if (fmt == NULL) fmt = sGC_malloc(size);
  if (fmt == NULL) errorKan1("%s\n","No more memory.");
  for (i=0; i<size; i++) fmt[i]=0;
  ss = "%s\n In ";
  strcpy(fmt,ss);
  strncpy(&(fmt[strlen(ss)]),msg,size-strlen(ss)-2);

  if (ob.tag != Sarray) errorKan1(fmt,"checkKeyValuePairFormat: argument must be an array.");
  n = getoaSize(ob);
  for (i=0; i<n ; i++) {
    eob = getoa(ob,i);
    if (eob.tag != Sarray) errorKan1(fmt,"checkKeyValuePairFormat: argument must be an array of arrays.");
    if (getoaSize(eob) != 2) errorKan1(fmt,"checkKeyValuePairFormat: argument must be an array of arrays of size 2.");
    eob0 = getoa(eob,0); eob1 = getoa(eob,1);
    if (eob0.tag != Sdollar) errorKan1(fmt,"checkKeyValuePairFormat: the key word must be a string.\n");
  }
  return 0;
}

int cgiKeyValuePairToUrlEncodingFile2(struct object ob,FILE2 *fp) {
  int n,i;
  struct object eob = OINIT;
  struct object eob0 = OINIT;
  struct object eob1 = OINIT;
  char *key, *s;
  checkKeyValuePairFormat(ob,"cgiKeyValuePairToUrlEncodingFile2");
  n = getoaSize(ob);
  for (i=0; i<n; i++) {
    eob = getoa(ob,i);
    eob0 = getoa(eob,0); eob1 = getoa(eob,1);
    key = KopString(eob0);
    if ((i == 0) && (strcmp(key,"URL")==0)) {
      if (eob1.tag != Sdollar) errorKan1("%s\n","URL value must be a string.");
      fp2fputs(KopString(eob1),fp);
      if ( n > 1 ) fp2fputc('?',fp);
    }else{
      fp2fputs(key,fp); fp2fputc('=',fp);
      if (eob1.tag == Snull) ;
      else if (eob1.tag == Sdollar) {
        s = KopString(eob1);
        fp2fputs(byteArrayToUrlEncoding((unsigned char *)s, strlen(s)),fp);
      }else if (eob1.tag == SbyteArray) {
        fp2fputs(byteArrayToUrlEncoding(KopByteArray(eob1),getByteArraySize(eob1)),fp);
      }else{
        errorKan1("%s\n","Value is not string nor byte array.");
      }
      if (i < n-1) fp2fputc('&',fp);
    }
  }
  return(fp2fflush(fp));
}

static struct object rStringToObj(char *s,int vstart,int vend,int mode) {
  /* mode has not yet been used. */
  struct object rob = OINIT;
  char *sss; int i;
  int bytearray;
  bytearray=0;
  if (vend < vstart) return NullObject;
  for (i=vstart; i<= vend; i++) {
    if (s[i] == 0) bytearray=1;
  }
  if (bytearray) {
    rob = newByteArrayFromStr(&(s[vstart]),vend-vstart+1);
    return(rob);
  }
  sss = (char *)sGC_malloc(vend-vstart+1);
  if (sss == NULL) errorKan1("%s\n","No more memory.");
  for (i=vstart; i<=vend; i++) {
    sss[i-vstart] = s[i]; sss[i-vstart+1] = 0;
  }
  rob = KpoString(sss);
  return(rob);
}

/*
  [["Content-Body$,"Body"],
   ["key1","value1"],
   ["key2","value2"],
   ...
  ]
*/
struct object cgiHttpToKeyValuePair(char *s,int size) {
  int ssize,i,j,k;
  int nOfPairs, startbody,state, kstart,kend,vstart, vend,startline,endline;
  int nextstart,path;
  struct object rob = OINIT;
  struct object ob = OINIT;
  ssize = strlen(s);
  nOfPairs = 0; startbody = -1;
  /* state==0 :  readline and set startline and endline; state = 1;
     state==1 :  if the line is empty, then state=10;
                 Determine kstart,kend (key) and vstart,vend (value); state=0;
     state==10 : Read the body.
  */
  for (path=0; path<2; path++) {
    if (path == 1) {
      rob = newObjectArray(nOfPairs+1); nOfPairs = 0;
    }
    i = 0;  state=0;
    while (i<size) {
      if (state == 0) {
        /* Read the line */
        startline=i; endline= size-1; nextstart = size;
        for (j = startline; j<size; j++) {
          if (s[j] == 0xd) {
            endline = j-1;
            if (s[j+1] == 0xa) nextstart = j+2; 
            else nextstart = j+1;
            break;
          }else if (s[j] == 0xa) {
            endline = j-1; nextstart = j+1; break;
          }
        }
        state = 1;  i = nextstart;
      }else if (state == 1) {
        if (endline <= startline) { state=10; }
        else {
          kstart=startline; kend = endline;
          vstart=endline+1; vend = endline;
          for (j=startline; j<=endline; j++) {
            if (s[j] > ' ') {kstart = j; break;}
          }
          for (j=kstart; j<=endline; j++) {
            if (s[j] == ':') { kend=j-1; break; }
          }
          for (j=kend+2; j<=endline; j++) {
            if (s[j] > ' ') {vstart = j; break; }
          }
          for (j=vend; j >= vstart; j--) {
            if (s[j] > ' ') { vend=j; break;}
            else vend = j-1;
          }
          /* ppp(s,kstart,kend,vstart,vend); */
          nOfPairs++;
          if (path == 1) {
            ob = newObjectArray(2);
            putoa(ob,0,rStringToObj(s,kstart,kend,0));
            putoa(ob,1,rStringToObj(s,vstart,vend,0));
            putoa(rob,nOfPairs,ob);
          }
          state = 0;
        }
      }else {
        startbody = i;
        if (path == 1) {
          ob = newObjectArray(2);
          putoa(ob,0,KpoString("Content-Body"));  
          putoa(ob,1,rStringToObj(s,startbody,size-1,0));
          putoa(rob,0,ob);
        }
        break;
      }
    }
  }
  return rob;
}

char *cgiKeyValuePairToHttp(struct object ob,int *sizep) {
  char *s;
  FILE2 *fp;
  int size;
  fp=fp2open(-1);
  cgiKeyValuePairToHttpFile2(ob,fp);
  s = fp2fcloseInString(fp,sizep);
  return(s);
}

int cgiKeyValuePairToHttpFile2(struct object ob,FILE2 *fp) {
  int n,i;
  struct object eob = OINIT;
  struct object eob0 = OINIT;
  struct object eob1 = OINIT;
  char *key, *s;
  checkKeyValuePairFormat(ob,"cgiKeyValuePairToHttpFile2");
  n = getoaSize(ob);
  if (n == 0) return(0);
  for (i=1; i<n; i++) {
    eob = getoa(ob,i);
    eob0 = getoa(eob,0); eob1 = getoa(eob,1);
    key = KopString(eob0);
    fp2fputs(key,fp); fp2fputs(": ",fp);
    if (eob1.tag == Snull) ;
    else if (eob1.tag == Sdollar) {
      s = KopString(eob1);
      fp2fputs(s,fp);
    }else{
      errorKan1("%s\n","Value is not a string.");
    }
    if (i < n-1) fp2fputc('\n',fp);
    else fp2fputs("\n\n",fp);
  }
  eob = getoa(ob,0);
  eob0 = getoa(eob,0); eob1 = getoa(eob,1);
  key = KopString(eob0);
  if (strcmp(key,"Content-Body") != 0) warningKan("Key word should be Content-Body.\n");
  if (eob1.tag == Sdollar) {
    fp2fputs(KopString(eob1),fp);
  }else if (eob1.tag == SbyteArray) {
    fp2write(fp,KopByteArray(eob1),getByteArraySize(eob1));
  }else errorKan1("%s\n","BODY must be a string or a byte array.");

  return(fp2fflush(fp));
}


static void test3() {
  char *s;
  struct object ob = OINIT;
  s = "Pragma: no-cache\nContent-Length:  2915\nContent-Type: text/html\nConnection: close\n\n                <DIV class=Section1> \n    <P class=MsoNormal \n   style=\"mso-list: none; mso-list-ins: \" 19991102T2025\">&nbsp;\n      </P> ";

  ob=cgiHttpToKeyValuePair(s,strlen(s)); 
  printObject(ob,1,stdout);

  s = "Pragma:\nContent-Length:  2915\r\nContent-Type: text/html\nConnection: close\n\n                <DIV class=Section1> \n    <P class=MsoNormal \n   style=\"mso-list: none; mso-list-ins: \" 19991102T2025\">&nbsp;\n      </P> ";
  ob=cgiHttpToKeyValuePair(s,strlen(s)); 
  printObject(ob,1,stdout);

  s = "Pragma\nContent-Length  2915\r\nContent-Type text/html\nConnection: close\n\n                <DIV class=Section1> \n    <P class=MsoNormal \n   style=\"mso-list: none; mso-list-ins: \" 19991102T2025\">&nbsp;\n      </P> ";
  ob=cgiHttpToKeyValuePair(s,strlen(s)); 
  printObject(ob,1,stdout);

  {
    char *s; 
    s = "This is a pen.com? !@#0989\n";
    printf("\n%s\n",byteArrayToUrlEncoding((unsigned char *)s,strlen(s)));
  }
}

struct object cgiKeyValuePairToUrlEncodingString(struct object ob) {
  char *s;
  s = cgiKeyValuePairToUrlEncoding(ob);
  if (s == NULL) return NullObject;
  return KpoString(s);
}
struct object cgiKeyValuePairToHttpString(struct object ob) {
  int size;
  char *s;
  s = cgiKeyValuePairToHttp(ob,&size);
  if (s == NULL) return NullObject;
  return KpoString(s);
}

struct object KooStringToUrlEncoding(struct object sob) {
  unsigned char *s;
  char *rs;
  int n;
  if (sob.tag == Sdollar) {
    s = (unsigned char *) KopString(sob);
    n = strlen((char *)s);
  }else if (sob.tag == SbyteArray) {
    s = KopByteArray(sob);
    n = getByteArraySize(sob);
  }else errorKan1("%s\n","KooStringToUrlEncoding: argument must be a string or a bytearray.");
  rs = byteArrayToUrlEncoding(s,n);
  return KpoString(rs);
}

struct object KooUrlEncodedStringToObj(struct object sob) {
  char *s;
  int n;
  if (sob.tag == Sdollar) {
    s = KopString(sob);
    n = strlen((char *)s);
  }else if (sob.tag == SbyteArray) {
    s = KopByteArray(sob);
    n = getByteArraySize(sob);
  }else errorKan1("%s\n","KooUrlEncodedStringToObj: argument must be a string.");
  return urlEncodedStringToObj(s,0,n-1,0);
}

static struct object toTokens(char *s,int *sep,int nsep) {
  /* s is the input, and sep are the separators. */
  /* -1 means <=' ' are separators */
  int nOfTokens,n,i,done,k,start,sav;
  struct object rob = OINIT;
  char *t;

  rob = NullObject;
  if (nsep < 1) return rob;
  if (sep[0] != -1) {
	fprintf(stderr,"cgiToTokens: Not implemeted for this separator.\n");
	return rob;
  }

  /* Count the number of tokens */
  n = strlen(s); i = 0; nOfTokens=0;
  while (i < n) {
	done = 0;
	while (s[i] <= ' ') {
	  i++; if (i >= n) { done=1; break;}
	}
	if (done==1) break;
	nOfTokens++;
	while (s[i] > ' ') {
	  i++; if (i >= n) { done=1; break; }
	}
	if (done == 1) break;
  }

  rob = newObjectArray(nOfTokens);
  n = strlen(s); i = 0; k = 0;
  while (i < n) {
	done = 0;
	while (s[i] <= ' ') {
	  i++; if (i >= n) { done=1; break;}
	}
	if (done==1) break;
	start = i;
	while (s[i] > ' ') {
	  i++; if (i >= n) { done=1; break; }
	}
	t = (char *) GC_malloc(i-start+1);
	if (t == NULL) { fprintf(stderr,"No more memory.\n"); exit(10); }
	t[i-start] = 0;
    strncpy(t,&(s[start]),i-start);
	putoa(rob,k,KpoString(t));
	k++;
	if (done == 1) break;
  }

  return rob;
}
  
struct object KooToTokens(struct object ob,struct object sep) {
  char *s;
  int n;
  int tmp[1];
  tmp[0] = -1;
  if (ob.tag == Sdollar) {
    s = KopString(ob);
    n = strlen((char *)s);
  }else errorKan1("%s\n","KooToTokens: the first argument must be a string.");
  if (sep.tag == Sarray) {
	if (getoaSize(sep) != 0) {
	  errorKan1("%s\n","This separators have not been implemented.");
	}
  }else errorKan1("%s\n","KooToTokens: the second argument(separators) must be an array.");
  return toTokens(s,tmp,1);
}
