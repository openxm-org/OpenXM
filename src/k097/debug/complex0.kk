/* complex0.kk , 1997, 4/17 */
/*  complex の第0版 */

class Complex extends Object {
  local re, /* 実部 */
        im; /* 虚部 */
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
    /* return( new0(re+b.real(), im+b.imaginary()) );
       このようなエラーをしても, エラーがおきない.  これは問題. */
    /* 片方が, プリミティブオブジェクトでも大丈夫なように書き換える.
       実は operator override の方法は数学の記述にはいまひとつなのでは?
       Mathematica は, 引数のデータ型を記述できるようになっている. 
       でも, ! IsObject() で分類していけばいい.
       引数を二つみて, つまり this, b の順に context を決めていくのは,
       私の idea.
    */
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
    /* den == 0 なら, isInvalidRational() がエラーをだすはず.
       このような, こと知らないと, プログラムを書けないのは良くない?
    */
  }

  def void show() {
    Print(re); Print(" +I["); Print(im); Print("]");
  }
  def void showln() {
    this.show(); Ln();
  }
}


