/* $OpenXM: OpenXM/src/kan96xx/Kan/list.c,v 1.3 2001/05/04 01:06:24 takayama Exp $ */
/* list.c */
#include <stdio.h>
#include "datatype.h"
#include "stackm.h"
#include "extern.h"

static errorList(char *s);
static warningList(char *s);

/* The basic data structure for list is
    struct object *,
   which is used as arguments and return values.
   NullList should be expressed by (struct object *)NULL = NULLLIST ;
   The null test is isNullList(struct object *).
*/

struct object *newList(objp)
     struct object *objp;
{
  struct object *op;
  op = (struct object *)sGC_malloc(sizeof(struct object));
  if (op == (struct object *)NULL) errorList("no more memory.");
  op->tag = Slist;
  op->lc.op = (struct object *)sGC_malloc(sizeof(struct object));
  if (op->lc.op == (struct object *)NULL) errorList("no more memory.");
  /* Warning!! Make a copy of the object. It is important. */
  *(op->lc.op) = *(objp);
  op->rc.op = (struct object *)NULL;
  return(op);
}

int klength(objp)
     struct object *objp;
{
  if (objp->tag != Slist) {
    warningList("use klength() for object-list.");
    return(-1);
  }
  if (isNullList(objp->rc.op)) return(1);
  else {
    return(klength(objp->rc.op) + 1);
  }
}

struct object listToArray(objp)
     struct object *objp;
     /* This function copies only the top level of the list */
{
  int n;
  struct object ans;
  int i;
  if (objp->tag != Slist) {
    warningList("use objectListToObjectArray() for object-list.");
    return(NullObject);
  }

  n = klength(objp);
  
  ans = newObjectArray(n);
  for (i=0; i<n; i++) {
    putoa(ans,i,*(objp->lc.op));  /* Warning!! we do not make a copy. */
    objp = objp->rc.op;
  }
  return(ans);
}

struct object *arrayToList(obj)
     struct object obj;
     /* obj.tag must be Sarray */
{
  struct object *op;
  struct object *list;
  struct object elem;
  int i,n;
  op = NULLLIST;
  n = getoaSize(obj);
  for (i=0; i<n; i++) {
    /*printf("\n%d: ",i); printObjectList(op);*/
    elem = getoa(obj,i);
    list = newList(&elem);
    op = vJoin(op,list);
  }
  return(op);
}

struct object *vJoin(list1,list2)
     struct object *list1,*list2;
     /* Join[(a b), (c d)] ---> (a b c d) */
     /* Join [(),(a b)] ---> (a b) */
     /* We do not copy. NullList is express by (struct object *)NULL.
        cf. isNullList()
     */
{
  /*
    printf("list1=");  printObjectList(list1);
    printf("\nlist2=");  printObjectList(list2);  printf("\n");
  */

  if (isNullList(list1)) return(list2);
  if (isNullList(list2)) return(list1);
  if (list1->tag != Slist || list2->tag != Slist) {
    warningList("use vJoin() for object-list.");
    return((struct object *)NULL);
  }
  if (list1->rc.op == (struct object *)NULL) {
    list1->rc.op = list2;
    return(list1);
  }else{
    vJoin(list1->rc.op,list2);
    return(list1);
  }
}

struct object car(list)
     struct object *list;
{
  if (list->tag != Slist) {
    warningList("car() is called for a non-list object.");
    return(NullObject);
  }
  if (isNullList(list)) return(NullObject);
  /* We do not copy the object */
  return(*(list->lc.op));
}

struct object *cdr(list)
     struct object *list;
{
  if (list->tag != Slist) {
    warningList("cdr() is called for a non-list object.");
    return((struct object *)NULL);
  }
  /* We do not copy the object */
  return( list->rc.op );
}


void printObjectList(op)
     struct object *op;
{
  if (op == NULL) return;
  if (isNullList(op)) return;
  if (op->tag == Slist) {
    printObjectList(op->lc.op);
    printf(", ");
    printObjectList(op->rc.op);
  }else {
    printObject(*op,0,stdout);
  }
}

memberQ(list1,obj2)
     struct object *list1;
     struct object obj2;
     /* If obj2 is an member of list1, the functions the position.
   memberQ( (1 (2 3) 4), 4) ----> 3.
*/
{
  int n,pos;
  if (isNullList(list1)) return(0);
  n = klength(list1);
  for (pos=1; pos<=n; pos++) {
    if (KooEqualQ(car(list1),obj2)) return(pos);
    else list1 = cdr(list1);
  }
  return(0);
}

static errorList(str)
     char *str;
{
  fprintf(stderr,"list.c: %s\n",str);
  exit(10);
}

static warningList(str)
     char *str;
{
  fprintf(stderr,"Warning. list.c: %s\n",str);
}

struct object KvJoin(struct object listo1,struct object listo2) {
  struct object rob;
  struct object *op1,*op2;
  if (listo1.tag == Snull) return listo2;
  if (listo2.tag == Snull) return listo1;
  if ((listo1.tag == Slist) && (listo2.tag == Slist)) {
    op1 = (struct object *)sGC_malloc(sizeof(struct object));
    op2 = (struct object *)sGC_malloc(sizeof(struct object));
    if ((op1 == NULL) || (op2 == NULL)) errorKan1("%s\n","KvJoin, No more memory.");
    *op1 = listo1; *op2 = listo2;
    rob = *(vJoin(op1,op2));
    return rob;
  }else{
    errorKan1("%s\n","KvJoin(Slist,Slist)");
  }
}
struct object Kcar(struct object listo) {
  if (listo.tag == Snull) return listo;
  if (listo.tag == Slist) {
    return car(&listo);
  }else{
    errorKan1("%s\n","Kcar(Slist)");
  }
}
struct object Kcdr(struct object listo) {
  struct object *op;
  struct object rob;
  if (listo.tag == Snull) return listo;
  if (listo.tag == Slist) {
    op = cdr(&listo);
    if (isNullList(op)) {
      rob = NullObject;
    }else{
      rob = *op;
    }
    return rob;
  }else{
    errorKan1("%s\n","Kcar(Slist)");
  }
}
struct object KlistToArray(struct object listo) {
  if (listo.tag == Snull) {
    return newObjectArray(0);
  }
  if (listo.tag == Slist) {
    return listToArray(&listo);
  }else{
    errorKan1("%s\n","KlistToArray(Slist)");
  }
}
struct object KarrayToList(struct object ob) {
  struct object *op;
  if (ob.tag != Sarray) {
    errorKan1("%s\n","KarrayToList(Sarray)");
  }
  op = arrayToList(ob);
  if (isNullList(op)) return NullObject;
  return *op;
}

/********************** test codes for Stest: ********************/
/* test of objectArrayToObjectList. in Stest: stackmachine.c 
   { 
   struct object *list;
   list = arrayToList(ob1);
   ob1 = listToArray(list);
   push(ob1);
   }
   test for memberQ().
   {
   struct object *list;
   list = objectArrayToObjectList(pop());
   printf("\nmemberQ()=%d\n",memberQ(list,ob1));
   }

*/
