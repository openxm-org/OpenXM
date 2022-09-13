#include "lin.h"
#include <time.h>

Ring CurrentRing;
char *parse_string;
int parse_string_index;
int Verbose;

extern Poly result;
extern Node mylist;
struct node InAns;

#if defined(USEGC)
void *GC_calloc_atomic(size_t n)
{
  void *p;
  p = GC_malloc_atomic(n);
  memset(p,0,n);
  return p;
}

void *gc_realloc(void *p,size_t osize,size_t nsize)
{
  return (void *)GC_realloc(p,nsize);
}
#endif

void error(char *msg)
{
  fprintf(stderr,"%s\n",msg);
  exit(0);
}

// exp should be placed in the reverse order.
// (e1 ... en) -> (en ... e1)

int mcomp_simple(Monomial a,Monomial b)
{
  if (a>b) return 1;
  else if (a==b) return 0;
  else return -1;
}

int length(Node p)
{
  int i;
  for ( i = 0; p; p = p->next, i++ );
  return i;
}


LONG tdeg_poly(Poly p)
{
  if ( p == 0 ) return -1;
  if (p->m != 0) return 1;
  else return 0;
}

Poly neg_poly(Poly p)
{
  struct poly root;
  Poly q,r,s;

  if ( p == 0 ) return p;
  r = &root;
  for ( q = p; q != 0; q = q->next ) {
     APPENDPOLY(r,s,CurrentRing->negc(q->c),q->m);
  }
  r->next = 0;
  return root.next;
}

Poly add_poly(Poly p1,Poly p2)
{
  struct poly root;
  Poly r,q1,q2,q;
  Coef c;
  int (*mcomp)(Monomial,Monomial);

  if ( p1 == 0 )
    return p2;
  else if ( p2 == 0 )
    return p1;
  else {
    r = &root;
    mcomp = CurrentRing->mcomp;
    for ( q1 = p1, q2 = p2; q1 != 0 && q2 != 0; ) {
      switch ( (*mcomp)(q1->m,q2->m) ) { 
        case 0:
          c = CurrentRing->addc(q1->c,q2->c);
          if ( !CurrentRing->zeroc(c) ) {
            APPENDPOLY(r,q,c,q1->m);
          }
          q1 = q1->next; q2 = q2->next;
          break;
        case 1:
          APPENDPOLY(r,q,q1->c,q1->m);
          q1 = q1->next;
          break;
        case -1:
          APPENDPOLY(r,q,q2->c,q2->m);
          q2 = q2->next;
          break;
      }
    }
    if ( q1 ) {
      r->next = q1;
    } else {
      r->next = q2;
    }
    return root.next;
  }
}

Poly merge_poly(Poly p1,Poly p2)
{
  struct poly root;
  Poly r,q1,q2,q,t;
  Coef c;
  int (*mcomp)(Monomial,Monomial);

  if ( p1 == 0 )
    return p2;
  else if ( p2 == 0 )
    return p1;
  else {
    r = &root;
    mcomp = CurrentRing->mcomp;
    for ( q1 = p1, q2 = p2; q1 != 0 && q2 != 0; ) {
      switch ( (*mcomp)(q1->m,q2->m) ) { 
        case 0:
          c = CurrentRing->addc(q1->c,q2->c);
          if ( !CurrentRing->zeroc(c) ) {
            q1->c = c; r->next = q1; r = q1; q1 = q1->next; 
          } else {
            t = q1->next; FREE(q1); q1 = t;
          }
          t = q2->next;  FREE(q2); q2 = t;
          break;
        case 1:
          r->next = q1; r = q1;
          q1 = q1->next;
          break;
        case -1:
          r->next = q2; r = q2;
          q2 = q2->next;
          break;
      }
    }
    if ( q1 ) {
      r->next = q1;
    } else {
      r->next = q2;
    }
    return root.next;
  }
}

Poly sub_poly(Poly p1,Poly p2)
{
  return add_poly(p1,neg_poly(p2));
}

