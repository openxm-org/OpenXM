/* size : success,  -1: error , -2 : do not use size field. */
int cmoFindHeader(struct cmoBuffer *cb)
{
  char data[4];
  int c;
  int mtag;
  int size;
  int count=0;
  data[0] = data[1] = data[2] = data[3] = 0xff;
  while (1) {
    if ((c = cmoGetIntFromBuf(CMOGETBYTE,cb)) < 0) {
      fprintf(stderr,"cmoFindHeader: unexpected EOF.\n");
      fprintf(stderr,"data=%2x,%2x,%2x,%2x\n",data[0],data[1],data[2],data[3]);
      /* errorCmo("cmoFindHeader: unexpected end of file."); */
      return(-1);
    }
    count++;
    data[0] = data[1];
    data[1] = data[2];
    data[2] = data[3];
    data[3] = (char)c;
    mtag = ntohl(*((int *)(data)));
    if (mtag == CMO_DATUM) {
      size = cmoGetIntFromBuf(CMOGET,cb);
      if (size == -1) {
	return(-2);
      }else if (size >= 0) {
	return(size);
      }else {
	return(-1);  /* Invalid size field. */
      }
    }else{
      if ((count>4) && (count <20)) {
	printf("cmoFindHeader: Warning: Illegal cmo format. Looking for a header.");
	printf(" %2x.\n",c); 
	if (count == 19) printf("Too many warnings. They are suppressed.\n");
      }else if (count >= 20) {
	count--;
      }
    }
  }
}

int cmoOutHeader(int size) {
  cmoOutRawInt(CMO_DATUM);
  if (size < 0) {
    size = -1;
  }
  cmoOutRawInt(size);
}

int cmoFindTail(struct cmoBuffer *cb)
{
  char data[4];
  int c;
  int mtag;
  int size;
  int count=0;
  data[0] = data[1] = data[2] = data[3] = 0xff;
  while (1) {
    if ((c = cmoGetIntFromBuf(CMOGETBYTE,cb)) < 0) {
      fprintf(stderr,"cmoFindHeader: unexpected EOF.\n");
      fprintf(stderr,"data=%2x,%2x,%2x,%2x\n",data[0],data[1],data[2],data[3]);
      /* errorCmo("cmoFindHeader: unexpected end of file."); */
      return(-1);
    }
    data[0] = data[1];
    data[1] = data[2];
    data[2] = data[3];
    data[3] = (char)c;
    count++;
    mtag = ntohl(*((int *)(data)));
    if (mtag == CMO_TAIL) {
      size = cmoGetIntFromBuf(CMOGET,cb);
      break;
    }else{
      if ((count > 4) && (count < 20)) {
	printf("cmoFindTail: Warning: Illegal cmo format. Looking for a tail.");
	printf(" %2x.\n",c);
	if (count == 19) printf("Too many warnings. They are suppressed.\n");
      }else if (count >= 20){
	count--;
      }
    }
  }
  /* read size signatures. */
  if (size > 0) {
    errorCmo("cmoFindTail:  digital signature has not been supported in this implementation.");
  }
  return(0);
}

int cmoOutTail(int size, void *signature)
{
  cmoOutRawInt(CMO_TAIL);
  if (size > 0) {
    cmoOutputToBuf(CMOPUT,signature,size);
  }else{
    cmoOutRawInt(0);
  }
  return(0);
}

