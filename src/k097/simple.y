/* $OpenXM$ */
/* simple.y 1996, 1/1 --- 1/6 */
/* simple.y.ccc,  1996, 4/1 --- */
%{
#include <stdio.h>
#include "d.h"
%}


%token ID QUOTE SINGLEQUOTE NUMBER

/* You have to change  isReserved() [d.c], too */
%token CLASS SUPER OPERATOR FINAL EXTENDS INCETANCEVARIABLE
%token THIS NEW SIZEOFTHISCLASS STARTOFTHISCLASS
%token MODULE PRINT LOCAL DEF SM1 LOAD TEST SPECIAL
%token AUTO BREAK CASE CHAR CONST CONTINUE DEFAULT DO DOUBLE ELSE ENUM
%token EXTERN FLOAT FOR GOTO IF INT LONG REGISTER
%token RETURN SHORT SIGNED SIZEOF STATIC STRUCT SWITCH TYPEDEF UNION
%token UNSIGNED VOLATILE VOID WHILE  PSFOR PROMPT

%right '=' PUT MULTPUT NEGATEPUT RESIDUEPUT

%left OR
%left AND
%left '|'
%left '&'
%left EQUAL NOTEQUAL
%left '<' '>' LESSEQUAL GREATEREQUAL
%left LEFTSHIFT RIGHTSHIFT
%left '+' '-'
%left '*' '/' '%'

%left UNARYMINUS '!' INCREMENT DECREMENT '~'
%right '^'

%left '.'
%left MEMBER  
%%

/* Mathematica-like parser */
program
  : globalstatements
  ;

globalstatements
  :  
    sentence globalstatements 
  | sentence            
  ;

sentence   /*  { ... } {...} :  ; does not need. */
  :
    for
  | PSfor
  | while
  | if
  | functionDefinition   {sendKan(0); }
  | exp ';'              {sendKan(0); }
  | exp ':'              {pkkan(" [ ] {showln} sendmsg2 \n"); sendKan(0); }
  | ';'                  {sendKan(0); }
  | class_definition     {sendKan(0); }
  | error ';'
  | PROMPT ';'           {sendKan(10); }
  ;

statements
  :  
    localsentence statements 
  | localsentence            
  ;

localsentence
  :
    for
  | PSfor
  | while
  | if
  | exp ';'          
  | return ';'
  | break ';'
  | ';'              
  ;

argList
  : exp ',' argList
  | exp   /* It does not cause reduce-reduce conflict
	     between "program"! */
  ;

return :
    RETURN exp   { pkkan(" /FunctionValue set  {/ExitPoint goto} exec %%return\n");}
  | RETURN  {pkkan("  {/ExitPoint goto} exec %%return void\n");}
  ;

break : BREAK  { pkkan(" exit "); }

list_prefix 
  :
    '['  { pkkan("[ "); }
  ;
list
  : list_prefix  ']'           { pkkan("  ] "); }
  | list_prefix argList ']'    { pkkan("  ] "); }
  ;

curryBrace
  :  '{'  '}'
  |  '{' statements '}'
  ;

if_prefix
  : IF '(' exp ')' { pkkan(" %% if-condition\n  { %%ifbody\n"); }
  ;
if_body
  : localsentence    { pkkan("  }%%end if if body\n  { %%if- else part\n"); }
  | curryBrace     { pkkan("  }%%end if if body\n  { %%if- else part\n"); }
  ;
if
  : if_prefix if_body                   { pkkan("  } ifelse\n"); }
/* The line cause 1 shift/reduce conflict. Look up a book. */
  | if_prefix if_body ELSE localsentence     { pkkan("  } ifelse\n"); }
  | if_prefix if_body ELSE curryBrace   { pkkan("  } ifelse\n"); }
  ;


for_prefix0
  : FOR '(' exp ';'  { pkkan("%%for init.\n%%for\n{ "); }
  | FOR '('     ';'  { pkkan("%%nothing for init.\n%%for\n{ "); }
  ;
for_exit
  : exp ';'    { pkkan(" {  } {exit} ifelse\n[ {%%increment\n"); }
  |     ';'    { pkkan("%%no exit rule.\n[ {%%increment\n"); }
  ;
for_inc
  : exp        { pkkan("} %%end of increment{A}\n"); }
  ;
/* [{ A } { B } roll 2 1] {exec} map ---> B A */
for_prefix
  : for_prefix0  for_exit  for_inc ')' 
               {  pkkan("{%%start of B part{B}\n"); }
  | for_prefix0  for_exit      ')'  
               {  pkkan("  } %% dummy A\n{%%start of B part{B}\n"); }
  ;
for
  : for_prefix curryBrace
               { pkkan("} %% end of B part. {B}\n"); 
                 pkkan(" 2 1 roll] {exec} map pop\n} loop %%end of for\n"); }

