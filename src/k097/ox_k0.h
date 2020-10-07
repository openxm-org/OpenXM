/* $OpenXM$ */
void KSstart(); // kan96xx/Kan/datatype.h
int KSexecuteString(char *s); // kan96xx/Kan/datatype.h
int Kan_pushCMOFromStream(FILE2 *fp); //kan96xx/Kan/plugin.h
int Kan_popCMOToStream(FILE2 *fp,int serial); //
int KgetCmoTagOfObject(struct object obj);    //
int KSstackPointer(void); //kan96xx/Kan/extern.h
int Kan_setMathCapToStream(FILE2 *fp,struct object ob); // kan96xx/plugin/cmo.c

void nullserver(int fdStreamIn,int fdStreamOut);
int nullserverCommand(ox_stream ostreamIn,ox_stream ostreamOut);
int nullserver_simplest(int fd);
void cancelAlarm(); // nullstackmachine.c
char *traceShowStack(); // kan96xx/Kan/extern.h
void traceClearStack(); // kan96xx/Kan/extern.h

void K0_start();
int K0_executeStringByLocalParser();


