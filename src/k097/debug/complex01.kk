class Complex extends Object {
  local re, /* real part */
        im; /* imaginary part*/
  def new2(a,b) {
    this = new(super.new0());
    re = a;
    im = b;
    return(this);
  }
  def real() { return(re); }
  def imaginary() { return(im); }
  def operator add(b) {
    return( new2(re+b.real(), im+b.imaginary()) );
  }
  def operator sub(b) {
    return( new2(re-b.real(), im-b.imaginary()) );
  }
  def operator mul(b) {
    return(new2( re*b.real()-im*b.imaginary(), re*b.imaginary()+im*b.real()));
  }
  def operator div(b) {
    local den,num1,num2;
    den = (b.real())^2 + (b.imaginary())^2 ;
    num1 = re*b.real() + im*b.imaginary();
    num2 = -re*b.imaginary()+im*b.real();
    return(new2(num1/den, num2/den));
  }

  def void show() {
    Print(re); Print(" +I["); Print(im); Print("]");
  }
  def void showln() {
    this.show(); Ln();
  }
}