Poly mulc_poly(Coef c,Poly p)
{
  struct poly root;
  Poly q,r,s;

  r = &root;
  for ( q = p; q != 0; q = q->next ) {
     APPENDPOLY(r,s,CurrentRing->mulc(c,q->c),q->m);
  }
  r->next = 0;
  return root.next;
}


// p2 should be a constant
Poly divc_poly(Poly p1,Poly p2)
{
  struct poly root;
  Poly p,q,r,s;
  Coef c,c1;
  Monomial m;

  if ( p2 == 0 )
    error("divc_poly : division by 0");
  if ( p2->m != 0 )
    error("divc_poly : division by a non constant poly");
  if ( CurrentRing->divc == 0 )
    error("divc_poly : division is not allowed");
  r = &root;
  c = p2->c;
  for ( q = p1; q != 0; q = q->next ) {
     c1 = CurrentRing->divc(q->c,c); 
     APPENDPOLY(r,s,c1,p1->m);
  }
  r->next = 0;
  return root.next;
}



// bpe = byte per an exponent; 1,2,4,8
Ring create_ring(int number_of_vars,ULONG chr, char **v)
{
  Ring r;
  int i;
  int size;

  NEWRING(r);
  if (v==NULL) {
    r->nv = number_of_vars;
    size = log(number_of_vars)/log(10) + 2;
    r->vname = (char **)MALLOC(r->nv*sizeof(char *));
    for (i = 0; i<number_of_vars; i++) {
      r->vname[i] = (char *)MALLOC(size);
      sprintf(r->vname[i],"x%d",i+1);
    }
  }else{
    for (number_of_vars=0; v[number_of_vars] != NULL; number_of_vars++) ;
    r->nv = number_of_vars;
    r->vname = (char **)MALLOC(r->nv*sizeof(char *));
    for (i = 0; i<number_of_vars; i++) {
      r->vname[number_of_vars-i-1] = v[i];
    }
  }
  r->mcomp = mcomp_simple;
  r->chr = chr;
  if ( chr == 0 ) {
    r->zeroc = zero_z;
    r->ntoc = ntoc_z;
    r->addc = add_z;
    r->subc = sub_z;
    r->mulc = mul_z;
    r->divc = 0;
    r->negc = neg_z;
    r->printc = print_z;
    r->one = one_z();
  } else if ( chr == 1 ) {
    r->zeroc = zero_q;
    r->ntoc = ntoc_q;
    r->addc = add_q;
    r->subc = sub_q;
    r->mulc = mul_q;
    r->divc = div_q;
    r->negc = neg_q;
    r->printc = print_q;
    r->one = one_q();
  } else {
    r->zeroc = zero_ff;
    r->ntoc = ntoc_ff;
    r->addc = add_ff;
    r->subc = sub_ff;
    r->mulc = mul_ff;
    r->divc = div_ff;
    r->negc = neg_ff;
    r->printc = print_ff;
    r->one = one_ff();
  }
  return r;      
}

void show_ring(Ring r)
{
  char **v;
  int n,i;
  char *ordtype,*maxexp;

  fprintf(stderr,"/*");
  switch ( r->chr ) {
    case 0: fprintf(stderr,"ring=Z["); break;
    case 1: fprintf(stderr,"ring=Q["); break;
    default: fprintf(stderr,"ring=(Z/%dZ)[",(int)r->chr); break;
  }
  v = r->vname; n = r->nv;
  for ( i = 0; i < n; i++ ) {
    fprintf(stderr,"%s",v[i]);
    if ( i < n-1 ) fprintf(stderr,",");
  }
  fprintf(stderr,"]");
  fprintf(stderr,"\nThe last is largest. The first is largest in the input file by --vars. */\n");
}

int skipspace() {
  int c;

  for ( c = Getc(); ; )
    switch ( c ) {
      case ' ': case '\t': case '\r': case '\n':
        c = Getc();
        break;
      case '#': 
        while ((c = Getc()) != '\n') ;
	break;
      default:
        return c;
         break;
    }
}

