/* $OpenXM: OpenXM/src/k097/object.kk,v 1.4 2001/01/08 05:26:49 takayama Exp $ */
/* object.kk , 1997, 4/11. */
Protect("showln",0);

class Object extends PrimitiveObject {
  local ;
  def new0() {
    return(Object);
  }
  def showln() {
    Println(this);
  }
  def getClass() {
    sm1(" this 0 get /FunctionValue set ");
  }
}

sm1(" /ectag { dup isClass not { pop -1 } { lc } ifelse } def ");
def k00ecTag(a) { sm1(a," ectag /FunctionValue set "); }
def IsObject(a) {
  if (!IsArray(a)) return(false);
  if (Length(a) < 1) return(false);
  if (k00ecTag(a[0]) == k00ecTag(Object[0])) return(true);
  else return(false);
}
HelpAdd(["IsObject",
 ["IsObject(<<a>>) return true if a is an Object."],
 ["a"],
 ["Tag"],
 "check if a given object is primitive or not."
]);



/* class Gmp. Number theoretic functions.  
   1997, 4/17 
*/
class Gmp extends Object {
  local ;
  def BitAnd(a,b) {
    sm1("[(and) ",a," ",b," ] mpzext /FunctionValue set ");
  }
  def BitOr(a,b) {
    sm1("[(ior) ",a," ",b," ] mpzext /FunctionValue set ");
  }
  def ModuloPower(base,ex,mmod) {
    sm1("[(powm) ",base," ",ex," ",mmod," ] mpzext /FunctionValue set ");
  }
  def ProbabilisticPrimeP(p,reps) {
    sm1("[(probab_prime_p) ",p," ",reps," ] mpzext /FunctionValue set ");
  }
  def Sqrt(a) {
    sm1("[(sqrt) ",a, " ] mpzext /FunctionValue set ");
  }
  def Gcd(a,b) {
    sm1("[(gcd) ",a," ",b," ] mpzext /FunctionValue set ");
  }
  def Div(a,b) {
    sm1("[(tdiv_qr) ",a," ",b," ] mpzext /FunctionValue set ");
  }
  def Mod(a,b) {
    sm1("[(tdiv_qr) ",a," ",b," ] mpzext 1 get /FunctionValue set ");
  }
}

HelpAdd(["Gmp.",
 ["Gmp is a class which supports the following methods:",
  "BitAnd, BitOr, ModuloPower, ProbabilisticPrimeP, Sqrt,",
  "Gcd, Div, Mod.",
  "These methods call functions of GNU MP package.",
  "Note that there is no method to create an instance.",
  "Ex.  r = Gmp.Gcd(5,8); "
  ],
  null,
  null,
  "class Gmp"
]);

class Stderr extends Object {
  local ;
  def Print(s) {
     s = ToString(s);
     if (!Boundp("Stderr_fd")) {
        sm1(" [37 (string) dc (stderr)] cat (w) file /Stderr_fd set ");
     }
     sm1(" Stderr_fd s writestring ");
  }
  def Println(s) {
     Print(s); Ln();
  }
  def Ln() {
     if (!Boundp("Stderr_fd")) {
        sm1(" [37 (string) dc (stderr)] cat (w) file /Stderr_fd set ");
     }
     sm1(" Stderr_fd 10 (string) dc writestring ");
  }
  def Flush() {
     sm1(" [(flush)] extension ");
  }
}

HelpAdd(["Stderr.",
 ["Stderr is a class which supports printing to stderr.",
  "Methods are Print, Println, Ln, Flush.",
  "Stderr_fd is a global variable to save a file descriptor."
  ],
  null,
  null,
  "class Stderr"
]);
