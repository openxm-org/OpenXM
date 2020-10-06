/* $OpenXM: OpenXM/src/kan96xx/Kan/gradedset.c,v 1.9 2005/07/03 11:08:53 ohara Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include "datatype.h"
#include "extern2.h"
#include "gradedset.h"

#define INITSIZE 2  /* 2 is for debug */
static int Debug=0;

struct polySet *newPolySet(n)
     int n;
{
  struct polySet *g;
  int i;
  g = (struct polySet *)sGC_malloc(sizeof(struct polySet));
  g->g = (POLY *)sGC_malloc(sizeof(POLY)*(n+1));
  g->gh = (POLY *)sGC_malloc(sizeof(POLY)*(n+1));
  g->gmod = (POLY *)sGC_malloc(sizeof(POLY)*(n+1));
  g->gen = (int *)sGC_malloc(sizeof(int)*(n+1));
  g->del = (int *)sGC_malloc(sizeof(int)*(n+1));
  g->syz = (struct syz0 **)sGC_malloc(sizeof(struct syz0 *)*(n+1));
  g->mark = (int *)sGC_malloc(sizeof(int)*(n+1));
  g->serial = (int *)sGC_malloc(sizeof(int)*(n+1));
  if (g->g == (POLY *)NULL || g->del == (int *)NULL ||
      g->gh == (POLY *)NULL || g->gen == (int *)NULL ||
      g->gmod == (POLY *)NULL ||
      g->syz == (struct syz0 **)NULL || g->mark == (int *)NULL ||
      g->serial == (int *)NULL) {
    errorGradedSet("No more memory.");
  }
  g->lim = n;
  g->size = 0;
  for (i=0; i<n; i++) g->del[i] = g->mark[i] = 0;
  if (Debug) printf("newPolySet(%d)\n",n);
  return(g);
}

struct pair *newPair(prev)
     struct pair *prev;
{
  struct pair *g;
  int i;
  g = (struct pair *) sGC_malloc(sizeof(struct pair));
  if (g == (struct pair *)NULL) errorGradedSet("No more memory.");
  g->lcm = ZERO;
  g->ig = g->ii = -1;
  g->jg = g->ji = -1;
  g->next = (struct pair *)NULL;
  g->prev = (struct pair *)NULL;
  g->del = 0;
  return(g);
}

struct gradedPolySet *newGradedPolySet(n)
     int n;
{
  struct gradedPolySet *g;
  g = (struct gradedPolySet *)sGC_malloc(sizeof(struct gradedPolySet));
  if (g == (struct gradedPolySet *)NULL) errorGradedSet("No more memory.");
  g->polys = (struct polySet **)sGC_malloc(sizeof(struct polySet *)*(n+1));
  if (g->polys == (struct polySet **)NULL)
    errorGradedSet("No more memory.");
  g->maxGrade = 0;
  g->lim = n;
  g->gb = 0;
  g->reduced = 0;
  return(g);
}

struct gradedPairs *newGradedPairs(n)
     int n;
{
  struct gradedPairs *g;
  int i;
  g = (struct gradedPairs *)sGC_malloc(sizeof(struct gradedPairs));
  if (g == (struct gradedPairs *)NULL) errorGradedSet("No more memory.");
  g->pairs = (struct pair **)sGC_malloc(sizeof(struct pair *)*(n+1));
  if (g->pairs == (struct pair **)NULL) errorGradedSet("No more memory.");
  for (i=0; i<n; i++) {
    g->pairs[i] = newPair((struct pair *)NULL);
  }
  g->lim = n;
  g->maxGrade = 0;
  return(g);
}

struct syz0 *newSyz0() {
  struct syz0 *r;
  r = (struct syz0 *)sGC_malloc(sizeof(struct syz0));
  if (r == (struct syz0 *)NULL) errorGradedSet("newSyz0(): No memory.");
  r->cf = ZERO; r->syz = ZERO;
  return(r);
}
  
struct pair *pairCopy(node)
     struct pair *node;
{
  struct pair *r;
  r = newPair(node->prev);
  *r = *node;
  return(r);
}
  
struct gradedPairs *enlargeGradedPairs(size,grD)
     int size;
     struct gradedPairs *grD;
{
  struct gradedPairs *new;
  int i;
  new = newGradedPairs(size);
  for (i=0; i<grD->lim; i++) {
    new->pairs[i] = grD->pairs[i];
  }
  return(new);
}

