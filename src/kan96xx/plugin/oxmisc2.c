/* $OpenXM: OpenXM/src/kan96xx/plugin/oxmisc2.c,v 1.27 2013/11/06 06:23:24 takayama Exp $ */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ox_kan.h"
#include "oxmisc2.h"   /* This file requires sm1 object description. */
#include "cmo.h"

extern FILE *MyErrorOut;
#define SET_MYERROROUT { if (MyErrorOut == NULL) MyErrorOut=stdout; }
/* It is also defined in oxmisc.c */

extern int SerialOX; /* defined in SerialOX */

extern int OxVersion;

int DebugMathCap = 1;



int oxGet(oxclientp client, struct object *op,int *isObj)
     /* This method should be synchronized. */
     /* oxGet is a function for client. */
{
  int ans;
  ox_stream os;
  int m;
  struct object rob = OINIT;
  int sss;  /* Serial number of the recieved packet. */
  *isObj = 0;
  op->tag = Snull;
  os = client->datafp2;
  switch(client->dstate) {
  case DSTATE_ANY:
    m = oxGetOXheader(os,&sss);
    switch(m) {
    case OX_DATA:
      client->dstate = DSTATE_WAIT_OX_DATA;
      return(oxGet(client,op,isObj));
    case OX_SYNC_BALL:
      client->dstate = DSTATE_ANY;
      return(OX_SYNC_BALL);
    default:
      errorOxmisc2("oxGet: cannot handle this tag.\n");
      client->dstate = DSTATE_ERROR;
      return(-1);
    }
    break;
  case DSTATE_FIRST_SYNC:  /* waiting the first sync ball */
    /* I need to clear the buffer?? */
    oxWaitSyncBall(os);
    client->dstate = DSTATE_ANY;
    oxSendSyncBall(os);
    return(OX_SYNC_BALL);
    break;
  case DSTATE_WAIT_OX_DATA:  /* waiting a cmo data. */
    *op = cmoObjectFromStream2(client->datafp2);
    client->dstate = DSTATE_ANY;
    *isObj = 1;
    return(0); 
    break;
  case DSTATE_ERROR:
    client->dstate = DSTATE_ERROR;
    errorOxmisc2("oxGet: dstate == DSTATE_ERROR (error state)\n");
    return(-1);
  default:
    client->dstate = DSTATE_ERROR;
    errorOxmisc2("oxGet: Unknown state number.");
  }
  
  return(-1);
}

int oxGetFromControl(oxclientp client)
{
  int ans = -1;
  AbortIfRFC_101(client);
  if (client->cstate != -1) {
    ans = oxGetResultOfControlInt32(client->controlfd);
    if (ans != -1) {    client->cstate = 0; }
    else {client->cstate = -1; }
  }
  return(ans);
}
      
int oxReq(oxclientp client,int func,struct object ob)
{
  struct object *ob1p;
  SET_MYERROROUT;
  /* request to the control channel */
  if (func == SM_control_reset_connection ||
      func == SM_control_kill) {
    AbortIfRFC_101(client);
    switch(func) {
    case SM_control_reset_connection:
      oxReqControlResetConnection(client->controlfd);
      client->cstate = 0;
      client->dstate = DSTATE_FIRST_SYNC;
      break;
    case SM_control_kill:
      oxReqControlKill(client->controlfd);
      client->cstate = 0;
      client->dstate = DSTATE_ANY;
      break;
    }
    fflush(NULL);
    return(0);
  }

  /* request to the data channel */
  if (client->dstate != DSTATE_ANY) {
    errorOxmisc2("oxReq: client->dstate != DSTATE_ANY, data channel is not ready to send data.\n");
    return(-1);
  }
  switch(func) {
  case SM_DUMMY_sendcmo:
    if (!cmoCheckMathCap(ob,(struct object *)client->mathcapObjp)) {
      errorOxmisc2("oxReq: your peer does not understand this cmo.\n");
      return(-1);
    }
    oxSendOXheader(client->datafp2,OX_DATA,SerialOX++);
    cmoObjectToStream2(ob,client->datafp2);
    client->dstate = DSTATE_ANY;
    break;
  case SM_sync_ball:
    oxSendSyncBall(client->datafp2);
    client->dstate = DSTATE_ANY; /*  We do not expect the sync ball.*/
    client->cstate = 0;          /* clear the cstate */
    break;
  case SM_popCMO:
    oxReqPopCMO(client->datafp2);
    client->dstate = DSTATE_ANY;
    break;
  case SM_mathcap:
    oxReqMathCap(client->datafp2);
    client->dstate = DSTATE_ANY;
    break;
  case SM_setMathCap:
    /* ob = [(mathcap-obj) [[version num, system name] [sm tags]
       ob1                        smtags
       oxtags      [[ox numbers, [cmo numbers]]]
       ob3         ob2 */
    /*     oxtags      [[OX_DATA, [cmo numbers]],[OX_DATA_LOCAL,[opt]],...]*/
    {
      struct object ob1 = OINIT;
      struct object ob2 = OINIT;
      struct object ob3 = OINIT;
      struct object obm = OINIT;
      struct object smtags = OINIT;
      struct object oxtags = OINIT;
      struct object ox = OINIT;
      int n,i;
      struct mathCap mathcap;

      if (strcmp(KopString(getoa(ob,0)),"mathcap-object") != 0) {
        errorOxmisc2("data format error in oxReqSetMathCap");
        client->dstate = DSTATE_ANY;
        break;
      }
      obm = getoa(ob,1);
      ob1 = getoa(obm,0);
      smtags = getoa(obm,1);
      oxtags = getoa(obm,2);
      if (smtags.tag != Sarray || oxtags.tag != Sarray) {
        errorOxmisc2("data format error in oxReqSetMathCap");
      }
      ob1p = (struct object *) sGC_malloc(sizeof(struct object));
      *ob1p = ob1;
      mathcap.infop = ob1p;

      n = getoaSize(oxtags);
      if (n >= MATHCAP_SIZE) errorOxmisc2("Too big mathcap of your peer.");
      mathcap.oxSize = n;
      for (i=0; i<n; i++) {
        ox = getoa(oxtags,i);
        if (ox.tag != Sarray) {
          errorOxmisc2("Data format error of the third argument of mathcap.");
        }
        mathcap.ox[i] = KopInteger(getoa(ox,0));
        if (mathcap.ox[i] == OX_DATA) {
          if (getoaSize(ox) < 2) {
            errorOxmisc2("Data format error in an entry of the third argument of mathcap.");
          }
          ob2 = getoa(ox,1);
          if (ob2.tag != Sarray) {
            errorOxmisc2("Data format error in an entry of the third argument of mathcap.");
          }
          mathcap.n = getoaSize(ob2);
          if (n >= MATHCAP_SIZE) errorOxmisc2("Too big mathcap of your peer.");
          for (i=0; i<mathcap.n; i++) {
            mathcap.cmo[i] = KopInteger(getoa(ob2,i));
          }
        }
      }

      n = getoaSize(smtags);
      if (n >= MATHCAP_SIZE) errorOxmisc2("Too big mathcap of your peer.");
      mathcap.smSize = n;
      for (i=0; i<n; i++) {
        mathcap.sm[i] = KopInteger(getoa(smtags,i));
      }

      oxReqSetMathCap(client->datafp2,&mathcap);
      client->dstate = DSTATE_ANY;
    }
    break;
  case SM_pops:
    if (ob.tag != Sinteger) {
      errorOxmisc2("SM_pops : the argument must be an integer.");
      return(-1);
    }
    oxReqPops(client->datafp2, KopInteger(ob));
    client->dstate = DSTATE_ANY;
    break;
  case SM_executeStringByLocalParser:
    if (ob.tag != Sdollar) {
      errorOxmisc2("SM_executeStringByLocalParser : the argument must be a string.");
      return(-1);
    }
    oxReqExecuteStringByLocalParser(client->datafp2,KopString(ob));
    client->dstate = DSTATE_ANY;
    break;
  case SM_executeFunction:
    if (ob.tag != Sdollar) {
      errorOxmisc2("SM_executeFunction : the argument must be a string.");
      return(-1);
    }
    oxReqExecuteFunction(client->datafp2,KopString(ob));
    client->dstate = DSTATE_ANY;
    break;
  case SM_executeFunctionWithOptionalArgument:
    if (ob.tag != Sdollar) {
      errorOxmisc2("SM_executeFunctionWithOptionalArgument : the argument must be a string.");
      return(-1);
    }
    oxReqExecuteFunctionWithOptionalArgument(client->datafp2,KopString(ob));
    client->dstate = DSTATE_ANY;
    break;
  case SM_popString:
    oxReqPopString(client->datafp2);
    client->dstate = DSTATE_ANY;
    break;
  case SM_evalName:
    if (ob.tag != Sdollar) {
      errorOxmisc2("SM_evalName : the argument must be a string.");
      return(-1);
    }
    oxReqEvalName(client->datafp2,KopString(ob));
    client->dstate = DSTATE_ANY;
    break;
  case SM_setName:
    if (ob.tag != Sdollar) {
      errorOxmisc2("SM_setName : the argument must be a string.");
      return(-1);
    }
    oxReqSetName(client->datafp2,KopString(ob));
    client->dstate = DSTATE_ANY;
    break;
  case SM_getsp:
    oxReqSingleOperand(client->datafp2,SM_getsp);
    client->dstate = DSTATE_ANY;
    break;
  case SM_dupErrors:
    oxReqSingleOperand(client->datafp2,SM_dupErrors);
    client->dstate = DSTATE_ANY;
    break;
  case SM_pushCMOtag:
    oxReqSingleOperand(client->datafp2,SM_pushCMOtag);
    client->dstate = DSTATE_ANY;
    break;
  default:
    fprintf(MyErrorOut,"func=%d ",func);
    errorOxmisc2("This function is not implemented.");
    break;
  }
  fp2fflush(client->datafp2);
  return(0);
}

