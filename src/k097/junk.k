def Init_w(f,vars,weight) {
  local w,top,ans,wtop;
  if (f == Poly("0")) return( Poly("0") );
  top = sm1(f, " init ");
  w = Exponent(top,vars) * weight ;
  wtop = w;
  ans = top;
  f = f-top;
  while(true) {
    if ( f == Poly("0")) sm1(" exit ");
    top = sm1(f, " init ");
    w = Exponent(top,vars) * weight ;
    if (w < wtop) sm1(" exit ");
    ans = ans + top;
    f = f-top;
    /* Println(ans); */
  }
  return(ans);
}