void insertPair(inode,before)
     struct pair *inode;
     struct pair *before;
{
  struct pair *q;
  inode = pairCopy(inode);
  if (before == (struct pair *)NULL)
    errorGradedSet("insertPair(): *before must be a pair.");
  q = before->next;
  before->next = inode;
  inode->prev = before;
  inode->next = q;
  if (q != (struct pair *)NULL) {
    q->prev = inode;
  }
}

struct pair *deletePair(p)
     struct pair *p;
{
  struct pair *q;
  struct pair *r;
  if (p == (struct pair *)NULL)
    errorGradedSet("deletePair(): *p must be a pair.");
  if (p->next == (struct pair *)NULL) return((struct pair *)NULL);
  q = p->next->next;
  r = p->next;
  p->next = q;
  if (q != (struct pair *)NULL) {
    q->prev = p;
  }
  return(r);
}

struct pair *getPair_org(grD)
     struct gradedPairs *grD;
{
  int gmax,i;
  struct pair *pair;
  gmax = grD->maxGrade;
  for (i=0; i<gmax; i++) {
    if ((grD->pairs[i])->next != (struct pair *)NULL) {
      pair = deletePair(grD->pairs[i]);
      return(pair);
    }
  }
  return((struct pair *)NULL);
}

struct pair *getPair(grD)
     struct gradedPairs *grD;
{
  int gmax,i;
  struct pair *pair;
  POLY minp;
  struct pair *node,*minnode;
  gmax = grD->maxGrade;
  for (i=0; i<gmax; i++) {
    if ((grD->pairs[i])->next != (struct pair *)NULL) {
      node = grD->pairs[i]->next;
      minp = node->lcm;
      minnode = node;
      node = node->next;
      while (node != (struct pair *)NULL) {
        if ((*mmLarger)(minp,node->lcm) >= 1) {
          minnode = node;
          minp = minnode->lcm;
        }
        node = node->next;
      }
      pair = deletePair(minnode->prev);
      return(pair);
    }
  }
  return((struct pair *)NULL);
}


void whereInG(g,fi,gradep,indexp,sugar)
     struct gradedPolySet *g;
     POLY fi;
     int *gradep;
     int *indexp;
     int sugar;
{
  if (sugar) {
    if (*gradep < 0 ) {
      /* if sugar and *gradep < 0, then compute the grade.
         Otherwise, grade is given by the caller. */
      *gradep = grade_sugar(fi);
    }
  }else{
    *gradep = (*grade)(fi);
  }

  if (*gradep < 0) {
    warningGradedSet("whereInG(): the grade is -1.");
    return;
  }
  if (*gradep >= g->maxGrade) {
    *indexp = 0;
    return;
  }
  *indexp = g->polys[*gradep]->size;
  return;
}

struct gradedPolySet *putPolyInG(g,fi,grade,index,syz,mark,serial)
     struct gradedPolySet *g;
     POLY fi;
     int grade;
     int index;
     struct syz0 *syz;
     int mark;
     int serial;
{
  int i,j;
  struct polySet *polysNew;
  struct gradedPolySet *gnew;
  struct polySet *ps;

  /*printf("--------------------\n");
    outputGradedPolySet(g,0);*/

  if (grade < 0) {
    warningGradedSet("putPolyInG(): the grade is -1. The element is ignored.");
    return(g);
  }
  if (grade >= g->lim) {
    /* enlarge the gradedPolySet. */
    if (Debug) printf("Enlarge the gradedPolySet.\n");
    gnew = newGradedPolySet(grade*2+1);
    for (i=0; i<g->lim; i++) {
      gnew->polys[i] = g->polys[i];
    }
    for (i=g->lim; i<gnew->lim; i++) {
      gnew->polys[i] = newPolySet(INITSIZE);
    }
    gnew->maxGrade = g->maxGrade;
    gnew->gb = g->gb; gnew->reduced = g->reduced;
    g = gnew;
  }

  if (g->polys[grade]->lim <= index) {
    /* enlarge the polySet */
    if (Debug) printf("Enlarge the polySet.\n");
    polysNew = newPolySet(index*2+1);
    for (i=0; i<g->polys[grade]->lim; i++) {
      polysNew->g[i] = g->polys[grade]->g[i];
      polysNew->gh[i] = g->polys[grade]->gh[i];
      polysNew->gmod[i] = g->polys[grade]->gmod[i];
      polysNew->gen[i] = g->polys[grade]->gen[i];
      polysNew->del[i] = g->polys[grade]->del[i];
      polysNew->syz[i] = g->polys[grade]->syz[i];
      polysNew->mark[i] = g->polys[grade]->mark[i];
      polysNew->serial[i] = g->polys[grade]->serial[i];
    }
    polysNew->size = g->polys[grade]->size;
    g->polys[grade] = polysNew;
  }
  
  g->polys[grade]->size = index+1;
  g->polys[grade]->g[index] = fi;
  g->polys[grade]->gh[index] = POLYNULL;
  g->polys[grade]->gmod[index] = POLYNULL;
  g->polys[grade]->gen[index] = 0;
  g->polys[grade]->del[index] = 0;
  g->polys[grade]->syz[index] = syz;
  g->polys[grade]->mark[index] = mark;
  g->polys[grade]->serial[index] = serial;
  if (g->maxGrade < grade+1) g->maxGrade = grade+1;

  /*printf("grade=%d, index=%d\n",grade,index);
    outputGradedPolySet(g,0);*/
  return(g);
}

