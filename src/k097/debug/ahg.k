
/* SSWork/yacc/debug/ahg.k
/*             cf. debug/toric0.k */
/*  toric の generator を求める関数.
    A-hypergeometric の indicial ideal を求める関数.
   This program is buggy.
*/
ShimomuraSpecial = true ;
Vvv = false;
SetRingVariables_Verbose = false;
def void QuietKan() {
  sm1(" [(KanGBmessage) 0] system_variable ");
}

def testhg1() {
  a = [[1,1,1,1,1,1],
       [0,0,0,1,1,1],
       [0,1,0,0,1,0],
       [0,0,1,0,0,1]];
  return(idhg(a));
}

def testhg2() {
  a = [[1,1,1,1,1],
       [0,2,3,4,3],
       [0,1,1,0,2]];
  return(idhg(a));
}

def idhg(a) {
  local a,ans,rd,i,ans2,ans3,n,ff,d;
  ans = toric(a);
  if (ShimomuraSpecial) {
    if (Vvv) {Println("-------- S-special ---------");}
    ans = Map(ans,"Init");
  }
  ans = Map(ans,"ToString");
  if (Vvv) {Println(ans);}

  rd = RingDonIndexedVariables("z",Length(a[0])+1+Length(a));
  ans = Map(ans,"Poly");
  n = Length(a[0]); d = Length(a);
  ans2 = NewArray(Length(ans));
  PSfor (i=0; i< Length(ans); i++) {
    ans2[i] = ztoDz(ans[i],n);
  }
  if (Vvv) {Println(ans2);}
  ans3 = atolin(a);
  if (Vvv) {Println(ans3);}
  ff = Map(Join(ans2,ans3),"ToString");
  ans = zindicial(ff,n,d);
  return(ans);
}

def toric0_toMonom(aa,i,offset, ring)
{
  local j,ans,m;
  m = Length(aa);
  ans = PolyR("1",ring);
  for (j=0; j<m; j++) {
    ans = ans*(z[offset+j]^(aa[j,i]));
  }
  return(ans);
}


def toric(aa) {
  local i,j,rz,n,d,ideal,ans,univ,rule,nn,weight,elim;
  d = Length(aa);  n = Length(aa[0]);
  if (Vvv) {Println(aa);}
 
  weight = [ ]; elim = [ ];
  PSfor (i= n; i< n+d; i++) {
     weight = Join(weight,[Indexed("z",i), 1]);
     elim = Append(elim, Indexed("z",i));
  }
  weight = Append([weight],[Indexed("z",n-1),1]);
  if (Vvv) {Println(weight);  Println(elim);}

  rz = RingPonIndexedVariables("z",n+d, weight); 
  /* z[0], ..., z[n-1], ... , z[n+d]*/
  ideal = [ ];
  PSfor (i=0; i< n; i++) {
     ideal = Append(ideal, z[i] - toric0_toMonom(aa,i,n,rz));
  }
  if (Vvv) {
    Println(" --------- input ideal -------------");
    Print(" z["); Print( n ); Print( "] --- z["); Print( n+d-1);
    Println("] should be eliminated."); 
    Println(ideal);
  }
 
  ans = Groebner(ideal);
  if (Vvv) {Println(" -------------- gb is ----------------- "); Println(ans);}
  ans = Eliminatev(ans,elim);
  if (Vvv) {Println(" ------------ eliminated -------------- "); Println(ans);}

  rule = [[h, PolyR("1",rz) ] ];
  nn = Length(ans); univ = [ ];
  PSfor (i=0; i<nn; i++) {
    univ = Append(univ,Replace(ans[i],rule));
  }
  ans = ReducedBase(univ);
  if (Vvv) {
    Println(" ----------- removed redundant elements ----------- ");
    Println(" ---------- generators of the toric ideal are ----- ");
    Println(ans);
    Println(" ");
  }
  return(ans);
}
       
