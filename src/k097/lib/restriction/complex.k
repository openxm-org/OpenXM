/* $OpenXM: OpenXM/src/k097/lib/restriction/complex.k,v 1.1 2001/01/04 12:29:33 takayama Exp $ */
/* Document of this module is at k097/Doc/complex.texi */

load["lib/restriction/restriction.k"];;

def Res_solv(m,d,rng) {
  local r,rr,ans,ac;
  ac = Length(Arglist);
  r = GetRing(Poly("1")); /* Save the current ring. */
  if (ac < 3) {
    rng = null;
    rr = GetRing(m);
    if (Tag(rr) == 0) rr = GetRing(d);
    if (Tag(rr) != 0) SetRing(rr);
  }else{
    SetRing(rng);
  }
  m=DC(m,"polynomial"); d = DC(d,"polynomial");

  if (IsRing(rng)) {
    sm1(" [ m d rng] res-solv /ans set ");
  }else{
    sm1(" [ m d] res-solv /ans set ");
  }

  SetRing(r);  
  return(ans);
}

/* m : D^p ---> D^q/jj,   u m = d mod jj */
def Res_solv2(m,d,jj,rng) {
  local r,rr,ans,ac,pp,qq,kk;

  ac = Length(Arglist);
  r = GetRing(Poly("1")); /* Save the current ring. */
  if (ac < 4) {
    rng = null;
    rng = GetRing(m);
    if (Tag(rng) == 0) rng = GetRing(d);
  }

  pp = Length(m); 
  if (!IsArray(m[0])) {
     sm1(" m { [ 2 1 roll ] } map /m set ");
  }
  qq = Length(m[0]);
  if (Length(jj) > 0) {
    if (!IsArray(jj[0])) {
       sm1(" jj { [ 2 1 roll ] } map /jj set ");
    }
    if (qq != Length(jj[0])) {
      Error(" Matrix size mismatch in m and jj of Kernel2(m,jj,r).");
    }
  }
  m = Join(m,jj);

  ans = Res_solv(m,d,rng);
  /* Println(ans); */
  SetRing(r);  
  return([Firstn(ans[0],pp),ans[1]]);
}
/* Res_solv2([x,y],[x^2+y^2],[x]):*/

def Res_solv_h(m,d,rng) {
  local r,rr,ans,ac;
  ac = Length(Arglist);
  r = GetRing(Poly("1")); /* Save the current ring. */
  if (ac < 3) {
    rng = null;
    rr = GetRing(m);
    if (Tag(rr) == 0) rr = GetRing(d);
    if (Tag(rr) != 0) SetRing(rr);
  }else{
    SetRing(rng);
  }
  m=DC(m,"polynomial"); d = DC(d,"polynomial");

  if (IsRing(rng)) {
    sm1(" [ m d rng] res-solv-h /ans set ");
  }else{
    sm1(" [ m d] res-solv-h /ans set ");
  }

  SetRing(r);  
  return(ans);
}

/* m : D^p ---> D^q/jj,   u m = d mod jj */
def Res_solv2_h(m,d,jj,rng) {
  local r,rr,ans,ac,pp,qq,kk;

  ac = Length(Arglist);
  r = GetRing(Poly("1")); /* Save the current ring. */
  if (ac < 4) {
    rng = null;
    rng = GetRing(m);
    if (Tag(rng) == 0) rng = GetRing(d);
  }

  pp = Length(m); 
  if (!IsArray(m[0])) {
     sm1(" m { [ 2 1 roll ] } map /m set ");
  }
  qq = Length(m[0]);
  if (Length(jj) > 0) {
    if (!IsArray(jj[0])) {
       sm1(" jj { [ 2 1 roll ] } map /jj set ");
    }
    if (qq != Length(jj[0])) {
      Error(" Matrix size mismatch in m and jj of Kernel2(m,jj,r).");
    }
  }
  m = Join(m,jj);

  ans = Res_solv_h(m,d,rng);
  /* Println(ans); */
  SetRing(r);  
  return([Firstn(ans[0],pp),ans[1]]);
}
/* Res_solv2_h([x,y],[x^2+y^2],[x]):*/

def Getxvars() {
  local v,n,i,ans,ans2;
  sm1(" getvNames /v set ");  
  sm1(" [(NN)] system_variable (universalNumber) dc /n set ");
  ans = [];
  for (i=1; i<n; i++) {
    ans = Append(ans,v[i]);
  }
  sm1(" ans from_records /ans2 set ");
  return([ans,ans2]);
}

