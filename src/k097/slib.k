/* $OpenXM: OpenXM/src/k097/slib.k,v 1.3 2000/12/10 02:21:46 takayama Exp $ */
/*  slib.sm1, standard library. */
/* April 26-- , 1996 */
/* Don't use function names that is already used as a postscipt macro names*/
/* You may encounter operand stack overflow. */
/* sm1("(incmac.sm1) run (slib.sm1) run ");
*/
if (K00_verbose) 
 sm1(" ( slib.k (slib.ccc): 8/17,1996, 3/4 -- 3/10,1997 ) message ");

Helplist = [ ];
def void HelpAdd(s) {
  Helplist = Append(Helplist,s);
}

def Print(a) {                      /* print object without new line */
  sm1(a," messagen");
}
def Println(a) {                      /* print object with new line */
  sm1(a," message");
}
def Ln() {  sm1(" ( ) message"); }  /* newline */
/* Warning!! When use sm1 as f = sm1(...), Never set /FunctionValue. 
    Example: f = sm1(" 1 1 add /FunctionValue set ") causes error.
*/
def Poly(f) {
  sm1(f," (poly) data_conversion /FunctionValue set");
}
def PolyR(f,r) {                     /* parse the polynomial in R */
  sm1(f,r," ,, /FunctionValue set");
}
def Degree(f,v) {
  sm1(f,v," degree (universalNumber) dc /FunctionValue set");
}

def Append(f,g) { return(Join(f,[g])); }

def Length(f) { sm1(f," length (universalNumber) dc /FunctionValue set"); }

def Indexed(name,i) {
  sm1(name,i," s.Indexed /FunctionValue set ");
}
/* Indexed2("a",2,3) ---> "a[2,3]" */
def Indexed2(name,i,j) {
  sm1(name,i,j," s.Indexed2 /FunctionValue set ");
}
def Transpose(mat) {
  sm1(mat," transpose /FunctionValue set ");
}

