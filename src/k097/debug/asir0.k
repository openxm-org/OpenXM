

/* SSWork/yacc/debug/asir0.k,  1997, 3/5 cf. debug/asir1.ccc */
if (K00_verbose) {
 Println("debug/asir0.k    you need to start k0 with -f option. ");
}

/* x = Poly("x");
   f = (x+2)^10; 
Println(f); */

def Factor(f)  {
  sm1(f, " factor /FunctionValue set");
  sm1(" clean-workfiles ");
}
HelpAdd(["Factor",[" Not Yet. <<need asir, start k0 with -f option.>>"]]);

def Cancel(f)  {
  local tmp,den,num;
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
    if (Characteristic(Ringp(f)) != 0) return(f);
    if (IsConstant(f)) return(DC(f,"integer"));
    return(f);
  }
  sm1(f," cancel /FunctionValue set");
  sm1(" clean-workfiles ");
}
HelpAdd(["Cancel",[" Not Yet. <<need asir, start k0 with -f option>>"]]);



def Primadec(f,g)  {
  sm1(f,g," primadec /FunctionValue set");
  sm1(" clean-workfiles ");
}
HelpAdd(["Primadec",[" Not Yet. <<need asir, start k0 with -f option.>>"]]);



