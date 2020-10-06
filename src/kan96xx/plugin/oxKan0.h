/* $OpenXM: OpenXM/src/kan96xx/plugin/oxKan0.h,v 1.11 2004/09/17 08:46:10 takayama Exp $ */
/* ox_kan0.h */

#define mymalloc(n) sGC_malloc(n)
#define myfree(p) sGC_free(p)
typedef FILE2 *  ox_stream;

int socketOpen(char *serverName,int portNumber);
/* OpenedSocket is used to return the port number. */
int socketAccept(int snum);
int socketAcceptLocal(int snum);
int oxSocketSelect0(int fd,int t);
int oxSocketMultiSelect(int sid[],int size,int t,int result[]);
int socketConnect(char *serverName,int portNumber);
int socketConnectWithPass(char *serverName,int portNumber,char *pass);
int errorOxmisc2(char *s);  // defined in oxmisc2.c


/* ------------------------------------- */
extern FILE *MyErrorOut;

typedef struct CMO_Object {
  int tag;
  char data[1];
} CMO_Object ;

typedef struct CMO_string_object {
  int tag;
  int size;
  char data[1];
} CMO_string_object;

typedef struct CMO_int32_object {
  int tag;
  int n;
} CMO_int32_object ;

void printCMO_object(FILE *fp,CMO_Object *op);


/* --------------------------------------- */
#define DSTATE_ERROR -1
#define DSTATE_ANY  0
#define DSTATE_FIRST_SYNC 1
#define DSTATE_SECOND_SYNC 2
#define DSTATE_WAIT_OX_DATA 256

typedef  struct oxclient {
  ox_stream datafp2;
  int dataport;
  int controlfd;
  int controlport;
  int dstate;    /* state of data channel */
     /*  0 : any 
         1 : waiting the first OX_SYNC_BALL
	 2 : waiting the second OX_SYNC_BALL
        256: waiting the OX_DATA to stack.
     */
  int cstate;    /* state of control channel */
     /*  0 : any
	 1 : waiting resultOfControlInt32
     */
  int humanio;   /* 1: stdin,  0: not */
  int type;      /* 0: CLIENT_SOCKET socket, 1: CLIENT_FILE file */
  int id;  /* client identifier. */
  void *mathcapObjp;  /* NULL for the initial state.
			 NULL means no restriction.*/
  int engineByteOrder;
  int controlByteOrder;
  int engineID;
} oxclient;
#define N_OF_CLIENT_FIELDS  14    /* If you add new field to struct oxclient
				     do not forget to increase the number.*/
/* Change also, oxInitClient, oxCreateClient(2), oxCreateClientFile    oxmisc.c
   oxClientToObject, oxObjectToClient oxmisc2.c
   */
typedef oxclient * oxclientp;

#define AbortIfRFC_101(oxclient) if (oxclient->controlport < 0) {\
  errorOxmisc2("The OpenXM RFC 101 client is used for RFC 100\n"); }

#define CLIENT_SOCKET 0
#define CLIENT_FILE   1

int oxGetResultOfControlInt32(int fd);
int oxclientMultiSelect(oxclientp clients[],int dataready[],int controlready[],
			int size, int t);
int oxInitClient(oxclientp c);
int oxGetControl(oxclientp client);
int oxIsThereErrorClient(oxclientp client);  /* 1 : error, 0 : OK. */
oxclientp oxCreateClient(char *ipname,int portStream,int portControl,
                         char *passControl,char *passData);
oxclientp oxCreateClientFile(char *fileName,char *mode,char *controlFileName,char *cmode);
oxclientp oxCreateClient2(int fdstream,int portStream,
			  int fdcontrol,int portControl,int ipmask,char *cpass,char *dpass);
int oxSetByteOrder(int fd);
int oxTellMyByteOrder(int fdOut,int fdIn);

char *oxGenPass(void);


/* -------------------------------------- */
int readOneByte(int fd);   /* blocking */
int oxfdGetInt32(int fd);
int oxGetInt32(ox_stream ostream);
int oxfdGetOXheader(int fd,int *serial);
int oxGetOXheader(ox_stream ostream, int *serial);
int oxGetCMOInt32(ox_stream ostream);
char *oxGetCMOString(ox_stream ostream);

void oxfdSendInt32(int fd,int k);
void oxfdSendOXheader(int fd,int k,int serial);
void oxfdSendCmoNull(int fd);
void oxfdSendCmoInt32(int fd,int k);
void oxfdSendCmoString(int fd,char *s);
void oxfdSendSyncBall(int fd);

void oxSendInt32(ox_stream ostream,int k);
void oxSendOXheader(ox_stream ostream,int k,int serial);
void oxSendCmoNull(ox_stream ostream);
void oxSendCmoError(ox_stream ostream);
void oxSendCmoError2(ox_stream ostream,char *s);
void oxSendCmoInt32(ox_stream ostream,int k);
void oxSendCmoString(ox_stream ostream,char *s);
void oxSendMathCap(ox_stream ostream,struct mathCap *mathcap);
void oxPushMathCap(struct mathCap *mathcap);


void oxSendSyncBall(ox_stream ostream);
int  oxWaitSyncBall(ox_stream ostream);

void oxSendResultOfControl(int fd);
void oxSendResultOfControlInt32(int fd, int k);


void oxReqMathCap(ox_stream os);
void oxReqSetMathCap(ox_stream os,struct mathCap *mathcap);
void oxReqPops(ox_stream os,int n);
void oxReqSetName(ox_stream os,char *name);
void oxReqEvalName(ox_stream os,char *name);
void oxReqExecuteStringByLocalParser(ox_stream os,char *s);
void oxReqExecuteFunction(ox_stream os,char *s);
void oxReqExecuteFunctionWithOptionalArgument(ox_stream os,char *s);
void oxReqPopString(ox_stream os);
void oxReqPushString(ox_stream os,char *s);
void oxReqPopCMO(ox_stream os);
void oxReqSingleOperand(ox_stream os,int sm_operator_tag);

void oxReqControlResetConnection(int fd);
void oxReqControlKill(int fd);

int oxReadPortFile(int func,char *fname);  /* 0 : .control, 1 : .data */
int oxWritePortFile(int func,int port,char *fname);
char *oxGenPortFile(void);
int oxRemovePortFile(void);

#define MAX_N_OF_CLIENT 1024
int oxGetClientID();

char *oxFIDtoStr(int id);