struct object KoxCreateClient(struct object ip,
                              struct object portStream,
                              struct object portControl,struct object pass)
{
  struct object rob = OINIT;
  oxclientp client;
  char *passControl; char *passData;
  struct object tob = OINIT;
  passControl = NULL; passData = NULL;
  rob.tag = Snull;
  if (ip.tag != Sdollar) {
    errorOxmisc2("KoxCreateClient(): The first argument must be a hostname given by a string.");
    return(rob);
  }
  if (portStream.tag == Sdollar) {
    client = oxCreateClientFile(KopString(ip),KopString(portStream),
                                "/dev/null","w");
    if (client == NULL) {
      errorOxmisc2("KoxCreateClient(): Open error.");
      return(rob);
    }
    rob = newObjectArray(N_OF_CLIENT_FIELDS);
    oxClientToObject(client,rob);
    return(rob);
  }
  
  if (portStream.tag != Sinteger) {
    errorOxmisc2("KoxCreateClient(): The second argument must be a port number given in an integer.");
    return(rob);
  }
  if (portControl.tag != Sinteger) {
    errorOxmisc2("KoxCreateClient(): The third argument must be a port number given in an integer.");
    return(rob);
  }
  if (pass.tag == Sarray) {
    if (getoaSize(pass) < 2) {
	  errorOxmisc2("KoxCreateClient(): the fourth argument --- pass must be an array of strings.");
	  return rob;
	}
    tob = getoa(pass,0);
	if (tob.tag != Sdollar) {
	  errorOxmisc2("KoxCreateClient(): the fourth argument --- pass must be an array of strings.");
	  return rob;
	}
    passControl = KopString(tob);
    tob = getoa(pass,1);
	if (tob.tag != Sdollar) {
	  errorOxmisc2("KoxCreateClient(): the fourth argument --- pass must be an array of strings.");
	  return rob;
	}
    passData = KopString(tob);
  }
  client = oxCreateClient(KopString(ip),KopInteger(portStream),KopInteger(portControl),passControl,passData);
  if (client == NULL) {
    errorOxmisc2("KoxCreateClient(): Open error.");
    return(rob);
  }
  rob = newObjectArray(N_OF_CLIENT_FIELDS);
  oxClientToObject(client,rob);
  oxClientListUpdate(rob);
  return(rob);
}

static int isItClientObject(struct object ob)
{
  int size,i;
  struct object ee[N_OF_CLIENT_FIELDS];
  if (ob.tag != Sarray) {
    return(0);
  }
  size = getoaSize(ob);
  if (size != N_OF_CLIENT_FIELDS) return(0);
  for (i=0; i<N_OF_CLIENT_FIELDS; i++) {
    ee[i] = getoa(ob,i);
  }

  if (ee[0].tag != Sdollar) return(0);
  if (strcmp(KopString(ee[0]),"client")!=0) return(0);
  
  if (ee[1].tag != Sfile) return(0);
  if (strcmp((ee[1]).lc.str,MAGIC2) != 0) return(0);

  for (i=2; i<=9; i++) {
    if (ee[i].tag != Sinteger) return(0);
  }
  return(1);
}


