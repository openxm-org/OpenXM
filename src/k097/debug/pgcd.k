

/* polynomial gcd.k */

class Pgcd extends Object {
 pzero = Poly("0");
 def gcd0(f,g,xx) {
   local tmp,r;
   if (Degree(g,xx) > Degree(f,xx)) {
      tmp = f; f = g; g = tmp;
   }
   while (!IsConstant(g)) {
     r = Reduction(f,[g]);
     Println([f,g,r[1]]);
     r = r[0];
     f = g;
     g = r;
   }
   if (g == pzero) return(f);
   else return(g);
 }
 def gcd(f,g,xx) {
   local tmp,r;
   if (Degree(g,xx) > Degree(f,xx)) {
      tmp = f; f = g; g = tmp;
   }
   while (!IsConstant(g)) {
     r = pseudoRemainder(f,g,xx);
     f = g;
     g = r;
   }
   if (g == pzero) return(f);
   else return(g);
 }
 def pseudoRemainder(f,g,xx) {
   local c,r;
   c = Init(g);
   Println([c,xx]);
   c = Coefficients(c,xx);
   c = c[1,0];
   r = Reduction(c^(Degree(f,xx)-Degree(g,xx)+1)*f,[g]);
   Println([f,g,r[1]]);
   return(r[0]);
 }
}



   
   