PSfor_prefix
  : PSFOR '(' ID '=' exp ';'
     { pkkan("%%PSfor initvalue.\n (integer) data_conversion \n");
       ips($3); 
     }
    ID '<' exp ';' ID INCREMENT  ')' 
     { ips($7); ips($10); 
       /* They must be equal id, but it is not checked. */
       pkkan(" (1).. sub  (integer) data_conversion  1  2 -1 roll \n");
       pkkan("{ %% for body\n (universalNumber) data_conversion ");
       pkkan("/"); printObjectSymbol($3); pkkan("  set \n");
     }
  ;
PSfor
  : PSfor_prefix curryBrace
     { pkkan("  } for \n"); }
  ;

while_prefix0
  : WHILE '('  { pkkan("\n%%while\n{ "); }
  ;

while_prefix
  : while_prefix0 exp ')'   { pkkan(" { } {exit} ifelse\n "); }
  | while_prefix0     ')'   { pkkan("%%no exit condition.\n "); }
  ;

while
  :  while_prefix curryBrace {   pkkan("} loop\n"); }
  ;


print
  : PRINT '(' exp ')' { pkkan(" print\n");}
  ;

sm1 :
    SM1 '(' sm1ArgList ')'
  ;

load :
    LOAD '(' QUOTE ')'   {   loadFile($3);  }
  | LOAD '(' ID ')'      {   loadFile($3);  }
  | LOAD ID              {   loadFile($2);  }
  | LOAD QUOTE           {   loadFile($2);  }
  | LOAD '[' QUOTE ']'   {   loadFileWithCpp($3); }

sm1ArgList :
  | sm1ArgList ',' QUOTE  { pkkan(" "); printObjectSymbol($3); pkkan(" "); }
  | QUOTE                 { pkkan(" "); printObjectSymbol($1); pkkan(" "); }
  | sm1ArgList ',' ID     { pkkan(" "); printObjectSymbol($3); pkkan(" "); }
  | ID                    { pkkan(" "); printObjectSymbol($1); pkkan(" "); }
  ;

primitive
  : ID
   { int tmp0;
     if ((tmp0 = K00getIncetanceVariable(objectSymbolToString($1))) != -1) {
       pkkan(" this "); pkkanInteger(tmp0); pkkan(" get ");
     } else {
       printObjectSymbol($1); pkkan(" ");
     }
   }
  | QUOTE     { pkkan("("); printObjectSymbol($1); pkkan(") "); }
  | SINGLEQUOTE
  | NUMBER    { pkkan("("); printObjectSymbol($1); pkkan(").. ");}
  | THIS      { pkkan(" this "); }
  | '(' exp ')'   
  | functionCall
  | list
  | print
  | sm1
  | load
  | test
  | special
  | set
  | arrayValue
  ;

functionCall_prefix
  :
    ID '('  { pkkan("this [ %% function args \n"); $$ = $1 }
  ;

/* function call. cf. Schreiner 19p */
/* If you want to have a strong binding s.t.
   ID[1,2], then you need to define a new non-terminal
   such as primitive. ????*/
functionCall
  :
    functionCall_prefix  argList  ')' 
       {pkkan("] {");printObjectSymbol($1);pkkan("} sendmsg2 \n");}
  | functionCall_prefix           ')'
       {pkkan("] {");printObjectSymbol($1);pkkan("} sendmsg2 \n");}

  ;
idList
  : idList ',' ID   { pkkan("/");printObjectSymbol($3); pkkan(" "); ips($3);}
  | ID              { pkkan("/");printObjectSymbol($1); pkkan(" "); ips($1);}
  ;

functionHead
  : DEF ID 
        { pkkan("/"); printObjectSymbol($2); pkkan(" {\n"); ips($2);
          pkdebug("In function : ", objectSymbolToString($2),
		  " of class ",K00getCurrentContextName());
	  pkkan(" /Arglist set /Argthis set /FunctionValue [ ] def\n [/this ");
	  $$ = $2;}
  ;

voidfunctionHead
  : DEF VOID ID 
        { pkkan("/"); printObjectSymbol($3); pkkan(" {\n"); ips($3);
          pkdebug("In function : ", objectSymbolToString($3),
		  " of class ",K00getCurrentContextName());
	  pkkan(" /Arglist set /Argthis set /FunctionValue [ ] def\n [/this ");
	  $$ = $2;}
  ;

functionArg
  : '(' idList ')'  '{' 
     {pkkan(" ] /ArgNames set ArgNames pushVariables [ %%function body\n");
      pkkan(" [Argthis] Arglist join ArgNames mapset\n");}
  | '('        ')'  '{' 
     {pkkan(" ] /ArgNames set ArgNames pushVariables [ %%function body\n");
      pkkan(" [Argthis]  ArgNames mapset\n"); }
  ;

