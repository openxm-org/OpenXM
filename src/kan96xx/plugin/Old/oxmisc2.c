#include <stdio.h>
#include "ox_kan.h"
#include "oxmisc2.h"   /* This file requires sm1 object description. */
extern FILE *MyErrorOut;

int oxGet(oxclientp client, struct object *op,int *isObj)
/* This method should be synchronized. */
/* oxGet is a function for client. */
{
  int ans;
  ox_stream os;
  int m;
  struct object rob;
  *isObj = 0;
  op->tag = Snull;
  os = client->datafp2;
  switch(client->dstate) {
  case DSTATE_ANY:
    m = oxGetInt32(os);
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
  if (client->cstate != -1) {
    ans = oxGetResultOfControlInt32(client->controlfd);
    if (ans != -1) {    client->cstate = 0; }
    else {client->cstate = -1; }
  }
  return(ans);
}
      
int oxReq(oxclientp client,int func,struct object ob)
{
  /* request to the control channel */
  if (func == SM_control_reset_connection ||
      func == SM_control_kill) {
    switch(func) {
    case SM_control_reset_connection:
      oxReqControlResetConnection(client->controlfd);
      client->cstate = 1;
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
    oxSendInt32(client->datafp2,OX_DATA);
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
			      struct object portControl)
{
  struct object rob;
  oxclientp client;
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
  client = oxCreateClient(KopString(ip),KopInteger(portStream),KopInteger(portControl));
  if (client == NULL) {
    errorOxmisc2("KoxCreateClient(): Open error.");
    return(rob);
  }
  rob = newObjectArray(N_OF_CLIENT_FIELDS);
  oxClientToObject(client,rob);
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

  for (i=2; i<N_OF_CLIENT_FIELDS; i++) {
    if (ee[i].tag != Sinteger) return(0);
  }
  return(1);
}


struct object KoxIsThereErrorClient(struct object ob)
{
  struct object rob;
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
  struct object ob;
  if (client == NULL) return;
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
  return(0);
}

int oxObjectToClient(struct object ob,oxclientp cp)
{
  struct object ob1;
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

  return(0);
}

struct object KoxReq(struct object client,
		     struct object func,
		     struct object ob1)
{
  int ans;
  static oxclientp cc1 = NULL;
  struct object rob;
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
  struct object rob;
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
  struct object rob;
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
  struct object rob;
  int i;
  int tt;
  struct object ob1;
  struct object ob2;
  struct object ob0;
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
  struct object rob;
  rob.tag = Snull;
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

    
struct object KoxCloseClient(struct object client) {
  oxclientp cc1 = NULL;
  oxclient cc;
  struct object rob;
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
    close(cc1->controlfd);
    break;
  case CLIENT_FILE:
    fp2fclose(cc1->datafp2);
    close(cc1->controlfd);
    break;
  default:
    errorOxmisc2("Unknown client->type\n");
    break;
  }
  return(KpoInteger(0));

}

		     
errorOxmisc2(char *s) {
  fprintf(MyErrorOut,"error in oxmisc2.c: %s\n",s);
  return;
}