struct object KoxIsThereErrorClient(struct object ob)
{
  struct object rob = OINIT;
  int ans;
  int size;
  oxclient cc;
  rob.tag = Snull;
  if (!isItClientObject(ob)) {
    errorOxmisc2("KoxIsThereErrorClient(): the argument must be an array for client object.");
    return(rob);
  }
  if (oxObjectToClient(ob,&cc) == -1) return(KpoInteger(-1));
  ans = oxIsThereErrorClient(&cc);
  return(KpoInteger(ans));
}

int oxClientToObject(oxclientp client,struct object rob)
{
  struct object ob = OINIT;
  if (client == NULL) return 0;
  /* rob = newObjectArray(N_OF_CLIENT_FIELDS); */
  if (rob.tag != Sarray) {
    errorOxmisc2("oxClientToObject(): the second argument must be an array.");
    return(-1);
  }
  if (getoaSize(rob) != N_OF_CLIENT_FIELDS) {
    errorOxmisc2("oxClientToObject(): the second argument must be an array of size N_OF_CLIENT_FIELDS.");
    return(-1);
  }

  ob = KpoString("client");
  putoa(rob,0,ob);

  ob.tag = Sfile; ob.lc.str = MAGIC2; ob.rc.voidp = (void *)(client->datafp2);
  putoa(rob,1,ob);

  putoa(rob,2,KpoInteger(client->dataport));
  putoa(rob,3,KpoInteger(client->controlfd));
  putoa(rob,4,KpoInteger(client->controlport));
  putoa(rob,5,KpoInteger(client->dstate));
  putoa(rob,6,KpoInteger(client->cstate));
  putoa(rob,7,KpoInteger(client->humanio));
  putoa(rob,8,KpoInteger(client->id));
  putoa(rob,9,KpoInteger(client->type));
  if (client->mathcapObjp == NULL) {
    putoa(rob,10,NullObject);
  }else{
    putoa(rob,10,*((struct object *)(client->mathcapObjp)));
  }
  putoa(rob,11,KpoInteger(client->engineByteOrder));
  putoa(rob,12,KpoInteger(client->controlByteOrder));
  putoa(rob,13,KpoInteger(client->engineID));
  return(0);
}

int oxObjectToClient(struct object ob,oxclientp cp)
{
  struct object ob1 = OINIT;
  struct object *obp;
  if (cp == NULL) {
    errorOxmisc2("oxObjectToClient(): the second argument is NULL");
    return(-1);
  }
  if (!isItClientObject(ob)) {
    errorOxmisc2("oxObjectToClient(): the first argument is not client object.");
    oxInitClient(cp);
    return(-1);
  }

  ob1 = getoa(ob,1);
  cp->datafp2 = (FILE2 *) (ob1.rc.voidp);

  ob1 = getoa(ob,2);
  cp->dataport = KopInteger(ob1);

  ob1 = getoa(ob,3);
  cp->controlfd = KopInteger(ob1);

  ob1 = getoa(ob,4);
  cp->controlport = KopInteger(ob1);

  ob1 = getoa(ob,5);
  cp->dstate = KopInteger(ob1);

  ob1 = getoa(ob,6);
  cp->cstate = KopInteger(ob1);

  ob1 = getoa(ob,7);
  cp->humanio = KopInteger(ob1);

  ob1 = getoa(ob,8);
  cp->id = KopInteger(ob1);

  ob1 = getoa(ob,9);
  cp->type = KopInteger(ob1);

  ob1 = getoa(ob,10);
  if (ob1.tag == Snull) {
    cp->mathcapObjp = NULL;
  }else{
    obp = (struct object *) sGC_malloc(sizeof(struct object));
    *obp = ob1;
    cp->mathcapObjp = (void *)obp;
  }
  ob1 = getoa(ob,11);
  cp->engineByteOrder = KopInteger(ob1);
  ob1 = getoa(ob,12);
  cp->controlByteOrder = KopInteger(ob1);

  ob1 = getoa(ob,13);
  cp->engineID = KopInteger(ob1);
  

  return(0);
}

struct object KoxReq(struct object client,
                     struct object func,
                     struct object ob1)
{
  int ans;
  static oxclientp cc1 = NULL;
  struct object rob = OINIT;
  rob.tag = Snull;
  if (cc1 == NULL) {
    cc1 = (oxclientp) mymalloc(sizeof(oxclient));
    if (cc1 == NULL) {
      errorOxmisc2("KoxReq(): no more memory.");
      return(rob);
    }
    oxInitClient(cc1);
  }

  if (oxObjectToClient(client,cc1) == -1) return(rob);
  if (cc1 == NULL) {
    errorOxmisc2("KoxReq(): the first argument must be a client object.");
    return(rob);
  }
  if (func.tag != Sinteger) {
    errorOxmisc2("KoxReq(): the second argument must be an integer.");
    return(rob);
  }
  ans = oxReq(cc1,KopInteger(func),ob1);
  /* synchronize cc1 and client. */
  oxClientToObject(cc1,client);

  return(KpoInteger(ans));
}

struct object KoxGet(struct object client)
{
  int ans,k;
  static oxclientp cc1 = NULL;
  struct object rob = OINIT;
  rob.tag = Snull;
  if (cc1 == NULL) {
    cc1 = (oxclientp) mymalloc(sizeof(oxclient));
    if (cc1 == NULL) {
      errorOxmisc2("KoxGet(): no more memory.");
      return(rob);
    }
    oxInitClient(cc1);
  }

  if (oxObjectToClient(client,cc1) == -1) return(rob);
  if (cc1 == NULL) {
    errorOxmisc2("KoxGet(): the first argument must be a client object.");
    return(rob);
  }

  ans = oxGet(cc1,&rob,&k);
  /* synchronize cc1 and client. */
  oxClientToObject(cc1,client);

  if (k) return(rob);
  else {
    return(KpoInteger(ans));
  }
}

struct object KoxGetFromControl(struct object client)
{
  int ans;
  static oxclientp cc1 = NULL;
  struct object rob = OINIT;
  rob.tag = Snull;
  if (cc1 == NULL) {
    cc1 = (oxclientp) mymalloc(sizeof(oxclient));
    if (cc1 == NULL) {
      errorOxmisc2("KoxGetFromControl(): no more memory.");
      return(rob);
    }
    oxInitClient(cc1);
  }

  if (oxObjectToClient(client,cc1) == -1) return(rob);
  if (cc1 == NULL) {
    errorOxmisc2("KoxGetFromControl(): the first argument must be a client object.");
    return(rob);
  }

  ans = oxGetFromControl(cc1);
  /* synchronize cc1 and client. */
  oxClientToObject(cc1,client);