void print_monomial(Monomial m)
{
  int nv; char **v;
  nv = CurrentRing->nv; 
  v = CurrentRing->vname; 
  if (m==0) printf("1");
  else printf("%s",v[m-1]);
}

void print_poly(Poly p)
{
  Poly q;

  if ( p == 0 ) {
    printf("0");
    return;
  }
  for ( q = p; q != 0; q = q->next ) {
    printf("+("); CurrentRing->printc(q->c); printf(")");
    if ( q->m != 0 ) {
      putchar('*'); print_monomial(q->m);
    }
  }
}

Node get_vars()
{
  struct node root;
  Node p,p1;
  int c,i;
  char buf[BUFSIZ];
  char *s;

  p = &root;
  while ( (c = Getc()) != '[' );
  while ( 1 ) {
    c = skipspace();
    if ( c == ']' ) {
      p->next = 0;
      return root.next;
    } else if ( c == ',' )
      ;  // skip a comma
    else {
      buf[0] = c;
      for ( i = 1; ; i++ ) {
        if ( i == BUFSIZ )
          error("get_vars : variable name too long");
        c = Getc();
        if ( !isalnum(c) ) {
          Ungetc(c);
          buf[i] = 0;
          break;
        } else
          buf[i] = c;
      }
      s = (char *)MALLOC(i+1);
      strcpy(s,buf);
      APPENDNODE(p,p1,s);
    }
  }
}

// create [x,y,z]
Node default_vars()
{
  char xyz[] = "xyz";
  int len = strlen(xyz),i;
  Node top,cur;
  char *s;

  for ( top = 0, i = len-1; i >= 0; i-- ) {
    s = (char *)MALLOC(2);
    s[0] = xyz[i]; s[1] = 0;
    CONSNODE(top,cur,s);
  }
  return top;
}


void init_calc(int nv,int chr,char **v)
{
#if defined(USEGC)
  GC_init();
  mp_set_memory_functions(
    (void *(*)(size_t))GC_malloc,
    (void *(*)(void *,size_t,size_t))gc_realloc,
    (void (*)(void *,size_t))GC_free);
#endif
  /* number_of_vars=3, char=0 */
  CurrentRing = create_ring(nv,chr,v);
}

// ringdef file format :
// chr ordid bpe [x y z ...]

Node eval_string(char *s)
{
  parse_string = s;
  parse_string_index = 0;
  yyparse();
  return mylist;
}

Poly itop(char *n)
{
  Coef c;
  Poly r;

  c = CurrentRing->ntoc(n);
  if ( CurrentRing->zeroc(c) ) return 0;
  else {
    NEWPOLY(r);
    r->c = c;
    r->m = 0;
    return r;
  }
}

/* variable name --> monomial */
Poly vtop(char *v)
{
  int nv,i,wpos,bpos,shift;
  char **vname;
  Poly r;
  Monomial m;

  nv = CurrentRing->nv;
  vname = CurrentRing->vname;

  for ( i = 0; i < nv; i++ )
    if ( !strcmp(v,vname[i]) ) break;
  if ( i == nv ) return 0;
  NEWPOLY(r);
  r->c = CurrentRing->one;
  r->m = i+1;
  r->next = 0;
  return r;
}

Monomial mul_monomial(Monomial m1,Monomial m2)
{
  if ((m1 == 0) || (m2 == 0)) return m1+m2;
  else fprintf(stderr,"Error: only linear polynomial is accepted.\n");
  return m1;
}

Poly mul1_poly(Poly p1,Poly p2)
{
  struct poly root;
  Poly p,q,r,s;
  Coef c,c1;
  Monomial m;

  r = &root;
  c = p1->c;
  for ( q = p2; q != 0; q = q->next ) {
     c1 = CurrentRing->mulc(c,q->c); 
     m = mul_monomial(p1->m,q->m);
     APPENDPOLY(r,s,c1,m);
  }
  r->next = 0;
  return root.next;
}

