

/* test of class definition.  1997, 3/31, 4/1 */
/* a = Complex.new0(1,1);
   b = a+a;
   b.show();
   b.foo();
*/
class Object extends PrimitiveObject {
   ;
}

def void show() {
    local this2;
    this2 = this;
    Print(this2);
}

sm1(" /ectag { dup isClass not { pop -1 } { lc } ifelse } def ");
def k00ecTag(a) { sm1(a," ectag /FunctionValue set "); }
def IsObject() {
  local this2;
  this2 = this;
  if (!IsArray(this2)) return(false);
  if (Length(this2) < 1) return(false);
  this2 = this2[0];
  if (k00ecTag(this2) == k00ecTag(Object[0])) return(true);
  else return(false);
}

/* ------------------------------------- */ 
class Complex extends Object {
  local re,im;
  def new0(a,b) {
     local this2;
     this2 = this;
     return([this2[0],a,b]);
  }
  def void show() {
     local this2;
     this2 = this;
     Print("["); Print(this2[1]); Print("]+i["); Print(this2[2]);
     Print("]");
  }
  def void foo() {
     show();  
     super.show();
  }
  def operator add(b) {
     local this2,ans;
     this2 = this;
     ans = Complex.new0(0,0);
     if (!this2.IsObject()) {
        Println("The first argument must be in Complex.");
        sm1(" error ");
     }
     if (!b.IsObject()) {
        Println("The second argument must be in Complex.");
        sm1(" error ");
     }
     ans[1] = this2[1]+b[1];
     ans[2] = this2[2]+b[2];
     return(ans);
  }
  def operator mul(b) {
     local x1,x2,x3,x4,this2,ans;
     this2 = this;
     ans = Complex.new0(0,0);
     x1 = this2[1]; x2 = this2[2];
     if (isComplex(b)) {
       y1 = b[1]; y2 = b[2];
       ans[1] = x1*y1-x2*y2;
       ans[2] = x1*y2+x2*y1;
       return(ans);
     }
     ans[1] = x1*b;
     ans[2] = x2*b;
     return(ans);
  }
}

/* これがメンバ関数ではこまる. */
def isComplex(this2) {
   if (! this2.IsObject()) return(false);
   if (this2[0] == Complex[0]) return(true);
   else return(false);
}

class Integer extends Object {
   local ii;
   def new0(i) {
      local this2;
      this2 = this;
      return([this2[0],i]);
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
}








