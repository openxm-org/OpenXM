#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <gmp.h>
#if defined(USEGC)
#include <gc.h>
#define MALLOC(a) GC_malloc(a)
#define MALLOC_ATOMIC(a) GC_calloc_atomic(a)
#define REALLOC(a,b) GC_realloc((a),(b))
#define FREE(a) GC_free(a)
#else
#define MALLOC(a) calloc((a),1)
#define MALLOC_ATOMIC(a) calloc((a),1)
#define REALLOC(a,b) realloc((a),(b))
#define FREE(a) free(a)
#endif

/* tag names. */
#define T_poly 0
#define T_node 1

typedef int64_t LONG;
typedef uint64_t ULONG;

typedef void *pointer;

typedef struct node {
  int tag;
  void *body;
  struct node *next;
} *Node;

#define NEWNODE(a) ((a)=(Node)MALLOC(sizeof(struct node)))

// ...->prev => ...->rev->cur, prev=cur
#define APPENDNODE(prev,cur,b) \
(NEWNODE(cur),(cur)->body=(void *)(b),(prev)->next=(cur),(prev)=(cur))

// prev->... => cur->prev->..., prev=cur
#define CONSNODE(prev,cur,b) \
(NEWNODE(cur),(cur)->body=(void *)(b),(cur)->next=(prev),(prev)=(cur))

typedef int Monomial;

typedef union {
  LONG f;
  mpz_ptr z;
  mpq_ptr q;
} Coef;

#define NEWZ(a) ((a)=(mpz_ptr)MALLOC(sizeof(mpz_t)))
#define NEWQ(a) ((a)=(mpq_ptr)MALLOC(sizeof(mpq_t)))
#define NEWCOEF(a) ((a)=(Coef)MALLOC(sizeof(union coef)))

typedef struct poly {
  Coef c;
  Monomial m;
  struct poly *next;
} *Poly;

#define NEWPOLY(a) ((a)=(Poly)MALLOC(sizeof(struct poly)))
#define APPENDPOLY(prev,cur,coef,mono) \
(NEWPOLY(cur),(cur)->c=(coef),(cur)->m=(mono),(prev)->next=(cur),(prev)=(cur))

typedef struct ring {
 int nv;
 char **vname;
 LONG chr;
 int (*mcomp)(Monomial,Monomial); 
 int (*zeroc)(Coef);
 Coef (*ntoc)(char *);
 void (*printc)(Coef);
 Coef (*addc)(Coef,Coef);
 Coef (*subc)(Coef,Coef);
 Coef (*mulc)(Coef,Coef);
 Coef (*divc)(Coef,Coef);
 Coef (*negc)(Coef);
 Coef one;
} *Ring;

extern Ring CurrentRing;
extern FILE *Input;
extern char *parse_string;
extern int parse_string_index;
extern Poly result;
extern Node mylist;

Ring create_ring(int nv,ULONG chr,char **v);
void show_ring(Ring ring);

#define NEWRING(a) ((a)=(Ring)MALLOC(sizeof(struct ring)))

Poly vtop(char *);
Poly itop(char *);
void check(void);
Node append_to_node(Node p,void *obj);
int yyparse(),yylex(),skipspace();
void init_calc(int nv,int chr,char **v);
Node eval_string(char *s);
void error(char *);
int Getc();
void Ungetc(int c);

Poly add_poly(Poly,Poly), sub_poly(Poly,Poly), neg_poly(Poly);
Poly mul_poly(Poly,Poly), divc_poly(Poly,Poly), power_poly(Poly,char *);
Poly dup_poly(Poly), mul1_poly(Poly,Poly);
void free_poly(Poly), print_poly(Poly);
LONG tdeg_poly(Poly p);

int zero_ff(Coef a);
Coef one_ff();
void print_ff(Coef a);
Coef add_ff(Coef a,Coef b), neg_ff(Coef a);
Coef sub_ff(Coef a,Coef b), mul_ff(Coef a,Coef b);
Coef mulsub_ff(Coef c,Coef a,Coef b);
Coef inv_ff(Coef s), div_ff(Coef a,Coef b);
Coef ntoc_ff(char *n);

int zero_z(Coef a);
Coef mpztoc(mpz_ptr t);
Coef one_z();
void print_z(Coef a);
Coef add_z(Coef a,Coef b), neg_z(Coef a);
Coef sub_z(Coef a,Coef b), mul_z(Coef a,Coef b);
Coef divexact_z(Coef a,Coef b), gcd_z(Coef a,Coef b);
Coef ntoc_z(char *n);

int zero_q(Coef a);
Coef mpqtoc(mpq_ptr t);
Coef one_q();
void print_q(Coef a);
Coef add_q(Coef a,Coef b), neg_q(Coef a);
Coef sub_q(Coef a,Coef b), mul_q(Coef a,Coef b);
Coef div_q(Coef a,Coef b);
Coef ntoc_q(char *n);

Node newnode(void *left,int tag);
Node append_node(Node a,Node b);
void print_node(Node a);
Node mycall(int type,Node x);

Poly divc_poly_d(Poly f, Coef c);
Node test0(Node x);
Poly reduce_d(Poly f, Coef c, Poly g);
Node test1(Node x);

int fmaxpos(Poly in[],int size);
int triangulate(Poly in[],Poly done[],int size);
Node test2_3_4(Node x,int m);
Poly normalize_d(Poly f);
Poly full_reduce_d(Poly f,Poly set[],int from,int to);
int rref(Poly done[],int donep);

int skipseparator(FILE *fp);
int skipseparator2(FILE *fp);
int getName(char *vname,FILE *fp);
char **define_ring_vars(char Fname[]);

int usage();
Poly poly_to_rule(Poly f,int from, int to);
Node rgb_to_rule(Node gb, int from, int to);
Node test5(Node x);
Node append_node2(Node a,Node b);





