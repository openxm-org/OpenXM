struct object KoxCreateClient(struct object ip,struct object portStream,
			      struct object portControl);
struct object KoxCloseClient(struct object client);
struct object KoxIsThereErrorClient(struct object ob);
struct object KoxReq(struct object client,struct object func,
		     struct object ob1);
struct object KoxGet(struct object client);
struct object KoxGetFromControl(struct object client);
struct object KoxMultiSelect(struct object clients,struct object t);
struct object KoxWatch(struct object client,struct object f);
extern int SerialOX;
struct object KoxGenPortFile(void);
void KoxRemovePortFile(void);
struct object KoxGetPort(struct object host);
struct object KoxCreateClient2(struct object peer,struct object ipmask,struct object pass);
struct object KoxGenPass(void);

