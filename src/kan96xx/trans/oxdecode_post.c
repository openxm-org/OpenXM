/* $OpenXM: OpenXM/src/kan96xx/trans/oxdecode_post.c,v 1.2 2013/09/22 02:38:25 takayama Exp $ 
Decompose a given multi-part post message to URL encoded one for cgi.sm1
*/

/*
  - use some codes in plugin/oxcgi.c
    keyValuePairToUrlEncoding()
  - doPolymake.OoHG does not work over the reverse proxy.
  - data/testpost.sh is a test script for the input by curl.
  - src/polymake/cgi/cgi-polymake.sh
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int multipart(char *s);
char *urlEncoding(char *s);
int substr(char s[],int n,char a[]);
char *byteArrayToUrlEncoding(unsigned char *s,int size);
int isUrlEncoding3(char s);
int main() {
  char *s;
  int i,j,limit,len;
  int c;
  char slen[1024];
  s=getenv("CONTENT_LENGTH");
  if (s!=NULL) sscanf(s,"%d",&limit);
  else limit=-1;
  if (limit <= 0) {
    fprintf(stderr,"CONTENT_LENGTH does not seem to be set.\n");
    return(-1);
  }
  s = (char *)malloc(limit+1);
  for (i=0; i<limit; i++) {
    c = getchar();
    if (c < 0) break; else s[i]=c;
    s[i+1]=0;
  }
  if (!multipart(s)) {
    for (i=0; i<limit; i++) putchar(s[i]);
    return(0); 
  }
  s = urlEncoding(s);
  len=strlen(s);
  sprintf(slen,"%d",len);
  setenv("CONTENT_LENGTH",slen,1);
  for (i=0; i<len; i++) putchar(s[i]);
  /* need \n? */
}

/* if a is a substr of s, then it returns
   the index i of s[] so that &(s[i]) agrees with a.
   n is the size of s.
*/
int substr(char s[],int n,char a[]) {
  int m,i,j;
  m = strlen(a);
  for (i=0; i<= n-m; i++) {  /*BUG(< --> <=). Make corrections for other use!*/
	for (j=0; j<m; j++) {
	  if (s[i+j] != a[j]) break;
	  if (j == m-1) return i;
	}
  }
  return -1;
}

/* bug, they should follow RFC */
int multipart(char *s) {
  if ((strlen(s) > 3) && (strncmp(s,"---",3)==0)) return(1);
  else return(0);
}

char *urlEncoding(char *s) {
  int i,j,pos,pos2;
  char *name;
  char *body;
  char *body2;
  char *msg;
  pos = substr(s,strlen(s),"name=");
  if (pos < 0) return "error: no name=";
  pos += 6; pos2=pos-1;
  for (i=pos; i<strlen(s); i++) {
    if (s[i] == '"') {
      pos2 = i;
      break;
    }
  }
  if (pos2 < pos) return "error: name format error";
  name = (char *) malloc(pos2-pos + 2);
  for (i=0; i<pos2-pos; i++) {
    name[i] = s[i+pos]; name[i+1]=0;
  }

  pos = pos2;
  for (i=pos2; i<strlen(s); i++) {
    if ((s[i] == '\n') && (s[i+1] == '\n')) { /* \n\n */
	pos = i+2; break;
    }  
    if ((s[i] == 0xd) && (s[i+1] == '\n') && (s[i+2] == 0xd) && (s[i+3] == '\n')) { /* 0d, 0x */
	pos = i+4; break;
    }  
  }
  pos2 = pos-1;
  pos2=substr(&(s[pos]),strlen(s)-pos,"\n---");
  if (pos2 < 0) return "error: body format error";
  pos2++; pos2 += pos;
  body = (char *) malloc(pos2-pos+2);
  for (i=0; i<pos2-pos; i++) {
    body[i] = s[i+pos]; body[i+1]=0;
  }
  /* we have obtained the name and body */
  body2=byteArrayToUrlEncoding((unsigned char *)body,strlen(body));
  msg = (char *)malloc(strlen(name)+strlen(body2)+5);
  sprintf(msg,"%s=%s",name,body2);
  return msg;
}


/* . - _  A-Z a-z 0-9
   space --> +
*/
int isUrlEncoding3(char s) {
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
  r = (char *)malloc(n+1);
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