  return(KpoInteger(ans));
}

struct object KoxMultiSelect(struct object oclients,struct object t)
{
  static int first = 1;
  static int csize = 0;
  static oxclientp *clients = NULL;
  oxclientp cc1;
  struct object rob = OINIT;
  int i;
  int tt;
  struct object ob1 = OINIT;
  struct object ob2 = OINIT;
  struct object ob0 = OINIT;
  int size;
  int ans;
  int dataready[1024];
  int controlready[1024];
  
  rob.tag = Snull;
  if (oclients.tag != Sarray) {
    errorOxmisc2("KoxMultiSelect(): the first argument must be an array.");
    return(rob);
  }
  size = getoaSize(oclients);
  if (first) {
    first = 0; csize = size;
    clients = (oxclientp *)mymalloc(sizeof(oxclientp)*(size+1));
    if (clients == NULL) {
      errorOxmisc2("KoxMultiSelect(): no more memory.");
      return(rob);
    }
    for (i=0; i<size; i++) {
      clients[i] =  (oxclientp) mymalloc(sizeof(oxclient));
      if (clients[i] == NULL) {
        errorOxmisc2("KoxMultiSelect(): no more memory.");
        return(rob);
      }
      oxInitClient(clients[i]);
    }
  }
  if (csize < size)  {
    first = 1;
    return(KoxMultiSelect(oclients,t));
  }
  for (i=0; i<size; i++) {
    ob0 = getoa(oclients,i);
    if (oxObjectToClient(ob0,clients[i]) == -1) return(rob);
  }
  if (t.tag != Sinteger) {
    errorOxmisc2("KoxMultiSelect(): the second argument must be an integer.");
  }
  tt = KopInteger(t);
  ans = oxclientMultiSelect(clients,dataready,controlready,size,tt);
  /* synchronize oclients and clients. */
  for (i=0; i<size; i++) {
    ob0 = getoa(oclients,i);
    oxClientToObject(clients[i],ob0);
    putoa(oclients,i,ob0);
  }
  rob = newObjectArray(3);
  putoa(rob,0,KpoInteger(ans));
  ob1 = newObjectArray(size);
  ob2 = newObjectArray(size);
  for (i=0; i<size; i++) {
    putoa(ob1,i,KpoInteger(dataready[i]));
    putoa(ob2,i,KpoInteger(controlready[i]));
  }
  putoa(rob,1,ob1);
  putoa(rob,2,ob2);
  return(rob);
}

struct object KoxWatch(struct object client,struct object f)
     /* f is not used for now. It should be log file. */
{
  int ans,k;
  static oxclientp cc1 = NULL;
  struct object rob = OINIT;
  extern int WatchStream;
  rob.tag = Snull;
  if (client.tag == Sinteger) {
    if (KopInteger(client)) {
      WatchStream = 1;
    }else{
      WatchStream = 0;
    }
    return rob;
  }
  if (cc1 == NULL) {
    cc1 = (oxclientp) mymalloc(sizeof(oxclient));
    if (cc1 == NULL) {
      errorOxmisc2("KoxWatch(): no more memory.");
      return(rob);
    }
    oxInitClient(cc1);
  }

  if (oxObjectToClient(client,cc1) == -1) return(rob);
  if (cc1 == NULL) {
    errorOxmisc2("KoxWatch(): the first argument must be a client object.");
    return(rob);
  }

  k = fp2watch(cc1->datafp2,stdout);
  /* synchronize cc1 and client. */
  oxClientToObject(cc1,client);

  return(KpoInteger(ans));
}

struct object KoxLog(struct object client,struct object in,struct object out)
{
  int ans,k;
  static oxclientp cc1 = NULL;
  struct object rob = OINIT;
  rob.tag = Snull;
  if (cc1 == NULL) {
    cc1 = (oxclientp) mymalloc(sizeof(oxclient));
    if (cc1 == NULL) {
      errorOxmisc2("KoxLog(): no more memory.");
      return(rob);
    }
    oxInitClient(cc1);
  }

  if (oxObjectToClient(client,cc1) == -1) return(rob);
  if (cc1 == NULL) {
    errorOxmisc2("KoxLog(): the first argument must be a client object.");
    return(rob);
  }

  if (in.tag != Sfile) {
	errorOxmisc2("KoxLog(): the second argument is not a file object.");
	return rob;
  }
  if (out.tag != Sfile) {
	errorOxmisc2("KoxLog(): the third argument is not a file object.");
	return rob;
  }
  k = fp2log(cc1->datafp2,in.rc.file,out.rc.file);
  fputc(cc1->engineByteOrder,out.rc.file); /* Output engineByteOrder. */
  /* synchronize cc1 and client. */
  oxClientToObject(cc1,client);

  return(KpoInteger(ans));
}

struct object KoxLogStop(struct object client) {
  static oxclientp cc1 = NULL;
  struct object rob = OINIT;
  rob.tag = Snull;
  if (cc1 == NULL) {
    cc1 = (oxclientp) mymalloc(sizeof(oxclient));
    if (cc1 == NULL) {
      errorOxmisc2("KoxLog(): no more memory.");
      return(rob);
    }
    oxInitClient(cc1);
  }

  if (oxObjectToClient(client,cc1) == -1) return(rob);
  if (cc1 == NULL) {
    errorOxmisc2("KoxLog(): the first argument must be a client object.");
    return(rob);
  }
  return(KpoInteger(fp2stopLog(cc1->datafp2)));  
}
    
struct object KoxCloseClient(struct object client) {
  oxclientp cc1 = NULL;
  oxclient cc;
  struct object rob = OINIT;
  rob.tag = Snull;
  cc1  = &cc;
  if (oxObjectToClient(client,cc1) == -1) return(rob);
  if (cc1 == NULL) {
    errorOxmisc2("KoxCloseClient(): the first argument must be a client object.");
    return(rob);
  }

  fp2fflush(cc1->datafp2);
  if (cc1->humanio) {
    /* Do not close the file. */
    return(KpoInteger(0));
  }
  switch (cc1->type) {
  case CLIENT_SOCKET:
    fp2fclose(cc1->datafp2);
    {int r; r=close(cc1->controlfd);}
    break;
  case CLIENT_FILE:
    fp2fclose(cc1->datafp2);
    {int r; r=close(cc1->controlfd);}
    break;
  default:
    errorOxmisc2("Unknown client->type\n");
    break;
  }
  oxClientListRemove(client);
  return(KpoInteger(0));

}

