

/* kobj2.kk,  1997, 3/30 */
/* 省略記法を用いない, ソース */
/* /PrimitiveContextp StandardContextp def
   Object = [PrimitiveContextp] 
   def new() { Return(Object); }
*/

/* local int i, ans, int j;  などという形でデータ型を指定する. */

class Complex extends Object {
  /* /Complex [(Complex) Object 0 get newcontext] def
     Complex 0 get setcontext
  */
  local re,im;
  def new(a,b) {
    this = super.new();
    /* /super Object def
       super [ ] {new} sendmsg2  /this set
       Complex this {rest} primmsg {append} primmsg
       [null null] {append} primmsg  /this set
    */       
    re = a; /* this 1 a {put} primmsg  */
    im = b; 
    return(this);
  }
  def void print() {  /* sm1 の print との関係を考えよ. */
    Print("[");
    /* ["["] {Print} sendmsg */
    Print(re); Print("]+i[");
    Print(im); Print("]");
  }
  def add(b) {
    local c;
    /* arg2 /b set  arg1 /this set */
    c = Complex.new(this.re,this.im);
    /* Complex [ this 1 {get} sendmsg this 2 {get} sendmsg ] {new} sendmsg2
       /c set */
    c.re = c.re+b.re;
    /* c 1 << c 1 get >> << b 1 get >> {add} sendmsg2 {Put} sendmsg2 */
    c.im = c.im+b.im;
    return(c);
  }
};

a = Complex.new(1,2);
/* /a Complex [1 2] {new} sendmsg2 def */
a = a+b;
/* /a a b {add} sendmsg2 def */
a.print(); Println();
/* a [ ] {print} sendmsg2  [ ] {Println} sendmsg */

    
 