void markRedundant(g,fi,grade,index,sugar)
     struct gradedPolySet *g;
     POLY fi;
     int grade,index;
     int sugar;
{
  int i,j;
  struct polySet *ps;
  int start;
  
  /* mark redundant */
  if (sugar) start=0;else start=grade;
  for (i=start; i<g->maxGrade; i++) {
    ps = g->polys[i];
    for (j=0; j<ps->size; j++) {
      if (i == grade && j == index) {
      }else if ((*isReducible)(ps->g[j],fi)) {
		if (! ps->gen[j]) ps->del[j] = 1; /*?*/
      }
    }
  }
}

void markRedundant0(g,grade,index)
     struct gradedPolySet *g;
     int grade,index;
{
  int i,j;
  struct polySet *ps;
  POLY fi;

  fi = g->polys[grade]->g[index];
  /* mark redundant */
  for (i=0; i<g->maxGrade; i++) {
    ps = g->polys[i];
    for (j=0; j<ps->size; j++) {
      if (i == grade && j == index) {
      }else if ((*isReducible)(ps->g[j],fi)) {
        if (! ps->gen[j] ) ps->del[j] = 1; /*?*/
      }else if ((*isReducible)(fi,ps->g[j])) {
        if (! g->polys[grade]->gen[index] ) g->polys[grade]->del[index] = 1; /*?*/
        return;
      }
    }
  }
}

struct gradedPairs *putPairInGradedPairs(struct gradedPairs *grP,
                                         struct pair *top)
{
  if (grP == (struct gradedPairs *)NULL) {
    grP = newGradedPairs(top->grade +1);
  }
  if (grP->lim <= top->grade) {
    grP = enlargeGradedPairs(2*(top->grade)+1,grP);
  }
  insertPair(top,grP->pairs[top->grade]);
  grP->maxGrade = max(grP->maxGrade,top->grade+1);
  return(grP);
}
  
void errorGradedSet(s)
     char *s;
{
  fprintf(stderr,"Error in gradedset.c, red.c, gb.c: %s\n",s);
  exit(23);
}

void warningGradedSet(s)
     char *s;
{
  fprintf(stderr,"Warning in gradedset.c, red.c, gb.c: %s\n",s);
}


void outputGradedPolySet(grG,needSyz)
     struct gradedPolySet *grG;
     int needSyz;
{
  int i,j;
  struct polySet *set;
  extern int Ecart;
  printf("======== gradedPolySet ==========\n");
  printf("maxGrade=%d\n",grG->maxGrade);
  for (i=0; i<grG->maxGrade; i++) {
    set = grG->polys[i];
    printf("grade=%d, size=%d\n",i,set->size);
    for (j=0; j<set->size; j++) {
      printf("j=%d, del=%d, g=%s\n",j,set->del[j],POLYToString(set->g[j],'*',1));
	  if (Ecart) printf("     gh=%s\n",POLYToString(set->gh[j],'*',1));
      if (needSyz) {
        printf("mark=%d,serial=%d, syz.cf=%s, syz.syz=%s\n",set->mark[j],
               set->serial[j],POLYToString(set->syz[j]->cf,'*',1),
               POLYToString(set->syz[j]->syz,'*',1));
      }else{
        printf("mark=%d,serial=%d\n",set->mark[j],
               set->serial[j]);
      }
    }
  }
  printf("================================\n\n");
}

int countGradedPolySet(grG)
     struct gradedPolySet *grG;
{
  int i,j;
  struct polySet *set;
  int count = 0;
  for (i=0; i<grG->maxGrade; i++) {
    set = grG->polys[i];
    count += set->size;
  }
  return(count);
}


