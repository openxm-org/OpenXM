
Println("SSWork/yacc/debug/int.k  1997, 3/10  cf. SSWork/yacc/memo.txt");
Println("Check the performance of loop");
/* As to timing data, please see SSWork/yacc/memo.txt */
Println("Loaded  foo1  foo2");

def void foo1() {
  local i,j,one,hundred ;
  Println("sm1 native for loop");
  sm1(" { 1 1 100 { /ii set 1 1 100 {/jj set } for } for } timer ");
  Println("k0 loop with sm1.integer.");
  one = sm1(" 1 "); hundred = sm1(" 100 ");
  sm1(" set_timer ");
  for (i=one; i< hundred ; i=i+one) {
    for (j=one; j< hundred ; j = j+one) {
      ;
    }
  }
  sm1(" set_timer ");
  /* sm1(" set_timer ");
  for (i=sm1(" 1 "); i< sm1(" 100 ") ; i=i+sm1(" 1 ")) {
    for (j=sm1(" 1 "); j<sm1(" 100 ") ; j = j+sm1(" 1 ")) {
      ;
    }
  }
  sm1(" set_timer "); */
  Println("Now, k0 loop with k0.integer (sm1.universalNumber). Please wait patiently.");
  sm1(" set_timer ");
  for (i=1; i<100; i=i+1 ) {
    for (j=1; j<100;j=j+1) {
      ;
    }
  }
  sm1(" set_timer ");
}

def void foo2() {
  local i,j,one,hundred, a;
  Println("Array Indexing.");
  a = NewVector(101);
  one = sm1(" 1 "); hundred = sm1(" 100 ");
  sm1(" set_timer ");
  for (i=one; i< hundred ; i=i+one) {
    for (j=one; j< hundred ; j = j+one) {
      a[i] = i;
      if ( i + j < sm1(" 100 ")) { a[i+j] = -1; }
    }
  }
  sm1(" set_timer ");
  Println("Now, k0 loop with k0.integer (sm1.universalNumber). Please wait patiently.");
  sm1(" set_timer ");
  for (i=1; i<100; i=i+1 ) {
    for (j=1; j<100;j=j+1) {
      a[i] = i;
      if ( i + j <  100 ) { a[i+j] = -1; }
    }
  }
  sm1(" set_timer ");
}

