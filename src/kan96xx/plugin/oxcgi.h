/* $OpenXM: OpenXM/src/kan96xx/plugin/oxcgi.h,v 1.3 2004/09/27 01:20:46 takayama Exp $ */

/* ----- data conversion : urlEncoding <---> keyValue pair */
struct object cgiUrlEncodingToKeyValuePair(char *s);
char *cgiKeyValuePairToUrlEncoding(struct object ob);
struct object cgiKeyValuePairToUrlEncodingString(struct object ob);
int cgiKeyValuePairToUrlEncodingFile2(struct object ob,FILE2 *fp);
char *byteArrayToUrlEncoding(unsigned char *s,int size);
struct object urlEncodedStringToObj(char *s,int vstart,int vend,int mode);

struct object KooStringToUrlEncoding(struct object sob);
struct object KooUrlEncodedStringToObj(struct object sob);

/* ------- HTML header <---> keyValue pair  */
struct object cgiHttpToKeyValuePair(char *s,int size);
char *cgiKeyValuePairToHttp(struct object ob,int *sizep);
struct object cgiKeyValuePairToHttpString(struct object ob);
int cgiKeyValuePairToHttpFile2(struct object ob, FILE2 *fp);

/* ------- misc ------------------- */
int checkKeyValuePairFormat(struct object ob,char *msg);

struct object KooToTokens(struct object ob,struct object sep);