void outputGradedPairs(grP)
     struct gradedPairs *grP;
{
  int i,j;
  struct pair *pair;
  printf("============ gradedPairs ========\n");
  printf("maxGrade=%d\n",grP->maxGrade);
  for (i=0; i<grP->maxGrade; i++) {
    pair = grP->pairs[i]->next;
    printf("grade=%d\n",i);
    while (pair != (struct pair *)NULL) {
      printf("lcm=%s, \n",POLYToString(pair->lcm,'*',1));
      printf("(grade,index): (%d,%d) and (%d,%d)\n",pair->ig,pair->ii,pair->jg,pair->ji);
      printf("grade=%d\n",pair->grade);
      pair = pair->next;
    }
  }
  printf("============================\n\n");
}

void outputNode(pair)
     struct pair *pair;
{
  int i = 0;
  printf("=== list === \n");
  while (pair != (struct pair *)NULL) {
    printf("lcm=%s, \n",POLYToString(pair->lcm,'*',1));
    printf("(grade,index): (%d,%d) and (%d,%d)\n",pair->ig,pair->ii,pair->jg,pair->ji);
    printf("grade=%d\n",pair->grade);
    pair = pair->next;
    i++;
    if (i > 100) {
      printf("Too long list. Type in ret.");
      getchar(); getchar();
    }
  }
  printf("=========\n");
}


int countPairs(grD)
     struct gradedPairs *grD;
{
  int i;
  int c;
  struct pair *p;
  c = 0;
  for (i=0; i<grD->maxGrade; i++) {
    p = grD->pairs[i];
    p = p->next;
    while (p != (struct pair *)NULL) {
      c++;
      p = p->next;
    }
  }
  return(c);
}

struct gradedPolySet *gradedPolySetCopy(grG)
     struct gradedPolySet *grG;
{
  int i,j;
  struct polySet *ps,*psOld;
  struct gradedPolySet *newG;

  newG = newGradedPolySet(grG->maxGrade+1);
  for (i=0; i<grG->maxGrade;i++) {
    ps = newG->polys[i] = newPolySet((grG->polys[i]->size) +1);
    psOld = grG->polys[i];
    for (j=0; j<psOld->size; j++) {
      ps->g[j] = psOld->g[j];
      ps->del[j] = psOld->del[j];
      ps->syz[j] = psOld->syz[j];
      ps->mark[j] = psOld->mark[j];
      ps->serial[j] = psOld->serial[j];
    }
    ps->size = psOld->size;
  }
  newG->maxGrade = grG->maxGrade;
  return(newG);
}

int deletePairByCriterion2B(struct gradedPairs *grD,POLY gt,
                            struct gradedPolySet *grG)
{
  int gmax,i;
  struct pair *node;
  int count;
  POLY it;
  POLY ij;
  POLY jt;
  int ig,ii,jg,ji;
  count = 0;
  gmax = grD->maxGrade;
  for (i=0; i<gmax; i++) {
    if ((grD->pairs[i])->next != (struct pair *)NULL) {
      node = grD->pairs[i]->next;
      while (node != (struct pair *)NULL) {
        ig = node->ig; ii = node->ii;
        jg = node->jg; ji = node->ji;
        if ((*isReducible)(node->lcm,gt)) {
          ig = node->ig; ii = node->ii;
          jg = node->jg; ji = node->ji;
          it = (*lcm)(grG->polys[ig]->g[ii],gt);
          ij = (*lcm)(grG->polys[ig]->g[ii],grG->polys[jg]->g[ji]);
          jt = (*lcm)(grG->polys[jg]->g[ji],gt);
          if ((*mmLarger)(it,ij) != 2 &&
              (*mmLarger)(it,jt) != 2 &&
              (*mmLarger)(ij,jt) != 2) {
            node = deletePair(node->prev);
            count++;
          }
        }
        node = node->next;
      }
    }
  }
  return(count);
}

int markGeneratorInG(struct gradedPolySet *g,int grade,int index)
{
  g->polys[grade]->gen[index] = 1;
  return 1;
}

int clearGmod(struct gradedPolySet *gset) {
  int grd,i;
  struct polySet *set;
  for (grd=0; grd < gset->maxGrade; grd++) {
	set = gset->polys[grd];
	for (i = 0; i<set->size; i++) {
	  set->gmod[i] = POLYNULL;
	}
  }
}
