

/* SSWork/yacc/debug/asir0.k,  1997, 3/5 cf. debug/asir1.ccc */
if (K00_verbose) {
 Println("debug/asir0.k    you need ox_asir module. ");
}

def loadOXasir() {
Load_sm1(["oxasir.sm1",AddString([GetEnv("OpenXM_HOME"),
          "/lib/sm1/oxasir.sm1"])],
         "oxasir.sm1.loaded");
}

/* x = Poly("x");
   f = (x+2)^10; 
Println(f); */

def Factor(f)  {
  loadOXasir();
  sm1(f, " f fctr /FunctionValue set");
}
HelpAdd(["Factor",["Factor a given polynomial."]]);

def Cancel(f)  {
  local tmp,den,num,r;
  loadOXasir();
  if (IsRational(f)) {
   den = Cancel(Denominator(f));
   num = Cancel(Numerator(f));
   if (IsInteger(den) && IsInteger(num)) {
     return(CancelNumber(num/den));
   }
   if (IsInteger(den)) {
     tmp = sm1("[(divByN) ",num, den," ] gbext ");
     if (tmp[1] == Poly("0")) {
       return(Cancel(tmp[0]));
     }else{
       return(f);
     }
   }
  }
  if (IsInteger(f)) return(f);
  if (IsPolynomial(f)) {
    if (f == Poly("0")) return(0);
    if (Characteristic(GetRing(f)) != 0) return(f);
    if (IsConstant(f)) return(DC(f,"integer"));
    return(f);
  }
  sm1("[[num den]] reduceByAsir /r set ");
  if (IsInteger(r[1])) {
    if (r[1] == 1) return(r[0]);
  }
  return(r[0]/r[1]);
}
HelpAdd(["Cancel",["Cancel the greatest common multiple of the denominators and the numerator. "]]);



/* BUG: It has not yet been rewritten. 
def Primadec(f,g)  {
  sm1(f,g," primadec /FunctionValue set");
  sm1(" clean-workfiles ");
}
HelpAdd(["Primadec",[" Not Yet. <<need asir, start k0 with -f option.>>"]]);
*/


