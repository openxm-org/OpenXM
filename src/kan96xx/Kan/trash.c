struct object gradedPolySetToGradedArray(struct gradedPolySet *gps,
					 int keepRedundant);
struct gradedPolySet *gradedArrayToGradedPolySet(struct object ob);

struct object gradedPolySetToGradedArray(gps,keepRedundant)
struct gradedPolySet *gps;
int keepRedundant;
{
  struct object ob,vec;
  int i;
  if (gps == (struct gradedPolySet *)NULL) return(NullObject);
  ob = newObjectArray(gps->maxGrade +1);
  vec = newObjectArray(gps->maxGrade);
  for (i=0; i<gps->maxGrade; i++) {
    putoa(vec,i,KpoInteger(i));
    putoa(ob,i+1,polySetToArray(gps->polys[i],keepRedundant));
  }
  putoa(ob,0,vec);
  return(ob);
}

struct gradedPolySet *gradedArrayToGradedPolySet(ob)
struct object ob;
{
  int n,max,i,j;
  struct object vec,gvec;
  struct gradedPolySet *grG;
  POLY f;
  int grd,ind;
  if (ob.tag != Sarray) errorKan1("%s\n","gradedArrayToGradedPolySet(): the argument must be an array.");
  n = getoaSize(ob);
  if (n<2) return(arrayToGradedPolySet(ob));
  if (getoa(ob,0).tag != Sarray) return(arrayToGradedPolySet(ob));
  if (n-1 != getoaSize(getoa(ob,0)))
    errorKan1("%s\n","gradedArrayToGradedPolySet(): the sizes of arrays mismatch.");
  max = 0; n--;
  gvec = getoa(ob,0); /* grade vector */
  for (i=0; i<n; i++) {
    if (getoa(gvec,i).tag != Sinteger)
      errorKan1("%s\n","gradedArrayToGradedPolySet(): the first argument must be a vector of grades.");
    if (KopInteger(getoa(gvec,i)) > max) max = KopInteger(getoa(gvec,i));
  }
  grG = newGradedPolySet(max+1);
  j = 0;
  for (i=0; i<grG->lim;i++) {
    if (i == KopInteger(getoa(gvec,i))) {
      j++;
      if (getoa(ob,j).tag != Sarray) errorKan1("%s\n","gradedArrayToGradedPolySet(): the argument must be an array of polynomials.");
      grG->polys[i] = newPolySet(getoaSize(getoa(ob,j)));
    }else {
      grG->polys[i] = newPolySet(0);
    }
  }
  for (i=0; i<n; i++) {
    vec = getoa(ob,i+1);
    for (j=0; j<getoaSize(vec);j++) {
      if (getoa(vec,j).tag != Spoly) {
	errorKan1("%s(%d,%d)\n","gradedArrayToGradedPolySet(): the element must be polynomial. (i,j)=");
      }
      f = KopPOLY(getoa(vec,j));
      whereInG(grG,f,&grd,&ind);
      grG = putPolyInG(grG,f,grd,ind,(struct syz0 *)NULL,1);
    }
  }
  return(grG);
}


  static int tmpOrder[] = 
/*
   0    1    2    3    4    5    6    7    8    9   10   11
  x5   x4   x3   x2   x1   x0   D5   D4   D3   D2   D1   D0
*/
  {
   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   0,
   0,   0,   0,   0,   0,  -1,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,  -1,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,  -1,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,  -1,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,  -1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  -1,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,  -1,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,  -1,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,  -1,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1};

							       
  /* tmp code */
  TransX[0] = "x0"; TransX[1] = "x1"; TransX[2] = "x2";
  TransX[3] = "x3"; TransX[4] = "x4"; TransX[5] = "t";

  TransD[0] = "h"; TransD[1] = "D1"; TransD[2] = "D2";
  TransD[3] = "D3"; TransD[4] = "D4"; TransD[5] = "T";

  CurrentRingp = (struct ring *)GC_malloc(sizeof(struct ring));
  CurrentRingp->p = P;
  CurrentRingp->n = N; CurrentRingp->m = M; CurrentRingp->l = L; CurrentRingp->c = C;
  CurrentRingp->nn = NN; CurrentRingp->mm = MM; CurrentRingp->ll = LL;
  CurrentRingp->cc = CC;
  CurrentRingp->x = TransX;
  CurrentRingp->D = TransD;
  CurrentRingp->order = tmpOrder;
  CurrentRingp->next = (struct ring *)NULL;
  Order = tmpOrder;
