/* $OpenXM$ */
/* object.kk , 1997, 4/11. */
Protect("showln",0);

class Object extends PrimitiveObject {
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
HelpAdd(["IsObject",["IsObject(a) return true if a is an Object."]]);



/* class Gmp. Number theoretic functions.  
   1997, 4/17 
*/
class Gmp extends Object {
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
  "Ex.  r = Gmp.Gcd(5,8); ",
  "These methods call functions of Gnu-MP package.",
  "The Copyright notice is in kan96xx/gmp.",
  "Note that there is no method to create an instance."]]);