static int cmoCheck00(struct object obj,int cmo[], int n) {
  int i,j,m;
  int ttt;
#define CHECK00_N  4098      /* look up stackm.h and kclass.h */
  static int typeTrans[CHECK00_N]; 
  static int init = 0;
  /* if n == 0, report the cmo tag of the object obj.
     If it cannot be translated to cmo, then return -1. */

  if (!init) {
    for (i=0; i<CHECK00_N; i++) {
      typeTrans[i] = 0;  /* unknown cmo number */
    }
    typeTrans[Snull] = CMO_NULL;
    typeTrans[Sinteger] = CMO_INT32;
    typeTrans[Sdollar] = CMO_STRING;
    if (OxVersion >= 199907170) {
      typeTrans[SuniversalNumber] = CMO_ZZ;
    }else{
      typeTrans[SuniversalNumber] = CMO_ZZ_OLD;
    }
    typeTrans[Sarray] = CMO_LIST;
    /* typeTrans[Spoly] = CMO_DMS;  */
    typeTrans[Spoly] = CMO_DISTRIBUTED_POLYNOMIAL; 
    typeTrans[Sdouble] = CMO_64BIT_MACHINE_DOUBLE;
	typeTrans[SrationalFunction] = CMO_RATIONAL;
    typeTrans[CLASSNAME_ERROR_PACKET]   = CMO_ERROR2;
    typeTrans[CLASSNAME_mathcap] = CMO_MATHCAP; 
    typeTrans[CLASSNAME_indeterminate] = CMO_INDETERMINATE; 
    typeTrans[CLASSNAME_tree] = CMO_TREE; 
    typeTrans[CLASSNAME_recursivePolynomial] = CMO_RECURSIVE_POLYNOMIAL; 
    typeTrans[CLASSNAME_polynomialInOneVariable] = CMO_POLYNOMIAL_IN_ONE_VARIABLE;
    init = 1;
  }
  ttt = typeTrans[obj.tag];
  if (obj.tag == Sclass) {
    ttt = typeTrans[ectag(obj)];
  }
  /* Only report the cmo tag. */
  if (n == 0) {
    if (ttt == 0) return(-1);
    else return(ttt);
  }

  for (i=0; i<n; i++) {
    if (ttt == cmo[i]) {
      if (ttt != CMO_LIST) return(1);
      else {
        m = getoaSize(obj);
        for (j=0; j<m; j++) {
          if (!cmoCheck00(getoa(obj,j),cmo,n)) return(0);
        }
        return(1);
      }
    }
  }
  if (DebugMathCap) {
    if (DebugMathCap && 1) {
      fprintf(stderr,"Type translation table (internal object tag --> CMO tag)\n");
      for (i=0; i<20; i++) {
        printf("%d ", typeTrans[i]);
      }
      printf("\n");
    }
    fprintf(stderr,"The type of the argument object in sm1 is %d.\n",obj.tag);
    fprintf(stderr,"The type of the argument object in CMO is %d.\n",ttt);
    fprintf(stderr,"Available CMO tags in mathcap= %d elements : [ ",n);
    for (i=0; i<n; i++) {
      fprintf(stderr," %d ",cmo[i]);
    }
    fprintf(stderr," ] \n");
  }
  return(0);
}
    
int cmoCheckMathCap(struct object obj, struct object *obp)
{
  struct object mathcap = OINIT;
  struct object cmolist = OINIT;
  struct object mathcapMain = OINIT;
  struct object mathcapThird = OINIT;
  struct object ox = OINIT;
  struct object oxtag = OINIT;
  struct object ob0 = OINIT;
  int oxsize;
  int n;
  int i;
#define CMO_CHECK_MATH_CAP_LIST_SIZE 1024
  int cmo[CMO_CHECK_MATH_CAP_LIST_SIZE];
  if (obp == NULL) return(1);
  /* printObject(*obp,0,stderr); for debug*/
  if (obp->tag != Sarray) {
    fprintf(stderr,"cmoCheckMathCap: the mathcap obj is \n");
    printObject(*obp,0,stderr);
    fprintf(stderr,"\n");
    errorOxmisc2("cmoCheckMathCap: format error in the client->mathcapObjp field.\n");
  }
  mathcap = *obp;
  /* Example of mathcap 
     [    $mathcap-object$ ,
     [    [    199909080 , $Ox_system=ox_sm1.plain$ , $Version=2.991106$ , 
     $HOSTTYPE=i386$ ]  , 
     [    262 , 263 , 264 , 265 , 266 , 268 , 269 , 272 , 273 , 275 , 
     276 ]  , 
     [    [    514  , [    2130706434 , 1 , 2 , 4 , 5 , 17 , 19 , 20 , 22 , 23 , 24 , 25 , 26 , 30 , 31 , 60 , 61 , 27 , 33 , 40 , 16 , 34 ] ] ]  ]  ]
  */

  n = getoaSize(mathcap);
  if (n < 2) {
    fprintf(stderr,"cmoCheckMathCap: the mathcap obj is \n");
    printObject(*obp,0,stderr);
    fprintf(stderr,"\n");
    errorOxmisc2("cmoCheckMathCap: length of mathcap is wrong in the client->mathcapObjp field.\n");
  }
  ob0 = getoa(mathcap,0);
  if (ob0.tag != Sdollar) {
    fprintf(stderr,"cmoCheckMathCap: the mathcap obj is \n");
    printObject(*obp,0,stderr);
    fprintf(stderr,"\n");
    errorOxmisc2("cmoCheckMathCap: The first field must be the string mathcap-object.\n");
  }
  if (strcmp(KopString(ob0),"mathcap-object") != 0) {
    fprintf(stderr,"cmoCheckMathCap: the mathcap obj is \n");
    printObject(*obp,0,stderr);
    fprintf(stderr,"\n");
    errorOxmisc2("cmoCheckMathCap: The mathcap must be of the form [(mathcap-object) [...]]\n");
  }
    
  /* I should check 
     getoa(getoa(mathcap,1),2)
     contains OX_DATA.
     It has not yet implemented.
  */
  mathcapMain = getoa(mathcap,1);
  if (mathcapMain.tag != Sarray) {
    fprintf(stderr,"cmoCheckMathCap: mathcap[1] is \n");
    printObject(mathcapMain,0,stderr);
    fprintf(stderr,"\n");
    errorOxmisc2("cmoCheckMathCap: format error in the (client->mathcapObjp)[1] field. It should be an array.\n");
  }
  if (getoaSize(mathcapMain) < 3) {
    fprintf(stderr,"cmoCheckMathCap: mathcap[1] is \n");
    printObject(mathcapMain,0,stderr);
    fprintf(stderr,"\n");
    errorOxmisc2("cmoCheckMathCap: format error in the (client->mathcapObjp)[1] field. It should be an array of which length is more than 2.\n");
  }
  mathcapThird = getoa(mathcapMain,2);
  oxsize = getoaSize(mathcapThird);
  for (i=0; i<oxsize; i++) {
    ox = getoa(mathcapThird,i);
    if (ox.tag != Sarray) {
      fprintf(stderr,"cmoCheckMathCap: the mathcap obj is \n");
      printObject(*obp,0,stderr);
      fprintf(stderr,"\n");
      errorOxmisc2("cmoCheckMathCap: the third element of mathcap is a list of lists.");
    }
    if (getoaSize(ox) != 0) {
      oxtag = getoa(ox,0);
      if (oxtag.tag != Sinteger) {
        fprintf(stderr,"cmoCheckMathCap: the mathcap obj is \n");
        printObject(*obp,0,stderr);
        fprintf(stderr,"\n");
        errorOxmisc2("cmoCheckMathCap: the third element of mathcap must be [OX_DATA_xxx, [  ]].");
      }
      if (KopInteger(oxtag) == OX_DATA) {
        if (getoaSize(ox) > 1) {
          cmolist = getoa(ox,1);
          if (cmolist.tag != Sarray) {
            fprintf(stderr,"cmoCheckMathCap: the mathcap obj is \n");
            printObject(*obp,0,stderr);
            fprintf(stderr,"\n");
            errorOxmisc2("cmoCheckMathCap: mathcap[1] must be an array of integers.\n");
          }
          n = getoaSize(cmolist);
          if (n > CMO_CHECK_MATH_CAP_LIST_SIZE) {
            errorOxmisc2("cmoCheckMathCap: Too big cmo list.\n");
          }
          for (i=0; i<n; i++) {
            cmo[i] = KopInteger(getoa(cmolist,i));
          }
        }else{
          fprintf(stderr,"cmoCheckMathCap: the mathcap obj is \n");
          printObject(*obp,0,stderr);
          fprintf(stderr,"\nox=");
          printObject(ox,0,stderr);
          errorOxmisc2("cmoCheckMathCap: [OX_DATA, cmolist]");
        }
      }
    }
  }
  return(cmoCheck00(obj,cmo,n));
}
    
             
struct object KoxGenPortFile(void) {
  struct object ob = OINIT;
  ob = KpoString(oxGenPortFile());
  return(ob);
}
void KoxRemovePortFile(void) {
  oxRemovePortFile();
}

