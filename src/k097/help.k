/* $OpenXM: OpenXM/src/k097/help.k,v 1.7 2001/01/04 12:29:31 takayama Exp $ */
if (K00_verbose)
  Println("help.k:  8/6, 1996 --- 8/7, 1996. 3/6, 1997 --- 12/21, 1997.");

def help(x) { 
  if (Length(Arglist) < 1) {
     ShowKeyWords(" ");
  } else {
     Help(x); 
  }
}


def Help(key) {
  local n,i,item,m,item1,j;
  if (Length(Arglist) < 1) {
     ShowKeyWords(" ");
     return( [ ] );
  }

  if (key == "ALL") {
    ShowKeyWords("ALL"); return(0);
  }
  n = Length(Helplist);
  PSfor (i=0; i<n; i++) {
    item = Helplist[i];
    if (item[0] == key) {
       if (IsArray(item[1])) {
         item1 = item[1];
         m = Length(item1);
         for (j=0; j<m; j++) {
           Println(item1[j]);
         }
       }else{
         Println(item[1]);
       }
       return(item);
    }
  }
  Print("The key word <<"); Print(key); Println(">> could not be found.");
  return([ ]);
}


def ShowKeyWords(ss) {
  local i,j,n,keys,max,width,m,k,kk,tmp0;
  Ln();
  n = Length(Helplist);
  keys = [" " ];  /* This is a gate keeper for shell. */
  PSfor (i=0; i< n; i++ ) {
    keys = Append(keys,Helplist[i,0]);
  }
  keys = sm1(keys," shell ");
  n = Length(keys);
  if (ss == "ALL") {
    PSfor (i=1; i<n; i++) {
      Print("# "); Print(keys[i]); Ln();
      Help(keys[i]); Ln();
    }
    return(0);
  }
  max = 0;
  PSfor (i=1; i<n; i++) {
     if (Length(keys[i]) > max) {
        max = Length(keys[i]);
     }
  }
  /* Println(max); */
  max = max+3;
  width = 80;
  m = 0;
  while ((m*max) < 80) {
     m = m+1;
  }
  if (m > 1) m = m-1;
  k = 0; kk = 0;
  PSfor (i=1; i<n; i++) {
    Print(keys[i]); kk = kk+1;
    k = k+Length(keys[i]);
    tmp0 = max-Length(keys[i]);
    /*for (j=0; j < tmp0 ; j++) {
       k = k+1;
       if (kk < m) {Print(" ");}
    }*/
    k = k+tmp0;
    if (kk < m) {
        sm1(" [ 0 1 ", tmp0, " (integer) dc 1 sub { pop $ $ } for ] aload length cat_n messagen ");
    }
    if (kk >= m) {
      kk = 0; k=0; Ln();
    }
  }
  Ln();
  Println("Type in Help(keyword);  to see a help message (string keyword).");
   
  /* Println(keys); */
}

def ShowKeyWordsOfSm1(ss) {
  local i,j,n,keys,max,width,m,k,kk,tmp0;
  Ln();
  sm1(" /help_Sm1Macro @.usages def ");
  n = Length(help_Sm1Macro);
  keys = [" " ];
  for (i=0; i< n; i++ ) {
    keys = Append(keys,help_Sm1Macro[i,0]);
  }
  keys = sm1(keys," shell ");
  n = Length(keys);
  if (ss == "ALL") {
    for (i=1; i<n; i++) {
      tmp0 = keys[i];
      Print("# "); Print(tmp0); Ln();
      sm1(tmp0," usage "); Ln();
    }
    return(0);
  }
    
  max = 0;
  for (i=1; i<n; i++) {
     if (Length(keys[i]) > max) {
        max = Length(keys[i]);
     }
  }
  /* Println(max); */
  max = max+3;
  width = 80;
  m = 0;
  while ((m*max) < 80) {
     m = m+1;
  }
  k = 0; kk = 0;
  for (i=1; i<n; i++) {
    Print(keys[i]); kk = kk+1;
    k = k+Length(keys[i]);
    tmp0 = max-Length(keys[i]);
    if (kk >= m) {
    }else {
      for (j=0; j < tmp0 ; j++) {
         k = k+1;
         Print(" ");
      }
    }
    if (kk >= m) {
      kk = 0; k=0; Ln();
    }
  }
  Ln();
  Ln();
  Println("Type in (keyword) usage ;  to see a help message.");
   
  /* Println(keys); */
}

