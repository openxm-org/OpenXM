#include <stdio.h>
#include "pvm3.h"
#define SLAVENAME "slave3"

#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "extern2.h"
/* #include "lookup.h" */
#include "matrix.h"
#include "gradedset.h"
#include "sm1pvm.h"

static int KpvmVerbose = 0;   /* 1 is for outputting debug messages.  */

/*    KSstart();    */
int KpvmStartSlaves(char *name,int n);
int KpvmStopSlaves(void);
int KpvmChangeStateOfSlaves(int k);
int KpvmMcast(char *comm);
struct object KpvmJobPool(struct object obj);

#define MAXHOSTS 32
static int PvmStarted = 0;
static int Mytid;                  /* my task id */
static int Tids[MAXHOSTS+1];         /* slave ids */
static int Nproc;
static  struct pvmhostinfo *Hostp[MAXHOSTS+1];

KpvmStartSlaves(char *name,int nproc) {
  int numt,i,info;
    /* enroll in pvm */
    Nproc = nproc;
    Mytid = pvm_mytid();
    if (Nproc > MAXHOSTS) {
      Nproc = MAXHOSTS-1;
      fprintf(stderr,"Too many tasks. It is set to %d\n",Nproc);
    }
    /* start up slave tasks */
    numt=pvm_spawn(name, (char**)0, 0, "", Nproc, Tids);
    if( numt < Nproc ){
       fprintf(stderr,"Trouble spawning slaves. Aborting. Error codes are:\n");
       for( i=numt ; i<Nproc ; i++ ) {
          printf("TID %d %d\n",i,Tids[i]);
       }
       for( i=0 ; i<numt ; i++ ){
          pvm_kill( Tids[i] );
       }
       pvm_exit();
       PvmStarted = 0;
       return(-1);
    }
    PvmStarted = 1;
    return(0);
}
    
int KpvmMcast(char *comm) {
  if (!PvmStarted) return(-1);
  pvm_initsend(PvmDataDefault);
  pvm_pkstr(comm);  
  if (pvm_mcast(Tids, Nproc, 0)<0) {
    fprintf(stderr,"Error in mcast.\n");
    pvm_exit(); return(-1);
    PvmStarted = 0;
  }
  return(0);
}

int KpvmStopSlaves() {
  int dataId;
  if (!PvmStarted) return(-1);
  pvm_initsend(PvmDataDefault);
  dataId = -1;
  pvm_pkint(&dataId,1,1);
  pvm_pkstr("HALT");
  pvm_mcast(Tids, Nproc, 10);
  /*
    for (i=0; i<numt; i++) {
      pvm_kill(Tids[i]);
    }
    */

  /* Program Finished exit PVM before stopping */
  PvmStarted = 0;
  pvm_exit();
}

int KpvmChangeStateOfSlaves(int k) {
  int dataId;
  if (!PvmStarted) return(-1);
  pvm_initsend(PvmDataDefault);
  dataId = -1;
  pvm_pkint(&dataId,1,1);
  pvm_pkstr("LISTEN");
  pvm_mcast(Tids, Nproc, 10);
  /* The next command should be KpvmMcast(); */
}

struct object KpvmJobPool(struct object obj) {
  struct object rob;
  struct object ob1,ob2;
  int dataId,msgtype,remaining,ansp;
  int bytes,tag,rtid;
  int i,m;
  char **darray;
  char **aarray;
  char ans[1024];  /* temporary work area. */
  struct object op1;
  int info;

  rob.tag = Snull;
  if (!PvmStarted) return(rob);

  if (obj.tag != Sarray) {
    fprintf(stderr,"Argument must be an array.\n");
    return(rob);
  }
  m = getoaSize(obj);
  darray = (char **) GC_malloc(sizeof(char *)*(m+1));
  aarray = (char **) GC_malloc(sizeof(char *)*(m+1));
  for (i=0; i<m; i++) {
    ob1 = getoa(obj,i);
    if (ob1.tag != Sdollar) {
      fprintf(stderr,"Argument must be a string.\n");
      return(rob);
    }
    darray[i] = ob1.lc.str;
  }
    

  /* Wait for results or ready signal from slaves */
  msgtype = 5;
  remaining = ansp = m;
  while (ansp >= 0) {
    if (KpvmVerbose) printf("Waiting for msgtype=5.\n");
    info = pvm_recv( -1, msgtype );
    pvm_bufinfo(info,&bytes,&tag,&rtid);
    pvm_upkint(&dataId,1,1);
    pvm_upkstr(ans);
    if (strlen(ans) == 0) { /* slave is ready. */
      if (KpvmVerbose) printf("Slave %d is ready.",rtid);
    }else{
      ansp -- ;
      aarray[dataId] = (char *) GC_malloc(sizeof(char)*(strlen(ans)+2));
      strcpy(aarray[dataId],ans);
      if (KpvmVerbose) printf("[%d] %s from %d.\n",dataId,ans,rtid);
      if (ansp <= 0) break;
    }

    if (remaining > 0) {
      remaining--;
      pvm_initsend(PvmDataDefault); /* Always necessary to flush the old data. */
      pvm_pkint(&remaining,1,1);
      pvm_pkstr(darray[remaining]);
      if (KpvmVerbose)
	printf("Sending the message <<%s>> of the type 10.\n",darray[remaining]);
      pvm_send(rtid, 10);
    }

  }

  printf("-------------------------------\n");
  for (i=0; i<m; i++) {
    if (KpvmVerbose) printf("%s\n",aarray[i]);
  }
  printf("------------------------------\n\n");
  
  rob = newObjectArray(m);
  for (i=0; i<m; i++) {
    op1.tag = Sdollar;
    op1.lc.str = aarray[i];
    putoa(rob,i,op1);
  }
  return(rob);
}

#ifdef MSG1PTEST
main()
{
  int m,i;
  struct object obj;
  struct object op1;
  struct object rob;
  m = 5;


  if (KpvmStartSlaves(SLAVENAME,3)) exit();
  KpvmMcast("/afo { /n  set (x+1). n power [((x-1)^2).] reduction 0 get (string) dc} def ");

  obj = newObjectArray(m);
  for (i=0; i<m; i++) {
    op1.tag = Sdollar;
    op1.lc.str = (char *)GC_malloc(10);
    sprintf(op1.lc.str," %d afo ", 100 + i%10);
    putoa(obj,i,op1);
  }
  rob = KpvmJobPool(obj);
  printObject(rob,0,stdout); printf("\n");

  KpvmChangeStateOfSlaves(0);
  KpvmMcast("/afo2 { /n  set n 1 add (dollar) dc } def ");

  obj = newObjectArray(m);
  for (i=0; i<m; i++) {
    op1.tag = Sdollar;
    op1.lc.str = (char *)GC_malloc(10);
    sprintf(op1.lc.str," %d afo2 ", 100 + i%10);
    putoa(obj,i,op1);
  }
  rob = KpvmJobPool(obj);
  printObject(rob,0,stdout); printf("\n");



  KpvmStopSlaves();
}
#endif