functionBody
  :
    declaration extern_declaration statements '}' 
          { pkkan("/ExitPoint ]pop popVariables %%pop the local variables\n"); }
  |  extern_declaration statements '}'
  ;


functionDefinition
  : functionHead functionArg functionBody
        {pkkan("/ExitPoint ]pop popVariables %%pop argValues\n");
	 pkdebug2();
         pkkan("FunctionValue } def\n%%end of function\n\n"); }
  | operatorfunctionHead functionArg functionBody
        {pkkan("/ExitPoint ]pop popVariables %%pop argValues\n");
	 pkdebug2();
         pkkan("FunctionValue } def\n%%end of function\n\n"); }
  | voidfunctionHead functionArg functionBody
        {pkkan("/ExitPoint ]pop popVariables %%pop argValues\n");
	 pkdebug2();
         pkkan("} def\n%%end of function\n\n"); }
  ;

declaration_prefix
  : LOCAL { pkkan("[ %%start of local variables\n"); }
  ;

declaration
  : declaration_prefix  idList ';'  
       { pkkan("] pushVariables [ %%local variables\n"); }
  ;

extern_idList
  : extern_idList ',' ID   
           {     ;}
  | ID     {     ;}
  ;

extern_declaration_prefix
  : EXTERN {   ; }
  ;

extern_declaration
  : extern_declaration_prefix  extern_idList ';'  
       {  ; }
  |
  ;

arrayValue :
   array    { pkkan(" Get\n"); }
  ;
array :
   array_prefix arrayIndex ']' {pkkan(" ] "); }
  ;
arrayIndex :
    exp ',' arrayIndex   
  | exp                  
  ;
array_prefix :
    ID '['    
     { int tmp0;
       if ((tmp0 = K00getIncetanceVariable(objectSymbolToString($1))) != -1) {
         pkkan(" this "); pkkanInteger(tmp0); pkkan(" get [");
       } else {
         printObjectSymbol($1); pkkan(" [");
       }
     }
  ;


id_set
  : ID             { $$ = $1;}
  ;
set
  : id_set '=' exp 
    { int tmp0;
      if ((tmp0 = K00getIncetanceVariable(objectSymbolToString($1))) != -1) {
	pkkan(" this "); pkkanInteger(tmp0);
	pkkan(" 3 -1 roll   put\n");
      }else {
	pkkan("/"); printObjectSymbol($1); pkkan(" "); ips($1); pkkan(" set\n");
      }
    }
  | array '=' exp  { pkkan(" Put\n"); }
  | THIS '=' exp   { pkkan(" /this set \n"); }
  ;

exp 
  : primitive
  | new
  | '!' exp      { pkkan(" not\n"); }
  | '~' exp
  | '-' exp      { pkkan(" (0)..  2 1 roll {sub} sendmsg \n"); }
  | exp '+' exp  { pkkan(" {add} sendmsg2 \n"); }
  | exp '-' exp  { pkkan(" {sub} sendmsg2 \n"); }
  | exp '*' exp  { pkkan(" {mul} sendmsg2 \n"); }
  | exp '/' exp  { pkkan(" {div} sendmsg2 \n"); }
  | exp '%' exp
  | exp '^' exp  { pkkan(" power\n"); }
  | exp '&' exp
  | exp '|' exp
  | exp EQUAL exp { pkkan(" eq\n"); }
  | exp '<' exp { pkkan(" lt\n"); }
  | exp '>' exp { pkkan(" gt\n"); }
  | exp LESSEQUAL exp { pkkan(" lessThanOrEqual\n"); }
  | exp LEFTSHIFT exp
  | exp GREATEREQUAL exp { pkkan(" greaterThanOrEqual\n"); }
  | exp RIGHTSHIFT exp
  | exp AND exp { pkkan(" and\n"); }
  | exp OR exp  { pkkan(" or\n"); }
  | exp NOTEQUAL exp  { pkkan(" eq not\n"); }
  | exp PUT exp
  | id_set INCREMENT { pkkan("/");
		       printObjectSymbol($1); ips($1);
		       pkkan(" "); printObjectSymbol($1);
		       pkkan(" (1).. {add} sendmsg2 "); pkkan("def\n"); }
  | id_set DECREMENT { pkkan("/");
		       printObjectSymbol($1); ips($1);
		       pkkan(" "); printObjectSymbol($1);
		       pkkan(" (1).. {sub} sendmsg2 "); pkkan("def\n"); }
  | exp MEMBER exp
  | exp '.' member_functionCall {  pkkan(" sendmsg2 \n"); }
  | exp '.' ID   { /* Not implemented yet. */ }
  | super '.' member_functionCall {  pkkan(" supmsg2 \n"); }
  | NUMBER '.' NUMBER {  pkkan("("); printObjectSymbol($1);
			 pkkan(")..  (double) dc ");
			 pkkan("("); printObjectSymbol($3); pkkan(")..  ");
                         pkkan("("); printTens($3); pkkan(").. ");
			 pkkan(" div (double) dc  add\n"); }
  | exp RESIDUEPUT exp
  | exp NEGATEPUT exp
  | exp MULTPUT exp
  | SIZEOFTHISCLASS 
    {
      int tmp0;
      tmp0 = K00getIncetanceVariable("K00sizeof");
      pkkan(" ("); pkkanInteger(tmp0); pkkan(").. ");
    }

  | STARTOFTHISCLASS
    {
      int tmp0;
      tmp0 = K00getIncetanceVariable("K00start");
      pkkan(" ("); pkkanInteger(tmp0); pkkan(").. ");
    }
  ;

