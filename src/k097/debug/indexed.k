/* $OpenXM$ */
/* These were moved from slib.k and help.k */

def Indexed(name,i) {
  sm1(name,i," s.Indexed /FunctionValue set ");
}
/* Indexed2("a",2,3) ---> "a[2,3]" */
def Indexed2(name,i,j) {
  sm1(name,i,j," s.Indexed2 /FunctionValue set ");
}
HelpAdd(["Indexed",
 "Indexed(name,i) returns the string name[i]
  (string name, integer i)"]);
HelpAdd(["RingDonIndexedVariables",
 ["RingDonIndexedVariables(name,n) defines and returns the ring of",
  "homogenized differential operators",
  "Q<h, name[0], ..., name[n-1], Dname[0], ..., Dname[n-1]>",
  "where <<name>> is a string and <<n>> is an integer.",
  "Note that this function defines global variables",
  "h, name[0], ..., name[n-1], Dname[0], ..., Dname[n-1].",
  "Example: RingDonIndexedVariables(\"x\",3).",
  "RingDonIndexedVariables(name,n,w) defines and returns the ring of",
  "homogenized differential operators with the ordering defined by ",
  "the weight vector <<w>> (array)",
  "Example: RingDonIndexedVariables(\"x\",3,[[\"x[0]\",1,\"x[2]\",3]])."]]);
def IndexedVariables(name,size) {
  local result,i,result2;
  result = [ ];
  for (i=0; i<size-1; i++) {
    result = Append(result,Indexed(name,i));
    result = Append(result,",");
  }
  if (size-1 >= 0) {
    result = Append(result,Indexed(name,size-1));
  }
  result2 = Join(["{"],result);
  result2 = Join(result2,["}"]);
  return(AddString(result2));
}
HelpAdd(["IndexedVariables",
["IndexedVariables(name,size) returns the string ",
 " {name[0],name[1],...,name[size-1]} which can be used as inputs to ",
 " the function RingD  (string name, integer size).",
 " cf. RingDonIndexedVariables.",
 " Ex. R = RingD(IndexedVariables(\"a\",3)); ",
 "     h = Poly(\"h\");",
 "     a = NewArray(3);",
 "     for (i=0; i<3; i++) {a[i] = Poly(Indexed(\"a\",i));} ;"]]);


def RingDonIndexedVariables(vList, size, weightMatrix,pp) {
  local myring,tmp,k00_i,argsize,vListD;
  /* You cannot use these local varialbes as a name of global ring 
     variables. Change these names to names that start with k00_ */
  argsize = Length(Arglist);
  if (argsize == 1) {
    Println("Error (IndexedRingD): ");
    return(null);
  }
  if (argsize == 2) {
    vListD = AddString(["D",vList]);
    myring = RingD(IndexedVariables(vList,size));
    tmp = NewArray(size);
    for (k00_i=0; k00_i<size; k00_i++) {tmp[k00_i]=Poly(Indexed(vList,k00_i));}
    sm1(vList, " (literal) dc ", tmp, " def ");
    tmp = NewArray(size);
    for (k00_i=0; k00_i<size; k00_i++) {tmp[k00_i]=Poly(Indexed(vListD,k00_i));}
    sm1(vListD, " (literal) dc ", tmp, " def ");
    if (SetRingVariables_Verbose) {
      Print("Set the global variables ");
      sm1("[(parse) ",vList," ] extension pop print ");
      sm1("[(parse) ",vListD," ] extension pop print "); Ln();
    }else {
      sm1("[(parse) ",vList," ] extension pop  ");
      sm1("[(parse) ",vListD," ] extension pop "); 
    }
    return( myring );
  }
  if (argsize == 3 || argsize == 4) {
   if (argsize == 3) { pp = 0; }
   vListD = AddString(["D",vList]);
   myring = RingD(IndexedVariables(vList,size),weightMatrix,pp);
    SetRingVariables(); 
    tmp = NewArray(size);
    for (k00_i=0;k00_i<size; k00_i++) {tmp[k00_i]=Poly(Indexed(vList,k00_i));}
    sm1(vList, " (literal) dc ", tmp, " def ");
    tmp = NewArray(size);
    for (k00_i=0;k00_i<size; k00_i++) {tmp[k00_i]=Poly(Indexed(vListD,k00_i));}
    sm1(vListD, " (literal) dc ", tmp, " def ");
    if (SetRingVariables_Verbose) {
      Print("Set the global variables ");
      sm1("[(parse) ",vList," ] extension pop print ");
      sm1("[(parse) ",vListD," ] extension pop print "); Ln();
    } else {
      sm1("[(parse) ",vList," ] extension pop  ");
      sm1("[(parse) ",vListD," ] extension pop "); 
    }
    return( myring );
  }
  return(-1);
}
def RingPonIndexedVariables(vList, size, weightMatrix) {
  local myring,tmp,k00_i,argsize,vListD;
  /* You cannot use these local varialbes as a name of global ring 
     variables. Change these names to names that start with k00_ */
  argsize = Length(Arglist);
  if (argsize == 1) {
    Println("Error (RingPonIndexedVariables): ");
    return(null);
  }
  if (argsize == 2) {
    myring = RingPoly(IndexedVariables(vList,size));
    SetRingVariables();
    tmp = NewArray(size);
    for (k00_i=0; k00_i<size; k00_i++) {tmp[k00_i]=Poly(Indexed(vList,k00_i));}
    sm1(vList, " (literal) dc ", tmp, " def ");
    if (SetRingVariables_Verbose) {
     Print("Set the global variables ");
     sm1("[(parse) ",vList," ] extension pop print "); Ln();
    }else {
     sm1("[(parse) ",vList," ] extension pop  ");
    }
    return( myring );
  }
  if (argsize == 3) {
    myring = RingPoly(IndexedVariables(vList,size),weightMatrix);
    SetRingVariables(); 
    tmp = NewArray(size);
    for (k00_i=0;k00_i<size; k00_i++) {tmp[k00_i]=Poly(Indexed(vList,k00_i));}
    sm1(vList, " (literal) dc ", tmp, " def ");
    if (SetRingVariables_Verbose) {
      Print("Set the global variables ");
      sm1("[(parse) ",vList," ] extension pop print "); Ln();
    } else {
      sm1("[(parse) ",vList," ] extension pop "); 
    }
    return( myring );
  }
  return(-1);
}

HelpAdd(["RingPonIndexedVariables",
 ["RingPonIndexedVariables(name,n) defines and returns the ring of",
  "polynomials",
  "Q<h, name[0], ..., name[n-1] >",
  "where <<name>> is a string and <<n>> is an integer.",
  "Note that this function defines global variables",
  "h, name[0], ..., name[n-1].",
  "Example: RingPonIndexedVariables(\"x\",3).",
  "RingPonIndexedVariables(name,n,w) defines and returns the ring of",
  "polynomials with the ordering defined by ",
  "the weight vector <<w>> (array)",
  "Example: RingPonIndexedVariables(\"x\",3,[[\"x[0]\",1,\"x[2]\",3]])."]]);

