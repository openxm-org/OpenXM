

/*  1997, 5/25,  debug/psfor.k PSfor を新しくつくった.*/
def void foo(n) {
  local i;
  Print(n); Print(" times of for loop ");
  sm1(" set_timer ");
  for (i=0; i<n; i++) {
    
  }
  Println("  ");
  sm1(" set_timer ");
}

   
def void foo2(n) {
  local i;
  Print(n); Print(" times of PSfor loop ");
  sm1(" set_timer ");
  PSfor (i=0; i<n; i++) {
    
  }
  Println("  ");
  sm1(" set_timer ");
}

Println("psfor.k   :  foo(n) for for and foo2(n) for PSfor.");
