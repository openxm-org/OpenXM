/* $OpenXM$ */
int Asir_ExecuteString(char *s);
char *Asir_PopString(void);
char *KasirKanConvert(char *s);

int Asir_Set(char *name);
int Asir_PushBinary(int size,void *data);
void *Asir_PopBinary(int *sizep);

