/* hol.k,  1998, 12/14 */
/* packages for holonomic systems. */
sm1("[(chattr) 0 /npower ] extension ");
sm1(" [(parse) (cohom.sm1) pushfile] extension ");
sm1("[(chattr) 1 /npower] extension ");
sm1(" [(parse) (oxasir.sm1) pushfile] extension ");
/* class Hol extends Object { };
*/
   def Gkz(a,b) {
     local a1,b1;
     sm1(a," { { (integer) dc } map } map /a1 set ", b,
           " {(integer) dc} map  /b1 set [a1 b1] gkz /FunctionValue set ");
   }
   def Rrank(a) {
     sm1(a," rrank /FunctionValue set ");
   }
   def DeRham(a,b) {
     local a1,b1;
     a1 = a; b1 = b;
     if (IsArray(b1)) {
        b1 = Map(b1,"ToString");
        b1 = sm1(b1, " from_records ");
     }
     if (IsPolynomial(b1)) {
        b1 = ToString(b1);
     }
     if (IsPolynomial(a1)) {
        a1 = ToString(a1);
     }
     sm1("[ ", a1, b1, " ] deRham {(universalNumber) dc } map /FunctionValue set ");
   }
   def Annfs(a,b) {
     local a1,b1;
     a1 = a; b1 = b;
     if (IsArray(b1)) {
        b1 = Map(b1,"ToString");
        b1 = sm1(b1, " from_records ");
     }
     if (IsPolynomial(b1)) {
        b1 = ToString(b1);
     }
     if (IsPolynomial(a1)) {
        a1 = ToString(a1);
     }
     sm1("[ ", a1, b1, " ] annfs /FunctionValue set ");
   }
   def Fctr(a) {
     sm1(a," fctr /FunctionValue set ");
   }



HelpAdd(["Hol.",
 ["Annfs:  RingD(\"x,y\"); x=Poly(\"x\"); y=Poly(\"y\"); Annfs(x^3-y^2,[x,y]):   ",
  "Annfs:  Annfs(\"x^3-y^2\",\"x,y\"):   ",
  "Rrank:  Rrank(Gkz([[1,1,1,1],[0,1,3,4]],[1,2])):   ",
  "DeRham:  RingD(\"x,y\"); x=Poly(\"x\"); y=Poly(\"y\"); DeRham(x^3-y^2,[x,y]):   ",
  "Fctr:  Fctr(x^10-1):   ",
  "Gkz       "
]]);

OutputPrompt ;

