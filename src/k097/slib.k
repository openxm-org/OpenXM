/* $OpenXM: OpenXM/src/k097/slib.k,v 1.9 2001/01/08 05:26:49 takayama Exp $ */
/*  slib.sm1, standard library. */
/* April 26-- , 1996 */
/* Don't use function names that is already used as a postscipt macro names*/
/* You may encounter operand stack overflow. */
/* sm1("(incmac.sm1) run (slib.sm1) run ");
*/
if (K00_verbose) 
 sm1(" ( slib.k (slib.ccc): 8/17,1996, 3/4 -- 3/10,1997 ) message ");


/*
  Each Helplist[i] has the format
   [ category, [ key, explanations, ( args ,) (refs ,) (short descriptions)]]
     category : null or string
     key      : string
     explanations : string or array of string   (Ex. and << >> are key words)
     args     : null or list of strings
     refs     : null or list of strings
     short descriptions : string
*/
Helplist = [ ];
def void HelpAdd(s,category) {
  local n;
  n = Length(Arglist);
  if (n <= 1) {
    category = null;
  }
  if (true) {
    /* Assert the args */
    /* You can use functions only defined before using HelpAdd */
    if (!(n == 1 || n == 2)) {
      Println(s);
      Error("HelpAdd: wrong argument length.");
    }
    if (!(Tag(category) == 0 || Tag(category) == 5)) {
      Println(category);
      Error("HelpAdd: wrong category.");
    }
    if (!(Tag(s) == 6)) {
      Println(s);
      Error("HelpAdd: s must be an array.");
    }
    if (! (Tag(s[0]) == 5)) {
      Println(s);
      Error("HelpAdd: s[0] must be a string.");
    }
    if (! (Tag(s[1]) == 5 || Tag(s[1]) == 6)) {
      Println(s);
      Error("HelpAdd: s[1] must be a string or an array.");
    }
    /* End of assert */
  }
  s = [category,s];
  Helplist = Append(Helplist,s);
}

def Tag(f) {
  local ans;
  ans = sm1(f," etag (universalNumber) dc ");
  return(ans);
}
def Error(s) {
  sm1(" s error ");
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

/*
def Reduction(f,G) { 
  sm1(f,G," reduction /FunctionValue set ");
}
*/
def Reduction(f,myset) {
  local n, indexTable, set2, i, j, tmp, t_syz,r,rng,
        vsize,tt;
  vsize = null;
  r = GetRing(Poly("1"));  /* Save the current ring */
  rng = GetRing(f);
  if (Tag(rng) == 0) {
    rng = GetRing(myset);
  }
  if (Tag(rng) != 0) {SetRing(rng);}

  if (IsArray(f)) {
     vsize = Length(f);
     sm1(" [f] fromVectors 0 get /f set ");
  }

  n = Length(myset);
  if (n > 0) {
    if (IsArray(myset[0])) {
      if (vsize != Length(myset[0])) {
         Error("Reduction: size mismatch.");
      }
      sm1(" myset fromVectors /myset set ");
    }
  }

  indexTable = NewArray(n);
  set2 = [ ];
  j = 0;
  for (i=0; i<n; i++) {
    if (Tag(myset[i]) == 0) {
      indexTable[i] = -1;
    }else if (myset[i] == Poly("0")) {
      indexTable[i] = -1; 
    }else{
      set2 = Append(set2,myset[i]);
      indexTable[i] = j;
      j++;
    }
  }
  sm1(" f set2 (gradedPolySet) dc reduction /tmp set ");
  t_syz = NewArray(n);
  for (i=0; i<n; i++) {
    if (indexTable[i] != -1) {
      t_syz[i] = tmp[2, indexTable[i]];
    }else{
      t_syz[i] = Poly("0");
    }
  }
  if (Tag(vsize) != 0) {
    tt = tmp[0];
    sm1(" [vsize (integer) dc tt] toVectors /tt set ");
    tmp[0] = tt;
  }
  SetRing(r);
  return([tmp[0],tmp[1],t_syz]);
}



def IntegerToSm1Integer(f) {
  sm1(f, " (integer) dc /FunctionValue set ");
}
def RingD(vList,weightMatrix,pp) {
  local new0,tmp,size,n,i,j,newtmp,ringpp,argsize;
  argsize = Length(Arglist);
  if (IsArray(vList)) {
    sm1(" vList {toString} map from_records /vList set ");
  }
  if (argsize == 1) {
    sm1("[", vList, 
        "ring_of_differential_operators ( ) elimination_order 0 ] define_ring
         /tmp set ");
    SetRingVariables();
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
  SetRingVariables();
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

SetRingVariables_Verbose = false;
def SetRingVariables() {
  /* Don't use local variables in this function,
     because we set global variables in this function.
     cf. SSWork/yacc/memo.txt,  1997,3/6 */
  if (SetRingVariables_Verbose ) {
    Print("SetRingVariables() Setting the global variables : ");
  }
  if (k00setRingVariables(0,sm1( "[(N)] system_variable (universalNumber) dc "))) {
    sm1(" define_ring_variables ");
  }
  if (SetRingVariables_Verbose) {Ln();}
}

def k00AreThereLeftBrace(s) {
  local leftBrace, jj, slist;
  leftBrace = sm1(" $[$ (array) dc 0 get (universalNumber) dc ");
  jj = Position(StringToAsciiArray(s),leftBrace);
  if (jj != -1) return(true); else return(false);
}

def k00setRingVariables(p,q) {
  local v,i;
  for (i = p; i< q; i++) {
    v = getxvar(i);
    if (k00AreThereLeftBrace(v)) { 
       return(false);
    }
	v = getdvar(i);
    if (k00AreThereLeftBrace(v)) {
       return(false);
    }
  }
  return(true);
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

def StringToAsciiArray(s) {
  sm1(s," (array) dc { (universalNumber) dc } map /FunctionValue set ");
}

def NewArray(n) {
 return(NewVector(n));
}

def GetEnv(s) {
 sm1(" [(getenv) s] extension /FunctionValue set ");
}
def Boundp(a) {
   local b;
   sm1("[(parse) [(/) ",a," ( load tag 0 eq 
                          { /FunctionValue 0 def }
                          { /FunctionValue 1 def } ifelse )] cat ] extension");
}
def Rest(a) {
  sm1(a," rest /FunctionValue set ");
}
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

def Load_sm1(fnames,flag) {
  local ppp,n,i,cmd;
  if (Boundp(flag)) {
  }else{
    n = Length(fnames);
    for (i=0; i<n; i++) {
       ppp = GetPathName(fnames[i]);
       if (Tag(ppp) != 0) {
          sm1(" [(parse) ppp pushfile ] extension ");
          cmd = AddString(["/",flag," 1 def "]);
          sm1(" [(parse) cmd ] extension ");
          i=n; /* break; */
       }
    }
  }
}


def GetRing(f) {
  sm1(" f getRing /FunctionValue set ");
}

def SetRing(r) {
  sm1(" r ring_def ");
}

def ReParse(a) {
  local c;
  if (IsArray(a)) {
    c = Map(a,"ReParse");
  }else{
    sm1(a," toString . /c set");
  }
  return(c);
}

def void Pmat(a) {
   sm1(" a pmat ");
}

