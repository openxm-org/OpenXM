/* $OpenXM$ */
/* kancmo.h */
#define Kan_ExecuteString(s) KSexecuteString(s)
#define Kan_Start()          KSstart()
#define Kan_PopString()      KSpopString()
#define Kan_Set(name)        KSset(name)

int KSexecuteString(char *s);
void KSstart();
char *KSpopString();
int KSset (char *name);


