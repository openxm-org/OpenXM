sm1(" [(parse) (slope.sm1) pushfile ] extension ");
load["restriction.k"];
load("complex.k");


def GKZ(a,b) {
  local v,ans,vv;
  sm1(" [a { /v set v { (integer) dc} map } map 
         b { (integer) dc } map ] gkz /ans set ");
  vv = ans[1];
  RingD(vv);
  return(ReParse(ans[0]));
}

def Slope(g,vv,f,v) {
  local ans;
  g = Map(g,"ToString");
  vv = Map(vv,"ToString");
  sm1(" f { (integer) dc } map /f set ");
  sm1(" v { (integer) dc } map /v set ");
  sm1(" [ g vv f v ] slope /ans set ");
  return(ans);
}

def Primadec(g) {
  sm1(" [g] primadec /FunctionValue set ");
}


OutputPrompt;