def zindicial0(input,n,m) {
  local rz,weight, ww,i,rule,zinverse,m,d,ans,elim;
  if (!ShimomuraSpecial) {
    ww = [ ]; elim = [ ];
    weight = [[Indexed("z",n),1]];
    if (Vvv) {Print("-------- weight ---------: "); Println(weight);}
    rz = RingDonIndexedVariables("z",n+1+m, weight); 
    input = Mapto(input,rz);
    if (Vvv) {Println("------------ input ------------"); Println(input);}

    /* F-homogenization. z[0], ..., z[n-1],  
       z[n] is the homogenization variable*/
    /* z[n]^(-1) とは書けないのはつらい. 1 を戻すので bug ともいえる. */
    zinverse = PolyR(AddString([Indexed("z",n),"^(-1)"]),rz);
    rule = [[Dz[n-1], Dz[n-1]*z[n]], [z[n-1],z[n-1]*zinverse]];
    input = Replace(input,rule);
    m = Length(input);
    PSfor (i=0; i<m; i++) {
      d = -Degree(Replace(input[i],[[z[n],zinverse]]),z[n]);
      if (d < 0) {
           input[i] = z[n]^(-d)*input[i];
      }
    }
    if (Vvv) {Print("------ input : "); Println(input);}
    ans = GroebnerTime(input);
    m = Length(ans);
    PSfor (i=0; i<m; i++)  {
      /* FW principal part をとる. */
      ans[i] = Init_w(ans[i],[z[n]],[1]);
    }
    if (Vvv) {Print("--------FW principal parts : ");Println(ans);}
    /* もう一回, GB の計算. */
    input = Map(ans,"ToString");
  }

  /* もう一回, GB の計算. */
  ww = [ ]; elim = [ ];
  /*  z[n+1], ..., z[n+m] がパラメータ変数 */
  /*  Dz[0] --- Dz[n-2], z[0] --- z[n-2] を消去する. */
  PSfor (i=0; i<n-1; i++) {
    ww = Join(ww,[Indexed("Dz",i), 1]);
    /* ww = Join(ww,[Indexed("z",i), 1]); */
    if (i != n-1) {
       elim = Append(elim,Indexed("Dz",i));
       /* elim = Append(elim,Indexed("z",i)); */
    }
  }
  weight = [[Indexed("z",n),1] , ww];
  if (Vvv) {Print("-------- weight ---------: "); Println(weight);}
  rz = RingDonIndexedVariables("z",n+1+m, weight); 
  /*  Trash : shimomura speical ??*/
  input = Mapto(input,rz);
  if (Vvv) {Print("------ input : "); Println(input);}
  ans = GroebnerTime(input);
  m = Length(ans);
  PSfor (i=0; i<m; i++)  {
    /* FW principal part をとる. */
    ans[i] = Init_w(ans[i],[z[n]],[1]);
  }
  if (Vvv) {Print("--------FW principal parts : ");Println(ans);}



  ans = Eliminatev(ans,elim);
  m = Length(ans);
  /* h,  z[n] を 1 にする. */
  for (i=0; i<m; i++) {
    ans[i] = Replace(ans[i],[[h,PolyR("1",rz)],[z[n],PolyR("1",rz)]]);
  }
  if (Vvv) {Println(" "); Println(" ");}
  return(ans);
}

def zrho(f,n) {
  local ans,i,top,w,rz;
  ans = 0;
  rz = Ringp(f);
  while(true) {
    if ( f == Poly("0")) sm1(" exit ");
    top = Init(f);
    f = f-top;
    w = Exponent(top,[Dz[n-1]]);
    top = Replace(top,[[Dz[n-1],PolyR("1",rz)]])*zipoch(z[n],w[0]);
    ans = ans + top;
  }
  return(ans);
}
 
def zipoch(f,w) {
  local ans,i;
  ans = 1;
  PSfor  (i=0; i<w; i++) {
    ans = ans*(f-i);
  }
  return(ans);
}




def zindicial(fff,n,mm) {
  local ans,n,i,m,r,tmp;
  ans = zindicial0(fff,n,mm);
  if (Vvv) {Println(ans);}
  m = Length(ans);
  r = [ ];
  if (Vvv) {
    Println(AddString(["------ The generic indicial polynomial  along z[",
                       ToString(n-1),
                       "] = 0 is the minimal degree polynomial of the following",
                       "polynomials."]));
    Println(AddString(["z[",ToString(n),"] is equal to s."]));
  }
  PSfor (i=0; i<m; i++) {
     tmp = ans[i];
     tmp = Replace(tmp,[[z[n-1],Poly("1")]]);
     tmp = zrho(tmp,n);
     if (Vvv) {Print(i); Print(" :  ");Println(tmp);}
     r = Append(r,tmp);
  }
  if (Vvv) {Println(" ");}
  return(r);
}

def ztoDz(f,n) {
  local rule,i;
  rule = NewArray(n);
  PSfor(i=0; i<n; i++) {
    rule[i] = [z[i],Dz[i]];
  }
  return(Replace(f,rule));
}

/* 行列より A-HG の線形方程式をだす. */
def atolin(a) {
  local d,n,eqs,ans,i,j;
  d = Length(a);
  n = Length(a[0]);
  eqs = NewArray(d);
  PSfor (i=0; i<d; i++) {
    ans = 0;
    PSfor (j=0; j<n; j++) {
      ans = ans + a[i,j]*z[j]*Dz[j];
    }
    ans = ans - z[n+1+i];
    eqs[i] = ans;
  }
  return(eqs);
}