test : TEST '(' exp ')' 
          { /* It is not used now. */
	    sendKan(1);
	    Sm1obj = KSpop();
	    if (Sm1obj.tag != Sdollar) {
	      fprintf(stderr," Argument of test must be a string.\n");
	    }
	    testNewFunction((struct Object *)&Sm1obj); 
	  }
     ;

special : SPECIAL '(' list ')'
      {
          fprintf(stderr," special is used to extend the function. \n");
      }
      ;


member_functionCall
  :
    member_functionCall_prefix  argList  ')' 
       {pkkan("] {");printObjectSymbol($1);pkkan("}  ");}
  | member_functionCall_prefix           ')'
       {pkkan("] {");printObjectSymbol($1);pkkan("}  ");}

  ;
member_functionCall_prefix
  :
    ID '('  { pkkan(" [ %% function args \n"); $$ = $1 }
  ;


class_definition
  :
    class_definition_prefix incetance_variables globalstatements '}'
      { pkkan(" PrimitiveContextp setcontext ");
	/* debug */ K00foo1();
        K00toPrimitiveClass();
      }
  |
    class_definition_prefix  globalstatements '}'
      { pkkan(" PrimitiveContextp setcontext ");
	K00putIncetanceVariable(IEXIT," ");
	/* debug */ K00foo1();
	K00toPrimitiveClass();
      }
  ;

class_definition_prefix
  :
    CLASS ID EXTENDS ID '{'
      { ips($2);
	pkkan("[ $") ; printObjectSymbol($2); pkkan("$ ");
	printObjectSymbol($4); pkkan(" 0 get  newcontext ] /");
	printObjectSymbol($2);	pkkan(" set \n");
	printObjectSymbol($2); pkkan(" 0 get setcontext \n");

	if (K00declareClass(objectSymbolToString($2),
			    objectSymbolToString($4)) == -1) {
	  /* error */
	  KCerror("Super class has not been defined or Invalid class name.");
	}else{
	  K00putIncetanceVariable(IRESET," ");
	}
      }

incetance_variables
  : LOCAL incetance_variables_list ';'
      {
	K00putIncetanceVariable(IEXIT," ");
      }
  ;
incetance_variables_list
  : incetance_variables_list ',' ID
    {
      K00putIncetanceVariable(IPUT,objectSymbolToString($3));
    }
  | ID
    {
      K00putIncetanceVariable(IPUT,objectSymbolToString($1));
    }

operatorfunctionHead
  : DEF OPERATOR ID 
        { pkkan("/"); printObjectSymbol($3); pkkan(" {\n"); ips($3);
          pkdebug("In function : ", objectSymbolToString($3),", of class  ",
		  K00getCurrentContextName());
	  pkkan(" /Arglist set /Arglist [Arglist] def ");
          pkkan(" /Argthis set /FunctionValue [ ] def\n [/this ");
	  $$ = $3;}
  ;

super 
  : SUPER   { pkkan(" this "); }
  ;
  
new
  : NEW '(' exp ')'
       { int tmp;
	 tmp = K00getIncetanceVariable("K00sizeof")
	   +K00getIncetanceVariable("K00start");
	 pkkanInteger(tmp);
	 pkkan(" ");
	 pkkan(K00getCurrentContextName());
	 pkkan(" cclass \n");
       }
  | NEW '(' ')'
       { int tmp;
         pkkan(" PrimitiveObject ");
	 tmp = K00getIncetanceVariable("K00sizeof")
	   +K00getIncetanceVariable("K00start");
	 pkkanInteger(tmp);
	 pkkan(" ");
	 pkkan(K00getCurrentContextName());
	 pkkan(" cclass \n");
       }
  ;
	   





