
if (K00_verbose) {
  Println(" debug/db.k (db.ccc), 1997, 3/2 (Sun) : checking debug functions of kxx");
  Println(" Type in test0(). ");
}

sm1(" /pushVariables {localVariables}  def ");
sm1(" /popVariables {restoreVariables} def ");
if (K00_verbose) {
  Println(" Overloaded on pushVariables and popVariables.");
}
sm1(" [(CatchCtrlC) 1] system_variable ");
sm1(" [(Strict) 1] system_variable ");
if (K00_verbose) {
  Println(" ctrl-C signal is caught in KSexecuteString() and <<Warning>> is regarded as an error.");
}

def test0() {
  sm1(" [(ErrorMessageMode) 2] system_variable ");
  sm1(" [(WarningMessageMode) 2] system_variable ");
  fib(15);
  sm1(" db.where.es ");  /* This line will not be executed. */
}

def void Where() {
  Println("CurrentContext is ...");
  sm1(" [(CurrentContextp)] system_variable {message} primmsg ");
  Println("VariableStack trace is....");
  sm1(" db.where ");
  Println("DebugStack trace is ....");
  sm1(" db.where.ds ");
  Println("To clear VariableStack, DebugStack and ErrorStack, type in Cleards().");
}

def void Cleards() {
  Print("Clearing DebugStack and ErrorStack...");
  sm1(" db.clear.ds db.clear.es ");
  Println(" ");
  Print("Restoring variables....");
  sm1(" db.restore  ");
  sm1(" [  ] localVariables ");  
  /* push [ ] for this function, otherwise this function exits with error.*/
  Println("Done");
}

def fib(n) {
  local ans,a,b;
  Print("fib of "); Println(n);
  if (n < 2) {return(1);}
  a = n-1; b = n-2;
  if (a == 11) {
    a[i] = 2;  /* We cause an error. */
  }
  ans = fib(a) + fib(b);
  return(ans);
}





