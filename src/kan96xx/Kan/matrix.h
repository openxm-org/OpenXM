/* $OpenXM$ */
/* 1992/09/21,
   The size of the matrix must be (m,n) and (m2,n2) respectively.
   They are older.
*/
#define ind(i,j)   ((i)*n+(j))
#define ind2(i,j)  ((i)*n2+(j))


/* 1992/11/14, they are newer.  struct arrayOfPOLY and matrixOfPOLY */
#define getArrayOfPOLY(ap,i)  (ap->array)[i]
#define getMatrixOfPOLY(mp,i,j) (mp->mat)[(i)*(mp->n)+(j)]
