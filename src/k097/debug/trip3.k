/* trip3.k  to test new.  1997, 4/8 */
class Object extends PrimitiveObject {
  def new0() {
    return(Object);
  }
}

class Complex extends Object {
  local re,im;
  def new0(a,b) {
    this = new();  /* or this = new(super.new0()); */
    re = a;
    im = b;
    return(this);
  }
  def void show() {
    Print(re); Print(" + "); Println(im);
  }
  def void showln() {
    this.show(); Ln();
  }
    
}

class Complex2 extends Complex {
  local abs;
  def new0(a,b) {
    this = new(super.new0(a,b));
    abs = a*a+b*b;
    return(this);
  }
}

class Integer extends Object {
   local ival;
   def new0(i) {
      this = new();
      ival = i;
      return(this);
   }
   one = Integer.new0(1);
   def operator sub(b) {
     local this2,ans;
     this2 = this;
     ans = Integer.new0(0);
     ans[1] = this2[1]-b[1];
     return(ans);
   }
  def operator mul(b) {
     local this2, ans;
     this2 = this;
     ans = Integer.new0(0);
     ans[1] = this2[1]*b[1];
     return(ans);
  }
  def isEqual(b) {
    local this2;
    this2 = this;
    if (this2[1] == b[1]) return(true);
    else return(false);
  }
  def factorial() {
    local this2,tmp;
    this2 = this;
    if (this.isEqual(one)) return( one );
    tmp = this2 - one;
    return( (tmp.factorial())* this2 );
  }
  /*  100 の階乗は,
    In[12]= a=Integer.new0(100);
    In[13]=sm1(" set_timer "); a.factorial(); sm1(" set_timer ");
    In[14]=In[15]=User time: 6.850000 seconds, System time: 0.100000 seconds
  */
  def void show() {
    Print(ival);
  }
  def void showln() {
    this.show(); Ln();
  }
}


def factorial(a) {
  if (a == 1) return(1);
  return( a*factorial(a-1) );
}
/* 
In[19]=sm1(" set_timer "); factorial(100); sm1(" set_timer ");
In[20]=In[21]=User time: 0.716667 seconds, System time: 0.000000 seconds
10 倍遅い.  On SS465.
sm1 の native mode でやるとどのくらいの早さか?
*/



/* Mathematica でやると,,,,

 factorial[a_]:=Module[{},
         If[a == 1,Return[1]];
         Return[a*factorial[a-1]]]


 $RecursionLimit = Infinity

 Timing[factorial[100]]

Out[8]= {0.1 Second, 93326215443944152681699238856266700490715968264381621468\
 
>     59296389521759999322991560894146397615651828625369792082722375825118521\
 
>     0916864000000000000000000000000

0.1 秒であった.  On SS5.
 
*/