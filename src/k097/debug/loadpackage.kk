

/**Name:  yacc/debug/loadpackage.kk */
/**Date:  1997, 4/19 */

def loadsm1(s) {
  sm1("[(parse) ",s," pushfile ] extension ");
}

def void cppload(s) {
  local com;
  System("/bin/rm -f k00.cppload.tmp");
  com = AddString(["/lib/cpp -P -lang-c++ <",s,"  >k00.cppload.tmp"]);
  Println(com);
  System(com);
  /* load("k00.cppload.tmp"); */
  /* System("/bin/rm -f k00.cppload.tmp"); */
}


def diff0(f,x) {
  local dx,ans;
  if (!IsPolynomial(f)) return(Poly("0"));
  if (f == Poly("0")) return(f);
  dx = PolyR(AddString(["D",ToString(x)]),GetRing(f));
  ans = Replace(dx*f,[[dx,Poly("0")],[h,PolyR("1",GetRing(f))]]);
  return(ans);
}

class Oaku extends Object {
  this = Oaku;  /* この this がないと, bf.sm1 の symbols は,
                   PrimitiveContextp に登録される. */
  loadsm1("/home/nobuki/SSkan/Doc/bf.sm1");
  def bf3(ff) {
    local f,g;
    sm1(" bf3_0 ");
    f = Poly(ff);
    /*  Give the generators.  t - s f(x), Dx + s Df/Dx Dt */
    g = [ t - s*f, Dx+ s*diff0(f,x)*Dt,
                   Dy+ s*diff0(f,y)*Dt,
                   Dz+ s*diff0(f,z)*Dt];
    sm1(g," bf3_1 reducedBase /FunctionValue set ");
  }
}