/* This works only for D cf. Getxvars(). */
def Intersection(i1,i2,rng) {
  local r,rr,ans,n,tt,vv,ac;
  ac = Length(Arglist);
  r = GetRing(Poly("1")); /* Save the current ring */
  if (ac < 3) {
    rr = GetRing(i1);
    if (Tag(rr) == 0) rr = GetRing(i2);
    if (Tag(rr) != 0) SetRing(rr);
  }else{
    SetRing(rng);
  }
  /* 
  i1=DC(i1,"polynomial"); i2 = DC(i2,"polynomial");
  */
  i1 = ReParse(i1); i2 = ReParse(i2);

  vv = Getxvars();
  vv = vv[1];
  if (Length(i1) == 0) {
    ans = i2;
  }else if (!IsArray(i1[0]))  {
    sm1(" [i1 i2 vv] intersection /ans set ");
  }else {
    n = Length(i1[0]);
    sm1(" i1  fromVectors  /i1 set ");
    sm1(" i2  fromVectors  /i2 set ");
    sm1(" [i1 i2 vv] intersection /tt set ");
    sm1(" [n (integer) dc tt] toVectors /ans set ");
  }

  SetRing(r);  
  return(ans);
}

def Firstn(a,n) {
  local r,i,j,ans;
  if (Length(a) == 0) {
    return([ ]);
  }
  if (!IsArray(a[0])) {
    r = NewArray(n);
    for (i=0; i<n; i++) {
      r[i] = a[i];
    }
    return(r);
  }else{
    ans = [ ];
    for (j=0; j < Length(a); j++) {
      r = NewArray(n);
      for (i=0; i<n; i++) {
        r[i] = a[j,i];
      }
      ans = Append(ans,r);
    }
    return(ans);
  }
}

/* Kernel is also defined in lib/minimal/minimal.k */
def Kernel(f,v) {
  local ans;
  /* v :  string or ring */
  if (Length(Arglist) < 2) {
    sm1(" [f] syz /ans set ");
  }else{
    sm1(" [f v] syz /ans set ");
  }
  return(ans);
}

def Kernel_h(f,v) {
  local ans;
  /* v :  string or ring */
  if (Length(Arglist) < 2) {
    sm1(" [f] syz_h /ans set ");
  }else{
    sm1(" [f v] syz_h /ans set ");
  }
  return(ans);
}

/*  Kernel of (D^p --- m ---> D^q/jj)  */
def Kernel2(m,jj,r)
{
  local crng,ac,pp,qq,kk;
  ac = Length(Arglist);
  crng = GetRing(Poly("1"));
  if (ac < 3) {
    r = GetRing(m);
  }
  pp = Length(m); 
  if (!IsArray(m[0])) {
     sm1(" m { [ 2 1 roll ] } map /m set ");
  }
  qq = Length(m[0]);
  if (Length(jj) > 0) {
    if (!IsArray(jj[0])) {
       sm1(" jj { [ 2 1 roll ] } map /jj set ");
    }
    if (qq != Length(jj[0])) {
      Error(" Matrix size mismatch in m and jj of Kernel2(m,jj,r).");
    }
  }
  m = Join(m,jj);
  kk = Kernel(m,r);
  SetRing(crng);
  return(Firstn(kk[0],pp));
}

/*  Kernel of (D^p --- m ---> D^q/jj)  */
def Kernel2_h(m,jj,r)
{
  local crng,ac,pp,qq,kk;
  ac = Length(Arglist);
  crng = GetRing(Poly("1"));
  if (ac < 3) {
    r = GetRing(m);
  }
  pp = Length(m); 
  if (!IsArray(m[0])) {
     sm1(" m { [ 2 1 roll ] } map /m set ");
  }
  qq = Length(m[0]);
  if (Length(jj) > 0) {
    if (!IsArray(jj[0])) {
       sm1(" jj { [ 2 1 roll ] } map /jj set ");
    }
    if (qq != Length(jj[0])) {
      Error(" Matrix size mismatch in m and jj of Kernel2(m,jj,r).");
    }
  }
  m = Join(m,jj);
  kk = Kernel_h(m,r);
  SetRing(crng);
  return(Firstn(kk[0],pp));
}