sm1("
/s.Indexed {
  (integer) dc /arg2 set
  /arg1 set
  arg1 ([) arg2 (dollar) dc (]) 4 cat_n
} def

/s.Indexed2 {
  (integer) dc /arg3 set
  (integer) dc /arg2 set
  /arg1 set
  arg1 ([) arg2 (dollar) dc (,) arg3 (dollar) dc (]) 6 cat_n
} def
");

def Groebner(F) { /* Print("Input is "); Println(F); */
                   sm1(F," {[[(h). (1).]] replace homogenize} map /arg1 set
                            [arg1] groebner 0 get 
                            /FunctionValue set "); }

def GroebnerTime(F) { /* Print("Input is "); Println(F); */
                   sm1(F," {[[(h). (1).]] replace homogenize} map /arg1 set
                            { [arg1] groebner 0 get } timer
                            /FunctionValue set "); }

def LiftStd(F) { /* Print("Input is "); Println(F); */
                  sm1(F," {[[(h). (1).]] replace homogenize} map /arg1 set
                            [arg1 [(needBack)]] groebner 
                            /FunctionValue set "); }

def Reduction(f,G) { 
  sm1(f,G," reduction /FunctionValue set ");
}



def IntegerToSm1Integer(f) {
  sm1(f, " (integer) dc /FunctionValue set ");
}
def RingD(vList,weightMatrix,pp) {
  local new0,tmp,size,n,i,j,newtmp,ringpp,argsize;
  argsize = Length(Arglist);
  if (argsize == 1) {
    sm1("[", vList, 
        "ring_of_differential_operators ( ) elimination_order 0 ] define_ring
         /tmp set ");
    return(tmp);
  } else ;
  if (argsize == 2) {
    pp = 0;
  }
  pp = IntegerToSm1Integer(pp);
  size = Length(weightMatrix);
  new0 = NewVector(size);
  sm1(" /@@@.indexMode.flag.save @@@.indexMode.flag def ");
  sm1(" 0 @@@.indexMode ");
  PSfor (i=0; i<size; i++) {
    tmp = weightMatrix[i];
    n = Length(tmp);
    newtmp = NewVector(n);
    for (j=1; j<n; j = j+2) {
       newtmp[j-1] = tmp[j-1];
       newtmp[j] = IntegerToSm1Integer( tmp[j] );
    }
    new0[i] = newtmp;
  }
  ringpp =
  sm1("[", vList, 
      "ring_of_differential_operators ", new0, " weight_vector",pp, " ] define_ring");
  /* setRingVariables();  It doesn't work. It's a mystery. */
  sm1(" @@@.indexMode.flag.save @@@.indexMode ");
  return( ringpp );
}

/* RingD("x,y",[["x",2,"y",1]]);
   RingD("x,y");
*/



/* from lib/setvariables.ccc :  to generate sm1-package setvariables.sm1 */
/*  1997, 3/6 */
/* sm1(" 0 @@@.indexMode ");  C-like notation of matrix. a[0], ... */

def getxvar(i) {
  sm1( "[(x) (var) ", i , " ..int ] system_variable /FunctionValue set ");
}

def getdvar(i) {
  sm1( "[(D) (var) ", i , " ..int ] system_variable /FunctionValue set ");
}

def getvarn() {
  sm1( "[(N)] system_variable (universalNumber) dc /FunctionValue set ");
}

SetRingVariables_Verbose = true;
def SetRingVariables() {
  /* Don't use local variables in this function,
     because we set global variables in this function.
     cf. SSWork/yacc/memo.txt,  1997,3/6 */
  if (SetRingVariables_Verbose ) {
    Print("SetRingVariables() Setting the global variables : ");
  }
  k00setRingVariables(0,sm1( "[(CC)] system_variable (universalNumber) dc "));
  k00setRingVariables(sm1( "[(C)] system_variable (universalNumber) dc "),
                      sm1( "[(LL)] system_variable (universalNumber) dc "));
  k00setRingVariables(sm1( "[(L)] system_variable (universalNumber) dc "),
                      sm1( "[(MM)] system_variable (universalNumber) dc "));
  k00setRingVariables(sm1( "[(M)] system_variable (universalNumber) dc "),
                      sm1( "[(NN)] system_variable (universalNumber) dc "));
  if (SetRingVariables_Verbose) {Ln();}
}

def k00AreThereLeftBrace(s) {
  local leftBrace, jj, slist;
  leftBrace = sm1(" $[$ (array) dc 0 get (universalNumber) dc ");
  jj = Position(StringToIntegerArray(s),leftBrace);
  if (jj != -1) return(true); else return(false);
}

def void k00setRingVariables(tmp002_p,tmp002_q) {
  /* tmp002_ must not be used as variables names. */
  local tmp002_i,tmp002_v,tmp002_str;
  PSfor (tmp002_i=tmp002_p;tmp002_i<tmp002_q;tmp002_i++) {
    tmp002_v = getxvar(tmp002_i);
    if (k00AreThereLeftBrace(tmp002_v)) { ; } 
    else {
      if (SetRingVariables_Verbose) {Print(tmp002_v); Print(" ");}
      str = AddString(["/",tmp002_v," $",tmp002_v,"$ (poly) data_conversion def "]);
      sm1("[(parse) ",str," ] extension ");
    }
    tmp002_v = getdvar(tmp002_i);
    if (k00AreThereLeftBrace(tmp002_v)) { ; } 
    else {
      if (SetRingVariables_Verbose) {Print(tmp002_v); Print(" ");}
      str = AddString(["/",tmp002_v," $",tmp002_v,"$ (poly) data_conversion def "]);
      sm1("[(parse) ",str," ] extension ");
    }
  }
}
/* ---------------------------------- */

def AddString(f) {
  sm1(f,"  aload length cat_n /FunctionValue set ");
}

def IntegerToString(f) {
  sm1(f," (string) dc /FunctionValue set ");
}

def Replace(f,rule) {
  sm1(f,rule," replace /FunctionValue set ");
}

def AsciiToString(c) {
  sm1(c," (integer) dc (string) dc /FunctionValue set ");
}


/* From lib/tostr.ccc */
def ToString(p) {
  local n,ans,i;
  ans = [ ];
  if (IsArray(p)) {
    n = Length(p);
    ans = Append(ans,"[ ");
    for (i=0; i<n; i++) {
      ans = Append(ans,ToString(p[i]));
      if (i != n-1) {
        ans = Append(ans," , ");
      }
    }
    ans = Append(ans," ] ");
  } else {
    ans = [ sm1(p," (dollar) dc ") ];
    /* Println(ans);   */
  }   
  return(AddString(ans));
}

def IsArray(p) {
  sm1(p," isArray /FunctionValue set  ");
}


/* Println(tostr2([1,[2,3,4]])); */


def Denominator(f) {
  sm1(f," (denominator) dc /FunctionValue set ");
}

def Numerator(f) {
  sm1(f," (numerator) dc /FunctionValue set ");
}


def Replace(f,rule) {
  local ans,n,tmp,i,num,den;
  if (IsArray(f)) {
    n = Length(f);
    ans = [ ];
    for (i=0; i<n; i++) {
      ans = Append(ans, Replace(f[i],rule));
    }
    return(ans);
  }

  if (sm1(f," tag RationalFunctionP eq ")) {
     num = Numerator(f);
     den = Denominator(f);
     num = sm1(num,rule, " replace ");
     den = sm1(den,rule, " replace ");
     return( num/den );
  }

  sm1( f, rule , " replace /FunctionValue set ");
}



/* 1997, 3/7 */  
def Map(karg,func) {
  sm1(karg," { [ 2 -1 roll ] this 2 -1 roll [(parse) ",func," ] extension pop } map /FunctionValue set");
}
HelpAdd(["Map",
 ["Map(karg,func) applies the function <<func>> to the <<karg>>(string func).",
  " Ex. Map([82,83,85],\"AsciiToString\"):"]]);
/* test Map
def foo1(i) { return(i*2); }
def foo() {
  Println(Map([82,83,84],"foo1"));
}
*/

def Position(list,elem) {
  local n,pos,i;
  n = Length(list);
  pos = -1;
  for (i=0; i<n; i++) {
    if (elem == list[i]) {
      pos = i;
      sm1(" /k00.label0 goto ");
    }
  }
  sm1(" /k00.label0 ");
  return(pos);
}
HelpAdd(["Position",
 ["Position(list,elem) returns the position p of the element <<elem>> in",
  " the array <<list>>. If <<elem>> is not in <<list>>, it return -1",
  " (array list).",
  "Ex. Position([1,34,2],34): "]]);

def StringToIntegerArray(s) {
  sm1(s," (array) dc { (universalNumber) dc } map /FunctionValue set ");
}
HelpAdd(["StringToIntegerArray",
 ["StringToIntegerArray(s) decomposes the string <<s>> into an array of",
  "ascii codes of <<s>>  (string s).",
  "cf. AsciiToString."]]);
def StringToAsciiArray(s) { return(StringToIntegerArray(s)); }
HelpAdd(["StringToAsciiArray",
 ["StringToAsciiArray(s) is StringToIntegerArray(s)."]]);


def NewArray(n) {
 return(NewVector(n));
}
HelpAdd(["NewArray",
["NewArray(n) returns an array of size n (integer n)."]]);

def GetEnv(s) {
 sm1(" [(getenv) s] extension /FunctionValue set ");
}
HelpAdd(["GetEnv",
["GetEnv(s) returns the value of the environmental variable s (string s)."]]);
def Boundp(a) {
   local b;
   sm1("[(parse) [(/) ",a," ( load tag 0 eq 
                          { /FunctionValue 0 def }
                          { /FunctionValue 1 def } ifelse )] cat ] extension");
}
HelpAdd(["Boundp",
["Boundp(s) checks if the symbol s is bounded to a value or not (string s)."]]);
def Rest(a) {
  sm1(a," rest /FunctionValue set ");
}
HelpAdd(["Rest",
["Rest(a) returns the rest (cdr) of  a (list a)."]]);
def GetPathName(s) {
  local t,sss;
  sss = s;
  sm1(" [(stat) s] extension 0 get /t set ");
  if (Tag(t) == 0) {
    s=AddString([GetEnv("LOAD_K_PATH"),"/",s]);
    sm1(" [(stat) s] extension 0 get /t set ");
    if (Tag(t) == 0) {
      return(null); 
    }else{
      return(s);
    }
  }else{
    return(s);
  }
}
HelpAdd(["GetPathName",
["GetPathName(s) checks if the file s exists in the current directory or",
 "in LOAD_K_PATH. If there exists, it returns the path name (string s)."]]);

