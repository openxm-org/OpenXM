%{
#if defined(USEGC)
#define malloc(x) MALLOC(x)
#define free(x) FREE(x)
#endif

#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "lin.h"
void yyerror(char *);
Poly result;
Node pvar[26];
FILE *Input;
Node mylist;
%}

%start start

%union {
  Poly f;
  char *s;
  int i;
  pointer p;
  Node n;
}

%token <s> VAR
%token <s> INT
%token <i> PVAR
%token <i> '+' '-' '*' '/' '[' ']' '='

%type <f> expr
%type <n> list
%type <n> array
%type <n> funcall

%left ','
%right '='
%left '+' '-' 
%left PLUS
%left MINUS
%left '*' '/'
%%

start : expr ';'
  { 
    mylist = newnode($1,T_poly); 
    YYACCEPT; 
  }
   | list ';'
       { mylist = $1;
           YYACCEPT;
        }
   | funcall ';' { mylist = $1; YYACCEPT; }
   | PVAR '=' list ';'
       { pvar[$1] = $3; mylist = $3; YYACCEPT; }
   | PVAR '=' funcall ';'
       { pvar[$1] = $3; mylist = $3; YYACCEPT; }
   | PVAR '=' expr ';'
       { pvar[$1] = newnode($3,T_poly); mylist = pvar[$1]; YYACCEPT; }
   | PVAR ';'
       { mylist = pvar[$1]; YYACCEPT; }
;
list : '[' array ']' {$$=$2; }
      | '[' ']' {$$=NULL; }
/* todo, remove shift reduce conflicts. */
      | '[' list ',' array ']' {$$=append_node(newnode($2,T_node),$4); }
      | '[' array ',' list ']' {$$=append_node($2,newnode($4,T_node)); }
      | '[' list ',' list ']' {$$=append_node(newnode($2,T_node),newnode($4,T_node)); }
      | '[' list ']' {$$=newnode($2,T_node); }

array : expr { $$=newnode($1,T_poly); } 
       | list {$$=newnode($1,T_node); }
       | funcall {$$ = newnode($1,T_node); }
       | PVAR
          { $$ = newnode(pvar[$1],T_node); }
       | array ',' array 
        { $$=append_node($1,$3); } 
/*        { $$=append_node2($1,$3); } */
funcall : PVAR '(' list ')'  { $$=mycall($1,$3); }
       |  PVAR '(' PVAR ')'  { $$=mycall($1,pvar[$3]); }
expr : '+' expr   %prec PLUS
  { $$ = $2; }
     | '-' expr   %prec MINUS
  { $$ = neg_poly($2); }
     | expr '+' expr
  { $$ = add_poly($1,$3); }
     | expr '-' expr
  { $$ = sub_poly($1,$3); }
     | expr '*' expr
  { $$ = mul_poly($1,$3); }
     | expr '/' expr
  { $$ = divc_poly($1,$3); }
     | INT
  { $$ = itop($1); }
     | VAR
  { $$ = vtop($1); }
     | '(' expr ')'
  { $$ = $2; }
;
%%

void yyerror(char *s)
{
  fprintf(stderr,"parser : %s\n",s);
}

extern char *parse_string;
extern int parse_string_index;

int Getc()
{
  if ( parse_string != 0 ) {
    return parse_string[parse_string_index++];
  } else
    return getc(Input);
}

void Ungetc(int c)
{
  if ( parse_string != 0 ) {
    parse_string[--parse_string_index] = c;
  } else
    ungetc(c,Input);
}

int yylex()
{
  int c,i,bufsize;
  char *buf,*s;;

  buf = MALLOC(BUFSIZ);
  bufsize = BUFSIZ;

  switch ( c = skipspace() ) {
    case EOF :  
     exit(0);
      break;
    case '+': case '-': case '*': case '/': case '^':
    case '(': case ')': case ';': case ',': case '=':
    case '[': case ']': 
     yylval.i = c;
     return c;
     break;
    default:
     break;
  }
  if ( isdigit(c) ) {
    buf[0] = c;
    for ( i = 1; ; i++ ) {
      if ( i == bufsize ) {
        bufsize *= 2;
        buf = REALLOC(buf,bufsize);
      }
      c = Getc();
      if ( !isdigit(c) ) {
        Ungetc(c);
        buf[i] = 0;
        break;
      } else
        buf[i] = c;
    }
    s = (char *)MALLOC(i+1);
    strcpy(s,buf);
    yylval.s = s;
    return INT;
  } else if ( islower(c) ) {
    buf[0] = c;
    for ( i = 1; ; i++ ) {
      c = Getc();
      if ( (!isalnum(c)) && (c != '_') ) {
        Ungetc(c);
        buf[i] = 0;
        break;
      } else
        buf[i] = c;
    }
    s = MALLOC(i+1);
    strcpy(s,buf);
    yylval.s = s;
    return VAR;
  } else if ( isupper(c) ) {
    yylval.i = c-'A';
    return PVAR;
  } else
   return 0; // dummy return
}
