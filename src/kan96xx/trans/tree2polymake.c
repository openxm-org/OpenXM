/* $OpenXM$ */
#include <stdio.h>
#include "../Kan/datatype.h"
#include "../Kan/stackm.h"
#include "../Kan/extern.h"
#include "../Kan/gradedset.h"
#include "../Kan/extern2.h"
#include "../Kan/kclass.h"
#include "../plugin/file2.h"
#include "trans.h"

int rationalToPolymakeS(struct object ob,FILE2 *fp) {
  struct object ob1;
  struct object ob2;
  int r;
  r = 0;
  if (ob.tag == SrationalFunction) {
    ob1 = *(Knumerator(ob));
    ob2 = *(Kdenominator(ob));
    r=integerToPolymakeS(ob1,fp);
    r=(fp2fputs("/",fp)<0?-1:r);
    r=(integerToPolymakeS(ob2,fp)<0?-1:r);
  }else if ((ob.tag == Sinteger) || (ob.tag == SuniversalNumber)) {
    r=integerToPolymakeS(ob,fp);
  }else{
    return(-1);
  }
  return(r);
}

int integerToPolymakeS(struct object ob,FILE2 *fp) {
  int r;
  char *s;
  char s2[254];
  r = 0;
  if (ob.tag == Sinteger) {
    sprintf(s2,"%d",ob.lc.ival);
    r=(fp2fputs(s2,fp)<0?-1:r);
  }else if (ob.tag == SuniversalNumber) {
    s=coeffToString(ob.lc.universalNumber);
    r=(fp2fputs(s,fp)<0?-1:r);
  }else{
    return(-1);
  }
  return(r);
}

int arrayToPolymakeS(struct object ob,FILE2 *fp,int format) {
  int r;
  int n,i;
  struct object ob1;
  r = 0;
  if (ob.tag != Sarray) {
    return(-1);
  }
  if (format == 0) {
    n = getoaSize(ob);
    for (i=0; i<n; i++) {
      ob1 = getoa(ob,i);
      if (ob1.tag == Sarray) {
        r=(arrayToPolymakeS(ob1,fp,format)<0?-1:r);
      }else{
        r=(integerToPolymakeS(ob1,fp)<0?-1:r);
        if (i != n-1) fp2fputc(' ',fp);
      }
    }
    fp2fputc('\n',fp);
  }else{
    n = getoaSize(ob);
    for (i=0; i<n; i++) {
      ob1 = getoa(ob,i);
      if (ob1.tag == Sarray) {
        if (i==0) fp2fputc('{',fp);
        r=(arrayToPolymakeS(ob1,fp,format)<0?-1:r);
        if (i==n-1) fp2fputc('}',fp);
      }else{
        r=(integerToPolymakeS(ob1,fp)<0?-1:r);
        if (i != n-1) fp2fputc(' ',fp);
      }
    }
    fp2fputc('\n',fp);
  }
  return(0);
}

int treeToPolymakeS(struct object ob,FILE2 *fp) {
  struct object obe;
  struct object obc;
  int r;
  int i,n;
  char *key;
  r = 0;
  if (ob.tag != Sclass) return(-1);
  if (ectag(ob) != CLASSNAME_tree) return(-1);
  obe = KtreeGetDocumentElement(ob);
  obc = KtreeGetChildNodes(ob);
  if (obe.tag != Sdollar) return(-1);
  if (obc.tag != Sarray) return(-1);
  key = KopString(obe);
  /* printf("key=%s\n",key); */
  if (strcmp(key,"data")==0) {
    /* Top node */
	n = getoaSize(obc);
    for (i=0; i<n; i++) {
      r=(treeToPolymakeS(getoa(obc,i),fp)<0?-1:r);
    }
  }
  else if (strcmp(key,"POINTS")==0) {
    /* array of array is expected. */
    fp2fputs("POINTS\n",fp);
    r=(arrayToPolymakeS(obc,fp,0)<0?-1:r);
    fp2fputc('\n',fp);
  }
  else if (strcmp(key,"FACETS") == 0) {
    fp2fputs("FACETS\n",fp);
    r=(arrayToPolymakeS(obc,fp,0)<0?-1:r);
  }
  /* Add more */
  else {
    /* Generic */
    fp2fputs(key,fp);
    fp2fputc('\n',fp);
    if (getoaSize(obc) > 0) {
      r=(arrayToPolymakeS(obc,fp,0)<0?-1:r);
    }
    fp2fputc('\n',fp);
  }
  fp2fputc('\n',fp);
  return(r);
}


struct object KtreeToPolymakeS(struct object ob) {
  FILE2 *fp;
  int r;
  int size;
  char *s,*s2;
  if (ob.tag != Sclass) errorKan1("%s\n","KtreeToPolymakeS: argument is not a tree.");
  if (ectag(ob) != CLASSNAME_tree) errorKan1("%s\n","KtreeToPolymakeS: argument is not a tree.");
  fp = fp2open(-1);
  if (fp == NULL) errorKan1("%s\n","KtreeToPolymakeS: fails to open a stream.");
  r = treeToPolymakeS(ob,fp);
  if (r < 0) errorKan1("%s\n","KtreeToPolymakeS: the tree may contain an object which cannot be translated.\n");
  s = fp2fcloseInString(fp,&size);
  if (s == NULL) errorKan1("%s\n","KtreeToPolymakeS:");
  s2 = (char *)sGC_malloc(size+2);
  if (s2 == NULL) errorKan1("%s\n","No more memory.");
  strcpy(s2,s);
  return KpoString(s2);
}