Poly mul_poly(Poly p1,Poly p2)
{
  Poly r,p,q;

  r = 0;
  for ( q = p1; q != 0; q = q->next ) {
    p = mul1_poly(q,p2);
    r = merge_poly(r,p);
  }
  return r;
}

Node newnode(void *left,int tag) {
  Node a;
  NEWNODE(a);
  a->tag=tag;
  a->body = left; a->next=NULL;
  return a;
}

void print_node(Node a) {
  if (a==NULL) {
    printf("[]"); return ;
  }
  printf("[");
  do {
    if (a->tag == T_poly) {
      print_poly((Poly)a->body);
    }else if (a->tag == T_node) {
      print_node((Node) a->body);
    }
    a=a->next;
    if (a != NULL) printf(",");
  }while (a != NULL);
  printf("]");
}

Node append_node(Node a,Node b) {
  Node last;
  if (a == NULL) return b;
  if (b == NULL) return a;
  last = a;
  while (last->next != NULL) last=last->next;
  last->next=b;
  return a;
}

/* 2022.02.03 */
/* Apply the function type for the Node x */
Node mycall(int type,Node x) {
  if (type==0) return test0(x);
  if (type==1) return test1(x);
  if (type==2) return test2_3_4(x,0);
  if (type==3) return test2_3_4(x,1); // rref
  if (type==4) return test2_3_4(x,2); // normalized rref,   E.
  if (type==5) return test5(x);       // gb to rules. F. a vars are set to 0.
  fprintf(stderr,"Error: function call of type=%d is not implemented. Return the argument.\n",type);
  return x;
}

/* _d means destructive. In other words, Poly f is changed. */
Poly divc_poly_d(Poly f, Coef c) {
  Poly q;
  if (CurrentRing->zeroc(c)) error("Division by c==0");
  if (f == NULL) return f;
  for (q=f; q != NULL; q=q->next) {
    q->c = CurrentRing->divc(q->c,c);
  }
  return f;
}
Node test0(Node x) {  
/*&usage A([f,g]); 
 Example:  A([3*x1+5,10]);
    [+(3/10)*x1+(1/2)]
*/
  Poly f;
  Poly g;
  Coef c;
  Node result;
  f = (Poly) x->body;
  g = (Poly) ((x->next)->body);
  c = g->c;
  divc_poly_d(f,c);
  return newnode(f,T_poly); 
}

/*
  f will be destructed.  g will be unchanged.
  f <- f - c*g
*/
Poly reduce_d(Poly f, Coef myc, Poly g) {
  Poly cm=NULL;
  struct poly root;
  Poly r,q1,q2,q,t,p1,p2;
  Coef c;
  Poly p;
  int (*mcomp)(Monomial,Monomial);
  Coef (*negc)(Coef c);
  int (*zeroc)(Coef c);
  if (f == NULL) {
    NEWPOLY(cm); cm->c=CurrentRing->negc(myc); cm->m=0; 
    return mul1_poly(cm,g);
  }
  if (g == NULL) return f;
  if (CurrentRing->zeroc(myc)) return f;
///  printf("reduce_d: ");print_poly(f);printf(" by "); print_poly(g); printf("\n"); 

  /* use the code of merge_poly */
  p1=f; p2=g;

  r = &root;
  mcomp = CurrentRing->mcomp;
  for ( q1 = p1, q2 = p2; q1 != 0 && q2 != 0; ) {
    switch ( (*mcomp)(q1->m,q2->m) ) { 
    case 0:
      c = CurrentRing->subc(q1->c,CurrentRing->mulc(myc,q2->c));
      if ( !CurrentRing->zeroc(c) ) {
	q1->c = c; r->next = q1; r = q1; q1 = q1->next; 
      } else {
	t = q1->next; FREE(q1); q1 = t;
      }
      t = q2->next;  q2 = t;
      break;
    case 1:
      r->next = q1; r = q1;
      q1 = q1->next;
      break;
    case -1:
      NEWPOLY(p);  /* g must not be modified */
      p->c = CurrentRing->negc(CurrentRing->mulc(myc,q2->c));
      p->m = q2->m;
      r->next = p; r = p;  
      q2 = q2->next;
      break;
    }
  }
  if ( q1 ) {
    r->next = q1;
  } else {
    NEWPOLY(cm); cm->c=CurrentRing->negc(myc); cm->m=0; 
    r->next = mul1_poly(cm,q2);
  }
  return root.next;
}

