

/* return --> goto で処理がどの程度遅い原因かを調べる.   return.k
   1997, 5/31 
*/

Println(" Type in Foo1(number) for regular return or Foo2(number) without regular return.");

def Foo1(n) {
   local i;
   sm1(" set_timer ");
   PSfor (i=0; i<n; i++) {
      foo1a();
   }
   sm1(" set_timer ");
}
def foo1a() {
   local x;
   return(10);
   x = 1;    x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;
   x = 1;    x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;
   x = 1;    x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;
   x = 1;    x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;
   x = 1;    x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;
   x = 1;    x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;
}

def Foo2(n) {
   local i;
   sm1(" set_timer ");
   PSfor (i=0; i<n; i++) {
      foo2a();
   }
   sm1(" set_timer ");
}
def foo2a() {
   local x;
   if (true) {
     x = 10;
   } else {
   x = 1;    x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;
   x = 1;    x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;
   x = 1;    x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;
   x = 1;    x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;
   x = 1;    x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;
   x = 1;    x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;   x = 1;
   }
   return(10);
}



