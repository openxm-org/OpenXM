struct object test2(ob)  /* test2 */
struct object ob;
{
  struct object rob;
  int k;
  static POLY f0;
  static POLY f1;
  
  POLY addNode;
  POLY f;
  int i;
  static int s=0;
  extern struct ring *SmallRingp;
  extern struct ring *CurrentRingp;
  addNode = pMalloc(SmallRingp);
  k = ob.lc.ival;
  switch(s) {
  case 0:
    f0 = addNode;
    for (i=0; i<=k; i++) {
      f0->next = cxx(iiComb(k,i,CurrentRingp->p),0,i,CurrentRingp);
      f0 = f0->next;
    }
    f0 = addNode->next;
    s++;
    rob.lc.poly = f0;
    break;
  case 1:
    f1 = addNode;
    for (i=0; i<=k; i++) {
      f1->next = bxx(BiiComb(k,i),0,i,SmallRingp);
      f1 = f1->next;
    }
    f1 = addNode->next;
    s = 0;
    rob.lc.poly = f1;
    break;
  default:
    rob.lc.poly = POLYNULL;
    s = 0;
    break;
  }

    
  rob.tag = Spoly;
  return(rob);
}

struct object test1(ob)
struct object ob;
{
  struct object rob;
  int k;
  static POLY f0;
  static POLY f1;
  /* It will dump the core 
  POLY f0; or = 1;
  POLY f1;
  */
  
  POLY addNode;
  POLY f;
  int i;
  static int s=0;
  extern struct ring *SmallRingp;
  extern struct ring *CurrentRingp;
  addNode = pMalloc(SmallRingp);
  k = ob.lc.ival;
  switch(s) {
  case 0:
    f0 = addNode;
    for (i=0; i<k; i++) {
      f0->next = cxx(i+1,0,i,CurrentRingp);
      f0 = f0->next;
    }
    f0 = addNode->next;
    s++;
    rob.lc.poly = f0;
    break;
  case 1:
    f1 = addNode;
    for (i=k-1; i>=0; i--) {
      f1->next = cxx(-i-1,0,-i,CurrentRingp);
      f1 = f1->next;
    }
    f1 = addNode->next;
    s++;
    rob.lc.poly = f1;
    break;
  default:
    f = addNode;
    f->next = cxx(1,0,1,SmallRingp);
    f = f->next;
    f->coeffp->tag = POLY_COEFF;
    f->coeffp->val.f = f0;
    f->next = cxx(1,0,2,SmallRingp);
    f = f->next;
    f->coeffp->tag = POLY_COEFF;
    f->coeffp->val.f = f1;
    rob.lc.poly = addNode->next;
    s = 0;
    break;
  }

    
  rob.tag = Spoly;
  return(rob);
}