void oxPushMathCap(struct mathCap *mathcap)
{
  struct object rob = OINIT;
  rob = newMathCap(mathcap); 
  Kpush(rob);
}

struct object KoxGenPass(void) {
  struct object rob = OINIT;
  rob = KpoString(oxGenPass());
  return(rob);
}

struct object KoxGetPort(struct object host)
{
  struct object rob = OINIT;
  int fdStream, fdControl;
  int portStream, portControl;
  extern int OpenedSocket;
  char *sname;
  rob = NullObject;
  if (host.tag != Sdollar) {
    errorOxmisc2("KoxGetPort: argument is not a string.");
    return(rob);
  }
  sname = KopString(host);
  fdControl = socketOpen(sname,0);
  portControl = OpenedSocket;
  fdStream = socketOpen(sname,0);
  portStream = OpenedSocket;
  rob = newObjectArray(4);
  putoa(rob,0,KpoInteger(fdStream));
  putoa(rob,1,KpoInteger(portStream));
  putoa(rob,2,KpoInteger(fdControl));
  putoa(rob,3,KpoInteger(portControl));
  return(rob);
}
struct object KoxGetPort1(struct object host)
{
  struct object rob = OINIT;
  int fdStream;
  int portStream;
  extern int OpenedSocket;
  char *sname;
  rob = NullObject;
  if (host.tag != Sdollar) {
    errorOxmisc2("KoxGetPort1: argument is not a string.");
    return(rob);
  }
  sname = KopString(host);
  fdStream = socketOpen(sname,0);
  portStream = OpenedSocket;
  rob = newObjectArray(2);
  putoa(rob,0,KpoInteger(fdStream));
  putoa(rob,1,KpoInteger(portStream));
  return(rob);
}

struct object KoxCreateClient2(struct object peer,
                               struct object ipmask,
                               struct object pass)
{
  struct object rob = OINIT;
  oxclientp client;
  int fdStream, portStream, fdControl, portControl;
  int i;
  struct object ob1 = OINIT;
  struct object opassControl = OINIT;
  struct object opassData = OINIT;
  rob.tag = Snull;
  if (peer.tag != Sarray) {
    errorOxmisc2("KoxCreateClient2(): The first argument must be an array [fdStream, portStream, fdControl, portControl]");
    return(rob);
  }
  if (getoaSize(peer) != 4) {
    errorOxmisc2("KoxCreateClient2(): The first argument must be an array [fdStream, portStream, fdControl, portControl] of size 4.");
    return(rob);
  }
  for (i=0; i<4; i++) {
    ob1 = getoa(peer,i);
    if (ob1.tag != Sinteger) {
      errorOxmisc2("KoxCreateClient2(): The element of the first argument must be an integer.");
    }
  }
  fdStream = KopInteger(getoa(peer,0));
  portStream = KopInteger(getoa(peer,1));
  fdControl = KopInteger(getoa(peer,2));
  portControl = KopInteger(getoa(peer,3));

  if (ipmask.tag != Sinteger) {
    errorOxmisc2("KoxCreateClient2(): ipmask must be an integer.");
  }
  if (pass.tag == Sdollar) {
    opassControl = pass; opassData = pass;
  }else if (pass.tag == Sarray) {
    if (getoaSize(pass) < 2) errorOxmisc2("KoxCreateClient2: #passArray < 2.");
    opassControl = getoa(pass,0);
    opassData = getoa(pass,1);
  }else{
    errorOxmisc2("KoxCreateClient2(): pass must be a string or an array.");
  }
  if ((opassControl.tag != Sdollar) || (opassData.tag != Sdollar)) {
    errorOxmisc2("KoxCreateClient2(): opassControl or opassData must be a string.");
  }

  client = oxCreateClient2(fdStream, portStream, fdControl, portControl,
                           KopInteger(ipmask),
                           KopString(opassControl),KopString(opassData));
  if (client == NULL) {
    errorOxmisc2("KoxCreateClient2(): Open error.");
    return(rob);
  }
  rob = newObjectArray(N_OF_CLIENT_FIELDS);
  oxClientToObject(client,rob);
  oxClientListUpdate(rob);
  return(rob);
}