Node test1(Node x) {  
/*&usage B([f,c,g]); 
 Example:  B([2*x2+3*x1+5,2,x2+x1+1]);
           B([2*x2+3*x1+5,2,x2+x1+1]);
           B([3*x1,3/5,5*x1+1]);
           B([2*x2+3*x1,2,x2]);
*/
  Poly f;
  Poly cf;
  Poly g;
  Node result;
  f = (Poly) x->body;
  x = x->next;
  cf = (Poly) (x->body);
  x = x->next;
  g = (Poly) x->body;
  f = reduce_d(f,cf->c,g);  
  return newnode(f,T_poly); 
}

/* 2022.02.04 */
int fmaxpos(Poly in[],int size) {
  int pos=0;
  int mymax=-1;
  int i;
  for (i=0; i<size; i++) {
    if (in[i]==NULL) continue;
    if ((in[i])->m > mymax) {
      mymax=(in[i])->m;
      pos = i;
    }
  }
  return pos;
}

int triangulate(Poly in[],Poly done[],int size) {
  int in_m;
  int done_p=0;
  int i,maxpos,maxm;
  int mg;
  Coef cf,cg;
  Poly f,g;
  in_m=size;
  maxpos = fmaxpos(in,size);
  maxm=(in[maxpos])->m;
  if (maxm == 0) return 0;
  while (in_m > 0) {
    if (Verbose) {
      fprintf(stderr,"%d/%d, maxpos(pivot pos)=%d, maxm(var-1)=%d. ",in_m,size,maxpos,maxm); fflush(NULL);
    }
    g=in[maxpos]; in[maxpos]=0; in_m--;
    done[done_p]=g; done_p++;
    mg=g->m; cg=g->c;
    for (i=0; i<size; i++) {
      if (in[i]==NULL) continue;
      if (in[i]->m != mg) continue;
      f=in[i]; cf=f->c;
      in[i]=reduce_d(f,CurrentRing->divc(cf,cg),g);
      if (in[i]==NULL) in_m--;
      else {
	if ((in[i])->m == 0) return 0;  // no solution 
      }
    }
    if (in_m> 0) maxpos=fmaxpos(in,size);
  }
//for (i=0; i<size; i++) {printf("i=%d: ",i); print_poly(in[i]); printf("\n");}
  if (Verbose) fprintf(stderr,"Done triangulate.\n");
  return done_p;
}

Node test2_3_4(Node x,int m) {
  int size=0;
  Poly *in;
  Poly *done;
  int donep,i;
  Node xx;
  Node lt;
  xx=x;
  while (x != NULL) {size++; x=x->next;}
  in = (Poly *)MALLOC(sizeof(Poly)*size);
  done = (Poly *)MALLOC(sizeof(Poly)*size);
  for (i=0,x=xx; i<size; i++) {
    in[i] = (Poly) (x->body); x=x->next;
  }
  donep = triangulate(in,done,size);
  if (m==2) {
    for (i=0; i<donep; i++) done[i]=normalize_d(done[i]);
  }
  if (m) {
    rref(done,donep);
  }
  /* Destroy the list x */
  if (donep <= 0) return NULL;
  for (i=0, x=xx; i<donep; i++) {
    x->body = done[i];
    if (i != donep-1) x=x->next;
    else x->next=NULL;
  }

  lt=&InAns;
  for (i=0; i<donep; i++) {
     NEWNODE(lt->next);
     lt = lt->next; lt->next=NULL;
     if (done[i] == NULL) lt->body=NULL;
     else {
       NEWPOLY(lt->body);
       ((Poly) lt->body)->c=(done[i]->c);
       ((Poly) lt->body)->m=(done[i]->m);
       ((Poly) lt->body)->next=NULL;
     }
  }
  return xx;
}
/*&usage
  Example: 
    C([x3+2*x2+3*x1+4,5*x3+6*x2+7*x1+8,9*x3+10*x2+11*x1+12]);
    nd_gr([x3+2*x2+3*x1+4,5*x3+6*x2+7*x1+8,9*x3+10*x2+11*x1+12],[x3,x2,x1],0,0);
    [x3-x1-2,x2+2*x1+3]

    C([x3+2*x2+3*x1+4,5*x3+6*x2+7*x1+8,9*x3+10*x2-11*x1-12]);
    nd_gr([x3+2*x2+3*x1+4,5*x3+6*x2+7*x1+8,9*x3+10*x2-11*x1-12],[x3,x2,x1],0,0);
    [11*x3-10,11*x2+9,11*x1+12]
*/

