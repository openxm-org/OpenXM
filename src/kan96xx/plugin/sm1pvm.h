/* $OpenXM$ */
int KpvmStartSlaves(char *name,int n);
int KpvmStopSlaves(void);
int KpvmChangeStateOfSlaves(int k);
int KpvmMcast(char *comm);
struct object KpvmJobPool(struct object obj);