int KgetCmoTagOfObject(struct object obj) {
  int k;
  k=cmoCheck00(obj,(int *)NULL,0);
  return(k);
}

int errorOxmisc2(char *s) {
  SET_MYERROROUT;  
  fprintf(MyErrorOut,"error in oxmisc2.c: %s\n",s);
  errorKan1("%s\n","  ");
  return 0;
}

struct object KoxPushCMD(struct object client,struct object cmd) {
  int ans;
  static oxclientp cc1 = NULL;
  struct object rob = OINIT;
  rob.tag = Snull;
  if (cc1 == NULL) {
    cc1 = (oxclientp) mymalloc(sizeof(oxclient));
    if (cc1 == NULL) {
      errorOxmisc2("KoxReq(): no more memory.");
      return(rob);
    }
    oxInitClient(cc1);  /* BUG: is it fine? */
  }

  if (oxObjectToClient(client,cc1) == -1) return(rob);
  if (cc1 == NULL) {
    errorOxmisc2("KoxReq(): the first argument must be a client object.");
    return(rob);
  }
  if (cmd.tag != Sinteger) {
    errorOxmisc2("KoxReq(): the second argument must be an integer.");
    return(rob);
  }
  /* BUG: check the mathcap */
  oxSendOXheader(cc1->datafp2,OX_COMMAND,SerialOX++);
  oxSendInt32(cc1->datafp2,KopInteger(cmd));
  /* synchronize cc1 and client. */
  oxClientToObject(cc1,client);
  return(cmd);
}

struct object KoxPushCMO(struct object client,struct object ob) {
  int ans;
  static oxclientp cc1 = NULL;
  struct object rob = OINIT;
  rob.tag = Snull;
  if (cc1 == NULL) {
    cc1 = (oxclientp) mymalloc(sizeof(oxclient));
    if (cc1 == NULL) {
      errorOxmisc2("KoxReq(): no more memory.");
      return(rob);
    }
    oxInitClient(cc1);  /* BUG: is it fine? */
  }

  if (oxObjectToClient(client,cc1) == -1) return(rob);
  if (cc1 == NULL) {
    errorOxmisc2("KoxReq(): the first argument must be a client object.");
    return(rob);
  }

  /* request to the data channel */
  if (cc1->dstate != DSTATE_ANY) {
    errorOxmisc2("oxPushCMO: cc1->dstate != DSTATE_ANY, data channel is not ready to send data.\n");
    return(rob);
  }

  if (!cmoCheckMathCap(ob,(struct object *)cc1->mathcapObjp)) {
    errorOxmisc2("oxPushCMO: your peer does not understand this cmo.\n");
    return(rob);
  }
  oxSendOXheader(cc1->datafp2,OX_DATA,SerialOX++);
  cmoObjectToStream2(ob,cc1->datafp2);
  /* synchronize cc1 and client. */
  oxClientToObject(cc1,client);
  return(ob);
}

oxclientp oxCreateControl_RFC_101(int fdstream,int portStream,
                                  int ipmask,char *pass);
struct object KoxCreateControl_RFC_101(struct object peer,struct object ipmask,struct object pass) 
{
  struct object rob = OINIT;
  oxclientp client;
  int fdStream, portStream;
  int i;
  struct object ob1 = OINIT;
  rob.tag = Snull;
  if (peer.tag != Sarray) {
    errorOxmisc2("KoxCreateControl_RFC_101(): The first argument must be an array [fdStream, portStream]");
    return(rob);
  }
  if (getoaSize(peer) != 2 ) {
    errorOxmisc2("KoxCreateControl_RFC_101(): The first argument must be an array [fdStream, portStream] of size 2.");
    return(rob);
  }
  for (i=0; i<getoaSize(peer); i++) {
    ob1 = getoa(peer,i);
    if (ob1.tag != Sinteger) {
      errorOxmisc2("KoxCreateControl_RFC_101(): The element of the first argument must be an integer.");
    }
  }
  fdStream = KopInteger(getoa(peer,0));
  portStream = KopInteger(getoa(peer,1));

  if (ipmask.tag != Sinteger) {
    errorOxmisc2("KoxCreateControl_RFC_101(): ipmask must be an integer.");
  }
  if (pass.tag != Sdollar) {
    errorOxmisc2("KoxCreateControl_RFC_101(): pass must be a string.");
  }

  client = oxCreateControl_RFC_101(fdStream, portStream,
                                   KopInteger(ipmask), KopString(pass));
  if (client == NULL) {
    errorOxmisc2("KoxCreateControl_RFC_101(): Open error.");
    return(rob);
  }
  rob = newObjectArray(N_OF_CLIENT_FIELDS);
  oxClientToObject(client,rob);
  return(rob);
}

oxclientp oxCreateControl_RFC_101(int fdstream,int portStream,
                                  int ipmask,char *pass)
{
  int v = 0;
  int fdControl = -1;
  int fdStream = -1;
  int m;

  char *s;
  oxclientp client;
#if defined(__CYGWIN__)
  extern sigjmp_buf MyEnv_oxmisc;
#else
  extern jmp_buf MyEnv_oxmisc;
#endif
  int engineByteOrder;
  extern int Quiet;

  v = !Quiet;
  
  switch(ipmask) {
  case 0:/* only local */
    fdStream  = socketAcceptLocal(fdstream);
    break;
  default:/* any */
    fdStream  = socketAccept(fdstream);
    break;
  }
  if (v) fprintf(stderr,"\nControl port %d : Connected.\n",portStream);

  if (fdStream == -1 ) {
    fprintf(stderr,"\nOpen error in oxCreateControl_RFC_101.\n");
    return(NULL);
  }

  /* Authentication by password. */
  m = strlen(pass);
  s = (char *)mymalloc(sizeof(char)*(m+1));
  {int r; r=read(fdStream,s,m+1);} s[m] = '\0';
  if (strcmp(s,pass) != 0) {
    fprintf(stderr,"oxCreateControl_RFC_101(): password authentication failed for control channel.\n");
    {int r; r=close(fdStream);}
    return(NULL);
  }


  engineByteOrder = oxSetByteOrder(fdStream);
  if (v) fprintf(stderr,"Byte order for control stackmacine is %s.\n",
                 (engineByteOrder == 0? "network byte order":
                  (engineByteOrder == 1? "little indican":
                   "big indian")));
  

  client = (oxclientp) mymalloc(sizeof(oxclient));
  oxInitClient(client);
  client->datafp2 = fp2open(fdStream);
  if (client->datafp2 == NULL) {
    fprintf(stderr,"oxCreateControl_RFC_101(): fp2open(fd) failed.\n");
    return(NULL);
  }
  client->dataport = portStream;
  client->controlport = -1;
  client->controlfd = -1;
  client->id = oxGetClientID();
  client->type = CLIENT_SOCKET; /* socket */
  client->engineByteOrder = engineByteOrder;
  client->controlByteOrder = -1;
  return(client);
}