HelpAdd(["Help", "Help(key) shows an explanation on the key (string key)."]);
HelpAdd(["HelpAdd", 
 ["HelpAdd([key,explanation]) (string key, string explanation)",
  " or (string key, array explanation)."]]);
HelpAdd(["load", 
 ["load(fname) loads the file << fname >>(string fname).",
  "load fname  loads the file << fname >>.",
  "load[fname] loads the file << fname >> with the preprocessing by /lib/cpp."
]]);
HelpAdd(["Ln","Ln() newline."]);
HelpAdd(["Println","Println(f) prints f and goes to the new line."]);
HelpAdd(["Print","Print(f) prints f."]);
HelpAdd(["Poly",
 "Poly(name) returns the polynomial name in the current ring 
  (string name)."]);
HelpAdd(["PolyR",
 ["PolyR(name,r) returns the polynomial name in the ring r 
 (string name, ring r).",
 "Ex. r = RingD(\"x,y\"); y = PolyR(\"y\",r); "]]);
HelpAdd(["RingD",
 ["RingD(names) defines a new ring (string names).", 
  "RingD(names,weight_vector) defines a new ring with the weight vector",
  "(string names, array weight_vector).", 
  "RingD(names,weight_vector,characteristic)",
  " Ex. RingD(\"x,y\",[[\"x\",2,\"y\",1]]) "]]);
HelpAdd(["Reduction","Reduction(f,G) returns the remainder and sygygies when
f is devided by G (polynomial f, array G)."]);
HelpAdd(["AddString","AddString(list) returns the concatnated string (array list)."]);
HelpAdd(["AsciiToString","AsciiToString(ascii_code) returns the string of which
ascii code is ascii_code (integer ascii_code)."]);
HelpAdd(["ToString","ToString(obj) transforms the <<obj>> to a string."]);
HelpAdd(["Numerator","Numerator(f) returns the numerator of <<f>> (rational f)."]);
HelpAdd(["Denominator","Denominator(f) returns the denominator of <<f>> (rational f)."]);
HelpAdd(["Replace",
  ["Replace(f,rule) (polynomial f, array rule).  ",
   "Ex. Replace( (x+y)^3, [[x,Poly(\"1\")]])"]]);
HelpAdd(["SetRingVariables",
 "SetRingVariables()
  Set the generators of the current ring as global variables.
  cf. RingD(), Poly(), PolyR()"]);
HelpAdd(["Append","Append([f1,...,fn],g) returns the list [f1,...,fn,g]"]);
HelpAdd(["Join",
 "Join([f1,...,fn],[g1,...,gm]) returns the list
  [f1,...,fn,g1,...,gm]"]);

HelpAdd(["!ReservedNames",
 ["The names k00*, K00*, sm1* , arg1,arg2,arg3,arg4,....," ,
  "Helplist, Arglist, FunctionValue,",
  "@@@*, db.*, k.*, tmp002*, tmp00* are used for system functions."]]);

HelpAdd(["IntegerToSm1Integer",
 "IntegerToSm1Integer(i) translates integer i
  to sm1.integer (integer i)."]);
HelpAdd(["true","true returns sm1.integer 1."]);
HelpAdd(["false","false returns sm1.integer 0."]);
HelpAdd(["IsArray",
 ["If f is the array object, then IsArray(f) returns true,",
  "else IsArray(f) returns false."]]);



HelpAdd(["Init_w",
 ["Init_w(f,vars,w) returns the initial terms with respect to the",
  "weight vector <<w>> (array of integer) of the polynomial <<f>>",
  "(polynomial).  Here, <<f>> is regarded as a polynomial with respect",
  "to the variables <<vars>> (array of polynomials).",
  "Example: Init_w(x^2+y^2+x,[x,y],[1,1]):"]]);


HelpAdd(["Groebner",
 ["Groebner(input) returns Groebner basis of the left module (or ideal)",
  "defined by <<input>> (array of polynomials)",
  "The order is that of the ring to which each element of <<input>>",
  "belongs.",
  "The input is automatically homogenized.",
  "Example: RingD(\"x,y\",[[\"x\", 10, \"y\", 1]]);",
  "         Groebner([Poly(\" x^2+y^2-4\"),Poly(\" x*y-1 \")]):",
  "cf. RingD, Homogenize"]]);


HelpAdd(["RingPoly",
 ["RingPoly(names) defines a Ring of Polyomials (string names).",
  "The names of variables of that ring are <<names>>  and ",
  "the homogenization variable h.",
  "cf. SetRingVariables, RingD",
  "Example: R=RingPoly(\"x,y\");",
  "  ",
  "RingPoly(names,weight_vector) defines a Ring of Polynomials",
  "with the order defined by the << weight_vector >>",
  "(string names, array of array weight_vector).",
  "RingPoly(names,weight_vector,characteristic)",
  "Example: R=RingPoly(\"x,y\",[[\"x\",10,\"y\",1]]);",
  "         (x+y)^10: "]]);


HelpAdd(["CancelNumber",
["CancelNumber(rn) reduces the rational number <<rn>>",
 "(rational rn).",
 "Example: CancelNumber( 2/6 ) : "]]);

HelpAdd(["IsString",
["IsString(obj) returns true if << obj >> is a string (object obj).",
 "Example:  if (IsString(\"abc\")) Println(\"Hello\"); ;"]]);

HelpAdd(["IsRing",
["IsRing(obj) returns true if << obj >> is a ring (object obj)."
]]);


HelpAdd(["IsSm1Integer",
["IsSm1Integer(obj) returns true if << obj >> is an integer of sm1(object obj)."]]);

HelpAdd(["sm1",
["sm1(arg1,arg2,...) is used to embed sm1 native code in the kxx program.",
 "Example: sm1( 2, 2, \" add print \"); ",
 "Example: def myadd(a,b) { sm1(a,b,\" add /FunctionValue set \"); }" ]]);

HelpAdd(["DC",
["DC(obj,key) converts << obj >> to a new object in the primitive",
 "class << key >> (object obj, string key)",
 "Example:  DC(\" (x+1)^10 \", \"polynomial\"): "]]);

HelpAdd(["Length",
["Length(vec) returns the length of the array << vec >>",
 "(array vec)"]]);

HelpAdd(["Transpose",
["Transpose(m) return the transpose of the matrix << m >>",
 "(array of array m)."]]);

HelpAdd(["Save",
["Save(obj) appends << obj >> to the file sm1out.txt (object obj)."]]);

HelpAdd(["Coefficients",
["Coefficients(f,v) returns [exponents, coefficients] of << f >>",
 "with respect to the variable << v >>",
 "(polynomial f,v).",
 "Example: Coefficients(Poly(\"(x+1)^2\"),Poly(\"x\")): "]]);

HelpAdd(["System",
["System(comm) executes the unix system command << comm >>",
 "(string comm)",
 "Example: System(\"ls\");"]]);

HelpAdd(["Exponent",
 ["Expoent(f,vars) returns the vector of exponents of the polynomial f",
  "Ex. Exponent( x^2*y-1,[x,y])"]]);

HelpAdd(["Protect",
 ["Protect(name) protects the symbol <<name>> (string)",
  "Protect(name,level) protects the symbol <<name>> (string) with ",
  "<<level>> "]]);

HelpAdd(["IsPolynomial",
 ["IsPolynomial(f) returns true if <<f>> (object) is a polynomial."]]);



/* -----------------------------------------------
   functions on tests.    */
/* ------------  Developping functions  --------------------- */

def RingPoly(vList,weightMatrix,pp) {
  local new0,tmp,size,n,i,j,newtmp,ringpp,argsize;
  argsize = Length(Arglist);
  if (argsize == 1) {
    sm1("[", vList, 
        "ring_of_polynomials ( ) elimination_order 0 ] define_ring
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
  for (i=0; i<size; i++) {
    tmp = weightMatrix[i];
    n = Length(tmp);
    newtmp = NewVector(n);
    for (j=1; j<n; j = j+2) {
       newtmp[j-1] = tmp[j-1];
       newtmp[j] = IntegerToSm1Integer( tmp[j] );
    }
    new0[i] = newtmp;
  }
  SetRingVariables();
  ringpp =
  sm1("[", vList, 
      "ring_of_polynomials ", new0, " weight_vector", pp, " ] define_ring");
  sm1(" @@@.indexMode.flag.save @@@.indexMode ");
  return( ringpp );
}

def IsString(ob) {
  sm1(ob , " isString /FunctionValue set ");
}

def IsSm1Integer(ob) {
  sm1(ob , " isInteger /FunctionValue set ");
}

def IsRing(ob) {
  sm1(ob , " isRing /FunctionValue set ");
}


def CancelNumber(rn) {
  local tmp;
  sm1(" [(cancel) ",rn," ] mpzext /tmp set ");
  if (IsInteger(tmp)) return(tmp);
  sm1(" tmp (denominator) dc (1).. eq { /FunctionValue tmp (numerator) dc def} { /FunctionValue tmp def } ifelse ");
}

def DC_polynomial(obj) {
  return(DC(obj,"polynomial"));
}
def DC(obj,key) {
  if (IsArray(obj) && key=="polynomial") {
    return(Map(obj,"DC_polynomial"));
  }
  if (key == "string") { return(ToString(obj)); }
  else if (key == "integer") { key = "universalNumber"; }
  else if (key == "sm1integer") { key = "integer"; }
  else if (key == "polynomial") { key = "poly"; }
  else ;
  sm1( obj , key, " data_conversion /FunctionValue set ");
}

def Transpose(m) {
  sm1(m, " transpose /FunctionValue set ");
}

def Save(obj) {
  sm1(obj, " output ");
}


def void System(comm) {
  sm1(comm, " system ");
}


def IsReducible(f,g) {
  sm1("[ (isReducible) ",f,g," ] gbext /FunctionValue set ");
}

def IsPolynomial(f) {
  sm1(" f isPolynomial /FunctionValue set ");
}
sm1(" /k00.toric0.mydegree {2 1 roll degree} def ");
def Exponent(f,vars) {
  local n,i,ans;
  if (f == Poly("0")) return([ ] );
  sm1(f," /ff.tmp set ", vars , 
      " {ff.tmp k00.toric0.mydegree (universalNumber) dc }map /FunctionValue set ");
}
def void Protect(name,level) {
  local n,str;
  n = Length(Arglist);
  if (n == 1) {
    level = 1;
    str = AddString(["[(chattr) ",ToString(level)," /",name," ",
                     " ] extension pop "]);
    /* Println(str); */
    sm1(" [(parse) ",str ," ] extension pop ");
  } else if (n ==2) {
    str = AddString(["[(chattr) ",ToString(level)," /",name," ",
                     " ] extension pop "]);
    /* Println(str); */
    sm1(" [(parse) ",str ," ] extension pop ");
  } else {
     k00_error("Protect","Arguments must be one or two. ");sm1(" error ");
  }
}
    
def void k00_error(name,msg) {
  Print("Error in "); Print(name); Print(". ");
  Println(msg);
}

def Init(f) {
  if (IsArray(f)) {
     return(Map(f,"Init"));
  } else if (IsPolynomial(f)) {
     sm1(f,"  init  /FunctionValue set ");
  } else {
     k00_error("Init","Argment must be polynomial or an array of polynomials");
     sm1(" error ");
  }
}
HelpAdd(["Init",
 ["Init(f) returns the initial term of the polynomial <<f>> (polynomial)",
  "Init(list) returns the array of initial terms of the array of polynomials",
  "<< list >> (array)"]]);

HelpAdd(["NewMatrix",
 ["NewMatrix(m,n) returns the (m,n)-matrix (array) with the entries 0."]]);

def Eliminatev(list,var)  /* [(x-y). (y-z).] [(z) ] */
{
   sm1(list, var, " eliminatev /FunctionValue set ");
}
HelpAdd(["Eliminatev",
["Eliminatev(list,var) prunes polynomials in << list >>(array of polynomials)",
 "which contains the variables in << var >> ( array of strings )",
 "Example: Eliminatev([Poly(\" x+h \"),Poly(\" x \")],[ \"h\" ]): "]]);

def ReducedBase(base) {
  sm1( base, " reducedBase /FunctionValue set ");
}
HelpAdd(["ReducedBase",
 ["ReducedBase[base] prunes redundant elements in the Grobner basis <<base>> (array)."
]]);


def Ringp(f) {
  sm1(f, " (ring) dc /FunctionValue set ");
}
HelpAdd(["Ringp",
 ["Ringp(f) ( polynomial f ) returns the ring to which the polynomial << f >>",
  "belongs."]]);

def Coefficients(f,v) {
  local ans,exp;
  ans = sm1(f,v, " coefficients ");
  exp = ans[0];
  exp = sm1(exp," { (universalNumber) dc } map ");
  return([exp,ans[1]]);
}

def IsInteger(a) {
  sm1(a , " isUniversalNumber /FunctionValue set ");
}
HelpAdd(["IsInteger",
["IsInteger(a) returns true if << a >> is an integer (object a).",
 "It returns false if << a >> is not.",
 "cf. IsSm1Integer"]]);

def IsRational(a) {
  sm1(a , " isRational /FunctionValue set ");
}
HelpAdd(["IsRational",
["IsRational(a) returns true if << a >> is a rational (object a).",
 "It returns false if << a >> is not."]]);


def IsDouble(a) {
  sm1(a , " isDouble /FunctionValue set ");
}
HelpAdd(["IsDouble",
["IsDouble(a) returns true if << a >> is a double (object a).",
 "It returns false if << a >> is not."]]);


sm1(" /cs { this  [ ] Cleards  } def ");


def Init_w(f,vars,weight) {
  local w,w2,w3,ans,i,n;
  if (f == Poly("0")) return( Poly("0") );
  w = Map(vars,"ToString");
  w2 = sm1(weight," {$integer$ data_conversion} map ");
  n = Length(w);
  w3 = NewArray(n*2);
  for (i=0; i<n ; i++) {
    w3[2*i] = w[i]; w3[2*i+1] = w2[i];
  }
  ans = sm1(f,w3, " weightv init ");
  return(ans);
}

HelpAdd(["Mapto",
 ["Mapto(obj,ring) parses << obj >> as elements of the << ring >>.",
  "(ring << ring >>, polynomial << obj >> or array of polynomial << obj >>).",
  "Ex. R = RingD(\"x,y\"); SetRingVariables();",
  "    f = (x+y)^2; R2 = RingD(\"x,y,z\",[[\"y\",1]]); ",
  "    f2 = Mapto(f,R2); f2: "]]);

def Mapto(obj,ring) {
   local ans,i,n;
   if (IsArray(obj)) {
      n = Length(obj);
      ans = Map(obj,"ToString");
      for (i=0; i<n; i++) {
         ans[i] = PolyR(ans[i],ring);
      }
   }else{
      ans = ToString(obj);
      ans = PolyR(ans,ring);
   }
   return(ans);
}


HelpAdd(["ToDouble",
 ["ToDouble(f) translates << f >> into double when it is possible",
  "object << f >>.",
  "Example: ToDouble([1,1/2,[5]]): "]]);
def k00_toDouble(f) {   return(DC(f,"double")); }
def ToDouble(f) {
  if (IsArray(f)) return(Map(f,"ToDouble"));
  if (IsDouble(f)) return(f);
  return(k00_toDouble(f));
}



def Mod(f,n) {
   if (IsPolynomial(f)) {
     sm1("[(mod) ",f,n,"] gbext  /FunctionValue set ");
   } else if (IsInteger(f)) { return( Gmp.Mod(f,n) ); }
}
HelpAdd(["Mod",
 ["Mod(f,p) returns f modulo n  where << f >> (polynomial) and",
  " << p >> (integer). "]]);




def Characteristic(ringp) {
  local r,p;
  r = sm1(" [(CurrentRingp)] system_variable ");
  sm1("[(CurrentRingp) ",ringp, " ] system_variable ");
  p = sm1("[(P)] system_variable (universalNumber) dc ");
  sm1("[(CurrentRingp) ",r, " ] system_variable ");
  return(p);
}
HelpAdd(["Characteristic",
["Characteristic(ring) returns the characteristic of the << ring >>."
]]);

def IsConstant(f) {
  if (Length(f) > 1) return(false);
  sm1("[(isConstant) ", f," ] gbext /FunctionValue set ");
}
HelpAdd(["IsConstant",
["IsConstant(f) returns true if the polynomial << f >> is a constant."
]]);

Println("Default ring is Z[x,h]."); x = Poly("x"); h = Poly("h");

def Substitute(f,xx,g) {
  local tmp, coeff0,ex,i,n,newex,ans;
  if (IsInteger(f)) return(f);
  if (IsArray(f)) {
     n = Length(f);
     ans = NewVector(n);
     for (i=0; i<n; i++) {
       ans[i] = Substitute(f[i],xx,g);
     }
     return(ans);
  }
  if (! IsPolynomial(f)) {
    k00_error("Substitute","The first argument must be polynomial.");
  }
  tmp = Coefficients(f,xx);
  coeff0 = tmp[1];
  ex = tmp[0];   /* [3, 2, 0] */
  n = Length(ex);
  newex = NewVector(n);
  if (n>0) { newex[n-1] = g^ex[n-1]; }
  for (i=n-2; i>=0; i--) {
    newex[i] = newex[i+1]*(g^(ex[i]-ex[i+1]));
  }
  return(Cancel(coeff0*newex));
}
HelpAdd(["Substitute",
["Substitute(f,xx,g) replaces << xx >> in << f >> by << g >>.",
  "This function takes coeffients of << f >> with respect to << xx >>",
  "and returns the inner product of the vector of coefficients and the vector",
  "of which elements are g^(corresponding exponent).",
  "Note that it may cause an unexpected result in non-commutative rings."
]]);

def Tag(f) {
  local ans;
  if (IsArray(f)) {
    return(Map(f,"Tag"));
  }else {
    ans = sm1(f," etag (universalNumber) dc ");
    return(ans);
  }
}
HelpAdd(["Tag",
["Tag(f) returns the datatype tags of f where",
 "5: string,  9: polynomial, 15: integer(big-num), 16: rational, ",
 "18:double, 257: Error ",
 "Ex. Tag([Poly(\"0\"), 0]):"
]]);

def Error(s) {
  sm1(" s error ");
}
HelpAdd(["Error",
["Error(s) causes an error and outputs a message s."]]);

OutputPrompt ;