Node test5(Node x) {
  int from,to,i;
  char **vname;
  from=to=-2;
  vname=CurrentRing->vname;
  for (i=0; i<CurrentRing->nv; i++) {
    /* printf("vname[%d]=%s\n",i,vname[i]); */
    if ((vname[i][0]=='a') && (from==-2)) from=i+1;
    if (vname[i][0]=='a') to=i+1;
    /* Note that vname[i] <--> m=i+1, see vtop() */
  }
  /* printf("from=%d, to=%d\n",from,to); */
  return rgb_to_rule(x,from,to);
}

/* f is modified */
Poly normalize_d(Poly f) {
  Coef myc;
  Poly ff;
  if (f == NULL) return f;
  myc = f->c;
  ff = f;
  while (f != NULL) {
    f->c = CurrentRing->divc(f->c,myc);
    f=f->next;
  }
  return ff;
}

/* Full reduce f by set[from]--set[to-1] 
   set is supposed to be triangulated.
   f is modified.
   効率悪いかも. 両方向リストにすべきか?
*/
Poly full_reduce_d(Poly f,Poly set[],int from,int to) {
  int i;
  Poly g,ff,r;
  ff=f;
  for (i=to-1; i>=from; i--) {
    f=ff;
    g = set[i];
    r=f;
    f=r->next;
    while (f) {
      if (f->m == g->m) {
	f=reduce_d(f,CurrentRing->divc(f->c,g->c),g);
	r->next=f;
      }
      r=f;
      if (r == NULL) break;
      f=r->next;
    }
///    print_poly(ff); printf("  full_reduce_d\n"); fflush(NULL); 
  }
  return ff;
}
/* row reduced form. done[] is assumed to be triangulated. */
int rref(Poly done[],int donep) {
  int i;
  Poly f;
  if (donep <= 0) return 0;
  for (i=donep-1; i>0; i--) {
    if (Verbose) {
      fprintf(stderr,"%d/%d. ",i,donep); fflush(NULL);
    }
    f=done[i-1];
///    print_poly(f); printf(": ");fflush(NULL);
    f=full_reduce_d(f,done,i,donep);
    done[i-1]=f;
///  {int j; for (j=i-1; j<donep; j++) {printf("j=%d: ",j); print_poly(done[j]); printf("\n"); } } 
  }
  if (Verbose) fprintf(stderr,"Done rref.\n");
  return donep;
}
/*&usage
  Example: (no solution)
    linsolv --char 11 <../Data/2022-02-04-linsov-small.txt
    
    ....
    C([x3+2*x2+3*x1+4,5*x3+6*x2+7*x1+8,9*x3+10*x2-11*x1-12]);
    E([x3+2*x2+3*x1+4,5*x3+6*x2+7*x1+8,9*x3+10*x2-11*x1-12]);
  Example:
    linsolv --nv 42 <../Data/2022-02-04-linsov-test3n.txt
*/

