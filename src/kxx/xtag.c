/* $OpenXM$
 */
#include <stdio.h>
int findChar(char s[],int c);
int findString(char s[],char a[]);
int findString2(char s[],char a[]);
char *getTag(char *s);
char *getKeyValue(char *s,char *key);
char *getBody(char *s);
void *xtagMalloc(int n);
void xtagFree(void *p);
void xtagError(char *s);
/*
  struct stringPair {
    char *first;
    char *rest;
  };
  char *getKeyValue(char *s, char *key);
  struct stringPair *getNextBlock(char *s);
*/

main_assert1() {
  /* asserting functions. */
  char *s1,*s2,*s3;
  s1 = " <login method=\"file\"> ";
  s2 = " <launch> ox -ox ox_asir </launch>";
  s3 = " <launch> ox <launch> hogera </launch></launch>";
  printf("%s\n",getTag(s1));
  printf("%s\n",getTag("hoge hoge hogera"));
  printf("%s\n",getTag("<hoge hoge hogera"));
  printf("%s\n",getKeyValue(s1,"method"));
  printf("%s\n",getKeyValue(s1,"user"));
  printf("%s\n",getBody(s2));
  printf("%s\n",getBody(s3));
}

void xtagError(char *s) {
  fprintf(stderr,"Error in xtag.c : %s\n",s);
  exit(10);
}
void *xtagMalloc(int n) {
  void *p;
  if (n <= 0) xtagError("malloc with negative arguments.");
  p = (void *)malloc(n);
  if (p == NULL) xtagError("No more memory.");
  return p;
}
void xtagFree(void *p) {
  free(p);
}

char *getTag(char *s) {
  int n,i,j;
  char *ans, *ans2;
  n = strlen(s);
  ans = (char *)xtagMalloc(n+1);
  ans[0] = 0;
  for (i=0; i<n; i++) {
	if (s[i] == '<') {
	  for (j=i+1; j<n; j++) {
		if ((s[j] == '>') ||
            (s[j] == '/' && s[j+1] == '>') ||
			(s[j] == ' ')) {
		  goto aaa;
		}
		ans[j-i-1] = s[j];
		ans[j-i] = 0;
	  }
	}
  }
  aaa: ;
  if (strlen(ans) == 0) return NULL;
  ans2 = (char *) xtagMalloc(strlen(ans)+1);
  strcpy(ans2,ans);
  xtagFree(ans);
  return ans2;
}

char *getKeyValue(char *s,char *key) {
  char *key2;
  int p;
  int i,start,end;
  char *ans;

  key2 = xtagMalloc(3+strlen(key)); /* search the string " key=" */
  strcpy(key2," ");
  strcat(key2,key);
  strcat(key2,"=");

  p = findString(s,key2);
  if (p < 0) return NULL;
  else {
	start = p + strlen(key2);
	/* skip blank and " */
	for (i=start; i<strlen(s) ; i++) {
	  if (s[i] == ' ' ||
		  s[i] == '\"') {
	  }else{
		start = i;
		break;
	  }
	}
	/* read key value */
	end = start;
	for (i = start; i<strlen(s); i++) {
	  if ((s[i] == ' ') ||
		  (s[i] == '\"')) {
		end = i;
		break;
	  }else{
		end++;
	  }
	}
	if (end - start <= 0) return NULL;
	ans = (char *)xtagMalloc((end-start)+1);
	for (i=start; i<end; i++) {
	  ans[i-start] = s[i];
	  ans[i-start+1] = 0;
	}
	return ans;
  }
}

char *getBody(char *s) {
  char *tag;
  char *startTag;
  char *endTag;
  char *ans;
  int start,end,i,level;

  tag = getTag(s);
  if (tag == NULL) return NULL;
  startTag = (char *)xtagMalloc(strlen(tag)+4);
  strcpy(startTag,"<");
  strcat(startTag,tag);
  strcat(startTag,">");
  endTag = (char *)xtagMalloc(strlen(tag)+4);
  strcpy(endTag,"</");
  strcat(endTag,tag);
  strcat(endTag,">");

  /* printf("%s,%s, %s\n",tag,startTag,endTag); */
  start = end = findString(s,startTag);
  if (start < 0) return NULL;
  start = start + strlen(startTag);
  level = 1;
  for (i=start; i<strlen(s); i++) {
	if (findString2(&(s[i]),startTag) == 0) {
	  level++;
	}else if (findString2(&(s[i]),endTag) == 0) {
	  level--;
	  if (level < 1) {
		end = i;
		break;
	  }
	}
  }
  if (end - start <= 0) return NULL;
  ans = (char *)xtagMalloc(end-start+1);
  for (i=start; i<end; i++) {
    ans[i-start] = s[i];
    ans[i-start+1] = 0;
  }
  return ans;
}

int findChar(char s[],int c) {
  int i;
  int n;
  n = strlen(s);
  for (i=0; i<n; i++) {
	if (s[i] == c) return i;
  }
  return -1;
}

int findString(char s[],char a[]) {
  int n,m,i,j;
  n = strlen(s);
  m = strlen(a);
  for (i=0; i<n-m; i++) {
	for (j=0; j<m; j++) {
	  if (s[i+j] != a[j]) break;
	  if (j == m-1) return i;
	}
  }
  return -1;
}
int findString2(char s[],char a[]) {
  int n,m,i,j;
  n = strlen(s);
  m = strlen(a);
  i = 0;
  for (j=0; j<m; j++) {
	if (s[i+j] != a[j]) break;
	  if (j == m-1) return i;
  }
  return -1;
}