oxclientp oxCreateEngine_RFC_101(int fdstream,int portStream,
                                 int ipmask,char *pass, int engineID);
struct object KoxCreateEngine_RFC_101(struct object peer,struct object ipmask,struct object pass, struct object engineID) 
{
  struct object rob = OINIT;
  oxclientp client;
  int fdStream, portStream;
  int i;
  struct object ob1 = OINIT;
  rob.tag = Snull;
  if (peer.tag != Sarray) {
    errorOxmisc2("KoxCreateEngine_RFC_101(): The first argument must be an array [fdStream, portStream]");
    return(rob);
  }
  if (getoaSize(peer) != 2 ) {
    errorOxmisc2("KoxCreateEngine_RFC_101(): The first argument must be an array [fdStream, portStream] of size 2.");
    return(rob);
  }
  for (i=0; i<getoaSize(peer); i++) {
    ob1 = getoa(peer,i);
    if (ob1.tag != Sinteger) {
      errorOxmisc2("KoxCreateEngine_RFC_101(): The element of the first argument must be an integer.");
    }
  }
  fdStream = KopInteger(getoa(peer,0));
  portStream = KopInteger(getoa(peer,1));

  if (ipmask.tag != Sinteger) {
    errorOxmisc2("KoxCreateEngine_RFC_101(): ipmask must be an integer.");
  }
  if (pass.tag != Sdollar) {
    errorOxmisc2("KoxCreateEngine_RFC_101(): pass must be a string.");
  }
  if (engineID.tag != Sinteger) {
    errorOxmisc2("KoxCreateEngine_RFC_101(): engineID must be an integer.");
  }

  client = oxCreateEngine_RFC_101(fdStream, portStream,
                                  KopInteger(ipmask), KopString(pass),KopInteger(engineID));
  if (client == NULL) {
    errorOxmisc2("KoxCreateEngine_RFC_101(): Open error.");
    return(rob);
  }
  rob = newObjectArray(N_OF_CLIENT_FIELDS);
  oxClientToObject(client,rob);
  oxClientListUpdate(rob);
  return(rob);
}

oxclientp oxCreateEngine_RFC_101(int fdstream,int portStream,
                                 int ipmask,char *pass,int engineID)
{
  int v = 0;
  int fdControl = -1;
  int fdStream = -1;
  int m;

  char *s;
  oxclientp client;
#if defined(__CYGWIN__)
  extern sigjmp_buf MyEnv_oxmisc;
#else
  extern jmp_buf MyEnv_oxmisc;
#endif
  int engineByteOrder;
  extern int Quiet;

  v = !Quiet;
  
  switch(ipmask) {
  case 0:/* only local */
    fdStream  = socketAcceptLocal(fdstream);
    break;
  default:/* any */
    fdStream  = socketAccept(fdstream);
    break;
  }
  if (v) fprintf(stderr,"\nEngine port %d : Connected.\n",portStream);

  if (fdStream == -1 ) {
    fprintf(stderr,"\nOpen error in oxCreateEngine_RFC_101.\n");
    return(NULL);
  }

  /* Authentication by password. */
  /*  skip password check for now. BUG. 
      m = strlen(pass);
      s = (char *)mymalloc(sizeof(char)*(m+1));
      read(fdStream,s,m+1); s[m] = '\0';
      if (strcmp(s,pass) != 0) {
      fprintf(stderr,"oxCreateEngine_RFC_101(): password authentication failed for control channel.\n");
      close(fdStream);
      return(NULL);
      }
  */

  engineByteOrder = oxSetByteOrder(fdStream);
  if (v) fprintf(stderr,"Byte order for engine stackmacine is %s.\n",
                 (engineByteOrder == 0? "network byte order":
                  (engineByteOrder == 1? "little indican":
                   "big indian")));
  

  client = (oxclientp) mymalloc(sizeof(oxclient));
  oxInitClient(client);
  client->datafp2 = fp2open(fdStream);
  if (client->datafp2 == NULL) {
    fprintf(stderr,"oxCreateEngine_RFC_101(): fp2open(fd) failed.\n");
    return(NULL);
  }
  client->dataport = portStream;
  client->controlport = -1;
  client->controlfd = -1;
  client->id = oxGetClientID();
  client->engineID = engineID;
  client->type = CLIENT_SOCKET; /* socket */
  client->engineByteOrder = engineByteOrder;
  client->controlByteOrder = -1;
  return(client);
}

void oxClientListUpdate(struct object ob) {
  int id;
  extern struct object OxClientList[];
  id = KopInteger(getoa(ob,8));
  /* printf("id=%d\n",id); */
  if ((id <MAX_N_OF_CLIENT) && (id >= 0)) {
	OxClientList[id] = ob;
  }else{
	errorOxmisc2("oxClientListUpdate(): the client table is full.\n");
  }
}
void oxClientListRemove(struct object ob) {
  int id;
  extern struct object OxClientList[];
  id = KopInteger(getoa(ob,8));
  if ((id <MAX_N_OF_CLIENT) && (id >= 0)) {
	(OxClientList[id]).tag = Snull;
  }else{
	/* errorOxmisc2("oxClientListRemove(): the client table is full.\n");*/
  }
}
static void KoxCleanClientList() {
  extern int OxClientListn;
  extern struct object OxClientList[];
  int i,j,n;
  struct object ob = OINIT;
  n = 0;
  for (i=0; i<OxClientListn; i++) {
	if ((OxClientList[i]).tag != Snull) {
	  if (!isItClientObject(OxClientList[i])) {
		(OxClientList[i]).tag = Snull;
	  }
	}
  }
}
struct object KoxGetClientList() {
  extern int OxClientListn;
  extern struct object OxClientList[];
  int i,j,n;
  struct object rob = OINIT;
  KoxCleanClientList();
  n = 0;
  for (i=0; i<OxClientListn; i++) {
	if ((OxClientList[i]).tag != Snull) n++;
  }
  rob = newObjectArray(n); 
  for (i=0, j=0; i<OxClientListn; i++) {
	if ((OxClientList[i]).tag != Snull) {
	  if (j >= n) {
		j=0;
		errorOxmisc2("oxGetClientList(): the client table is broken.\n");
	  }
	  putoa(rob,j,OxClientList[i]);
	  j++;
	}
  }
  return rob;
}