/* 2022.02.05 */
int isseparator(int c) {
  if (c==',') return 1;
  if ((c>=0) && (c <=' ')) return 1;
  return 0;
}
int isseparator2(int c) {
  if (c==',') return 1;
  if (c=='[') return 1;
  if (c==']') return 1;
  if ((c>=0) && (c <=' ')) return 1;
  return 0;
}
int skipseparator(FILE *fp) {
  int c;
  for (;;) {
    c = fgetc(fp);
    if (c == EOF) return c;
    if (isseparator(c)) continue;
    else return c;
  }
}
int getName(char *vname,FILE *fp) {
  int c;
  int i=0;
  vname[0]=0;
  c=skipseparator(fp);
  vname[i]=c; i++; vname[i]=0;
  if ((c == '[') || (c == ']')) return i;
  do {
    c = fgetc(fp);
    if (!isseparator2(c)) {
      vname[i]=c; i++; vname[i]=0;
    }else break;
  } while (1);
  return i;
} 

char **define_ring_vars(char Fname[]) {
  int nv=0;
  ULONG chr;
  int i;
  char **v;
  FILE *fp;
  int c;
  int len=0;
  char *vname;
  chr = CurrentRing->chr;
  fp = fopen(Fname,"r");
  if (fp == NULL) {
    fprintf(stderr,"File %s for define_ring_vars is not found.\n",Fname);
    return NULL;
  }
  while ((c=fgetc(fp)) != EOF) {
    len++;
    if (c == ',') ++nv;
  }
  rewind(fp);
  while ((c=fgetc(fp)) != '[') {
    if (c == EOF) error("no [ ] in ring_vars");
  }
  nv++;
  vname=(char *)MALLOC(len+1);
  v = (char **)MALLOC(sizeof(char *)*(nv+1));
  for (i=0; i<nv; i++) {
    getName(vname,fp);
    if (strcmp(vname,"[")==0) getName(vname,fp);
    if (strcmp(vname,"]")==0) break;
    v[i] = (char *)MALLOC(strlen(vname)+1);
    v[i+1]=NULL;
    strcpy(v[i],vname);
  }
  FREE(vname);
  return v;
}

/*
  returns -rest(f)
*/
Poly poly_to_rule(Poly f, int from, int to) {
  struct poly root;
  Poly r,g;
  Coef c2;
  Coef (*negc)(Coef);
  root.next = NULL;
  if (f == NULL) return NULL;
  r=&root; r->next=NULL; g=f->next;
  negc = CurrentRing->negc;
  while (g != NULL) {
    if ((g->m >= from) && (g->m <= to)) {
      g=g->next; continue;
    }
    c2 = negc(g->c);
    NEWPOLY(r->next);
    r=r->next;
    r->m = g->m;
    r->c = c2;
    r->next = NULL;
    g=g->next;
  }
  return root.next;
}
/*
  All inputs should be linear.
  [ ..., [in(gb[i]),-rest(gb[i]), ...]
  vars [from,to] is set to zero.
*/
Node rgb_to_rule(Node gb, int from, int to) {
  struct node root;
  Node r,r2;
  Poly f,g,in;
  int count=0;
  r=&root; r->next=NULL;
  while (gb != NULL) {
    f = (Poly) gb->body;
    g = poly_to_rule(f,from,to);
    if (f==NULL) in=NULL;
    else{
      NEWPOLY(in);
      in->c = f->c;  /* share coef with f in gb, todo OK? */
      in->m = f->m;
    }
    r2 = newnode(in,T_poly);
    r2->next = newnode(g, T_poly);
    r->next = newnode(r2,T_node);
    r = r->next;
    gb=gb->next;
    if (Verbose) {
      fprintf(stderr,"%d,",count++); fflush(NULL);
    }
  }
  if (Verbose) fprintf(stderr,". Done reduced GB to rule. a*'s in the right are set to 0.\n");
  return root.next;
}

/* not used */
Node append_node2(Node a,Node b) {
  Node last;
  if (a == NULL) {
    if (b != NULL) {
      if (b->tag == T_node) return newnode(b,T_node);
    }
    return b;
  }
  if (b == NULL) {
    return append_node2(NULL,a);
  }
  if (a->tag == T_node) a = newnode(a,T_node);
  return append_node(a,b);
}
