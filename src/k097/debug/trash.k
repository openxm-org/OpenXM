  if (ShimomuraSpecial) {
    Println(" ---- S-Special -----");
    /* F-homogenization. z[0], ..., z[n-1],  
       z[n] is the homogenization variable*/
    /* z[n]^(-1) とは書けないのはつらい. 1 を戻すので bug ともいえる. */
    zinverse = PolyR(AddString([Indexed("z",n),"^(-1)"]),rz);
    rule = [[Dz[n-1], Dz[n-1]*z[n]], [z[n-1],z[n-1]*zinverse]];
    input = Replace(input,rule);
    m = Length(input);
    for (i=0; i<m; i++) {
      d = -Degree(Replace(input[i],[[z[n],zinverse]]),z[n]);
      if (d < 0) {
           input[i] = z[n]^(-d)*input[i];
      }
    }
    for (i=0; i<m; i++)  {
      /* FW principal part をとる. */
      input[i] = Init_w(input[i],[z[n]],[1]);
    }
    Print("--------FW principal parts : ");Println(input);
  }
