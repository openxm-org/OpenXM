/* $OpenXM$ */
/* include from kan */
#include "datatype.h"
#include "stackm.h"
#include "extern.h"
#include "kclass.h"

struct object cmoObjectFromStream2(FILE2 *fp);
struct object cmoObjectToStream2(struct object ob,FILE2 *fp);

int cmoCheckMathCap(struct object ob, struct object *mathcapObjp);
struct object newMathCap(void *mathcapp);

int oxReq(oxclientp client, int func, struct object ob);
int oxGet(oxclientp client,struct object *op,int *isObj);
int oxGetFromControl(oxclientp client);

/* ------------  to ox.hh ------------- */
int oxClientToObject(oxclientp client,struct object ob);
int oxObjectToClient(struct object ob,oxclientp client);

#include "oxx.h"


