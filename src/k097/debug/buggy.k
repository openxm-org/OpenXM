/* $OpenXM$ */
/* This is used to check  if KCerror() works properly. */
def help(x) { 
  if (Length(Arglist) < 1) {
     ShowKeyWords(" ");
  } else {
     Help(x); 
  }
}


def Help(key) {
  local n,i,item,m,item1,j;
  if (Length(Arglist) < 1 
     ShowKeyWords(" ");
     return( [ ] );
  }

  if (key == "ALL") {
    ShowKeyWords("ALL"); return(0);
  }
  n = Length(Helplist);
  PSfor (i=0; i<n; i++) {
    item = Helplist[i,1];
    if (item[0] == key) {
       if (IsArray(item[1])) {
         item1 = item[1];
         m = Length(item1);
         for (j=0; j<m; j++) {
           Println(item1[j]);
         }
       }else{
         Println(item[1]);
       }
       return(item);
    }
  }
  Print("The key word <<"); Print(key); Println(">> could not be found.");
  return([ ]);
}

load("help.k");
if (1==2  {
  Println("Hello");
}else {
  Println("bye");
}

for (i=0) {
  Println("Hello");
}
