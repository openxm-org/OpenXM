class Object extends PrimitiveObject {
  ;
}

class Complex extends Object {
  local re,im;
  def new0(a,b) {
    local ans;
    /* this = super.new0(); this = Join(this,NewArray(sizeOfThisClass)); */
    this = NewArray(startOfThisClass+sizeOfThisClass);
    ans = this;
    ans[0] = Complex[0];
    re = a;
    im = b;
    return(this);
  }
  def void show() {
    Print(re); Print(" + "); Println(im);
  }
    
}

class Complex2 extends Complex {
  local abs;
  ;
}

class Integer extends Object {
  local ival;
  ;
}
