/* graph.kk , 1997, 4/9? */
/*  see object.kk for pre-installed objects 
*/

/* cf. p.70 Java reference マニュアル */
class Circle extends Object {
  local x,  /* 中心 */
        y,
        r;  /* 半径 */
  def new0(xx,yy,rr) {
    this = new();
    x = xx; y = yy; r = rr;
    return(this);
  }
  def void show() {
    Print(AddString([
     "x=", ToString(x), " y=", ToString(y), " r=", ToString(r)]));
    Ln();
  }
  def area() {
    return( (31415/10000)*r*r );
  }
}

/* cf. p.89 Java reference マニュアル */
class GraphicCircle extends Circle {
  local outline, fill;
  def new0(xx,yy,rr) {
    this = new(super.new0(xx,yy,rr));
    outline = 1; /* default value */
    fill = 1; /* default value */
    return(this);
  }
}