/* From lib/minimal/minimal.k */
def Gb(m,rng) {
  local r,rr,ans,ac;
  ac = Length(Arglist);
  r = GetRing(Poly("1")); /* Save the current ring. */
  if (ac < 2) {
    rng = null;
    rr = GetRing(m);
    if (Tag(rr) != 0) SetRing(rr);
  }else{
    rr = rng;
    SetRing(rr);
  }
  /* m=DC(m,"polynomial");  */
  m = ReParse(m);
  sm1(" [m rr] gb /ans set ");
  SetRing(r);
  return(ans);
}

def Gb_h(m,rng) {
  local r,rr,ans,ac;
  ac = Length(Arglist);
  r = GetRing(Poly("1")); /* Save the current ring. */
  if (ac < 2) {
    rng = null;
    rr = GetRing(m);
    if (Tag(rr) != 0) SetRing(rr);
  }else{
    rr = rng;
    SetRing(rr);
  }
  /* m=DC(m,"polynomial");  */
  m = ReParse(m);
  sm1(" [m rr] gb_h /ans set ");
  SetRing(r);
  return(ans);
}

def Res_shiftMatrix(m,v,rng) {
  local n,ans,r,ac,i,j,b1,b2;
  sm1(" 40 (string) dc /b1 set ");
  sm1(" 41 (string) dc /b2 set ");
  ac = Length(Arglist);
  r = GetRing(Poly("1")); /* Save the current ring. */
  if (ac < 3) {
  }else{
    SetRing(rng);
  }
  n = Length(m);
  ans = NewVector(n);
  for (i=0; i<n; i++) {
    ans[i] = NewVector(n);
    for (j=0; j<n; j++) {
      ans[i,j] = Poly("0");
    }
    ans[i,i] = Poly(AddString([
        DC(v,"string"),"^",b1,DC(m[i],"string"),b2]));
  }
  SetRing(r);
  return(ans);
}

def ChangeRing(f) {
  local r;
  r = GetRing(f);
  if (Tag(r) == 14) {
    SetRing(r);
    return(true);
  }else{
    return(false);
  }
}


def test2() {
  RingD("x,y,z");
  /* Step 1. J */
  Println(" ---------- J --------------");
  mm = [[1],
        [x*Dx],
        [y*Dy],
        [z*Dz]];
  b1 = Res_solv(mm,[Dz]);
  Println(b1);
  b2 = Res_solv(mm,[Dy]);
  Println(b2);
  /* Step 2. K */
  Println(" --------- K -------------");
  mm2 = [[Dz],
         [Dy],
        [x*Dx],
        [y*Dy],
        [z*Dz]];
   k1 = Kernel(mm2);
   Pmat(Firstn(k1[0],2));
   aa=Kernel2([[Dz],[Dy]],[x*Dx,y*Dy,z*Dz]);
   Pmat(aa);
}

def test3() {
   RingD("x,y,z");
   mm = [[-Dz],[Dy],[x*Dx],[0]];
   kk = Kernel(mm);
   Pmat(kk[0]);
   rrr= RingD("x,y,z,t",[["t",1,"x",-1,"y",-1,"z",-1,
                                "Dx",1,"Dy",1,"Dz",1]]);
   kk0 = Reparse(kk[0]);
   gg = Gb(kk0*Res_shiftMatrix([1,1,0,2],t));
   Pmat(gg[0]); Pmat(gg[1]);
   gg2= Substitute(Substitute(gg[0],t,1),h,1);
   Pmat(gg2);
   Println("----------------------------");
   mm2 = [[0,0],
          [0,0],
          [0,0],
          [-Dy,-Dz]];
   jj2 = [[x*Dx,0],
          [y*Dy,0],
          [z,0],
          [0,x*Dx],
          [0,y],
          [0,z*Dz]];
   kk2 = Kernel2(mm2,jj2);
   Pmat(kk2);
   Println("-----------------------");
   ii = Intersection(gg2,kk2);
   Pmat(ii);
   SetRing(rrr);
   ii = Reparse(ii);
   gg3 = Gb(ii*Res_shiftMatrix([1,1,0,2],t));
   Pmat(gg3[0]); 
   gg4= Substitute(Substitute(gg3[0],t,1),h,1);
   Println("---- page 20, observation 4 -----");
   Pmat(gg4);
}