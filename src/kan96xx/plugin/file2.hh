/* $OpenXM: OpenXM/src/kan96xx/plugin/file2.hh,v 1.4 2001/08/12 03:13:36 takayama Exp $ */
else if (strcmp(key,"fp2fdopen") == 0) {
  if (size != 2) errorKan1("%s\n","[(fp2fdopen)  obj] extension obj-fp2.");
  if (SecureMode) errorKan1("%s\n","Security violation for fp2fdopen.");
  obj1 = getoa(obj,1);
  if (obj1.tag != Sinteger) {
    errorKan1("%s\n","[(fp2fdopen)  file-descriptor-integer] extension obj-fp2.");
  }
  rob.tag = Sfile;
  rob.lc.str = MAGIC2 ;
  rob.rc.voidp = (void *) fp2open(KopInteger(obj1));
  
}
else if (strcmp(key,"fp2fflush") == 0) {
  if (size != 2) errorKan1("%s\n","[(fp2fflush) obj-fp2] extension 0.");
  obj1= getoa(obj,1);
  if (obj1.tag != Sfile) {
    errorKan1("%s\n","[(fp2fflush)  obj-fp2] extension 0.");
  }
  if (strcmp(obj1.lc.str,MAGIC2) != 0) {
    errorKan1("%s\n","[(fp2fflush)  obj-fp2] extension 0. obj-fp2 is not Sfile (FILE2).");
  }
  fp2fflush((FILE2 *) obj1.rc.voidp);
  rob = KpoInteger(0);
}
else if (strcmp(key,"fp2fclose") == 0) {
  if (size != 2) errorKan1("%s\n","[(fp2fclose) obj-fp2] extension r.");
  obj1= getoa(obj,1);
  if (obj1.tag != Sfile) {
    errorKan1("%s\n","[(fp2fclose)  obj-fp2] extension r.");
  }
  if (strcmp(obj1.lc.str,MAGIC2) != 0) {
    errorKan1("%s\n","[(fp2fclose)  obj-fp2] extension r. obj-fp2 is not Sfile (FILE2).");
  }
  rob = KpoInteger(fp2fclose((FILE2 *) obj1.rc.voidp));
}
else if (strcmp(key,"fp2fgetc") == 0) {
  if (size != 2) errorKan1("%s\n","[(fp2fgetc) obj-fp2] extension c.");
  obj1= getoa(obj,1);
  if (obj1.tag != Sfile) {
    errorKan1("%s\n","[(fp2fgetc)  obj-fp2] extension c.");
  }
  if (strcmp(obj1.lc.str,MAGIC2) != 0) {
    errorKan1("%s\n","[(fp2fgetc)  obj-fp2] extension c. obj-fp2 is not Sfile (FILE2).");
  }
  rob = KpoInteger(fp2fgetc((FILE2 *) obj1.rc.voidp));
}
else if (strcmp(key,"fp2fputc") == 0) {
  if (size != 3) errorKan1("%s\n","[(fp2fgetc) c obj-fp2] extension c.");
  obj1 = getoa(obj,1);
  if (obj1.tag != Sinteger) {
    errorKan1("%s\n","[(fp2fputc)  c-integer obj-fp2] extension c.");
  }
  obj2= getoa(obj,2);
  if (obj2.tag != Sfile) {
    errorKan1("%s\n","[(fp2fputc)  c obj-file-fp2] extension c.");
  }
  if (strcmp(obj2.lc.str,MAGIC2) != 0) {
    errorKan1("%s\n","[(fp2fputc)  c obj-fp2] extension c. obj-fp2 is not Sfile (FILE2).");
  }
  rob = KpoInteger(fp2fputc(obj1.lc.ival,(FILE2 *) obj2.rc.voidp));
}
else if (strcmp(key,"fp2dumpBuffer") == 0) {
  if (size != 2) errorKan1("%s\n","[(fp2dumpBuffer) obj-fp2] extension c.");
  obj1= getoa(obj,1);
  if (obj1.tag != Sfile) {
    errorKan1("%s\n","[(fp2dumpBuffer)  obj-fp2] extension c.");
  }
  if (strcmp(obj1.lc.str,MAGIC2) != 0) {
    errorKan1("%s\n","[(fp2dumpBuffer)  obj-fp2] extension c. obj-fp2 is not Sfile (FILE2).");
  }
  rob = KpoInteger(fp2dumpBuffer((FILE2 *) obj1.rc.voidp));
}
else if (strcmp(key,"fp2openForRead") == 0) {
  if (size != 2) errorKan1("%s\n","[(fp2openForRead) name] extension c.");
  if (SecureMode) errorKan1("%s\n","Security violation for fp2openForRead.");
  obj1= getoa(obj,1);
  if (obj1.tag != Sdollar) {
    errorKan1("%s\n","[(fp2openForRead)  name-string] extension c.");
  }
  rob = KpoInteger(open(obj1.lc.str,O_RDONLY));
}
else if (strcmp(key,"fp2openForWrite") == 0) {
  if (size != 2) errorKan1("%s\n","[(fp2openForWrite) name] extension c.");
  if (SecureMode) errorKan1("%s\n","Security violation for fp2openForWrite.");
  obj1= getoa(obj,1);
  if (obj1.tag != Sdollar) {
    errorKan1("%s\n","[(fp2openForWrite)  name-string] extension c.");
  }
  rob = KpoInteger(creat(obj1.lc.str, S_IREAD|S_IWRITE|S_IRGRP|S_IROTH));
}
else if (strcmp(key,"fp2close") == 0) {
  if (size != 2) errorKan1("%s\n","[(fp2close) fd] extension c.");
  obj1= getoa(obj,1);
  if (obj1.tag != Sinteger) {
    errorKan1("%s\n","[(fp2close)  fd-integer] extension c.");
  }
  rob = KpoInteger(close(obj1.lc.ival));
}
else if (strcmp(key,"fp2clearReadBuf") == 0) {
  if (size != 2) errorKan1("%s\n","[(fp2ClearReadBuf) obj-fp2] extension c.");
  obj1= getoa(obj,1);
  if (obj1.tag != Sfile) {
    errorKan1("%s\n","[(fp2clearReadBuf)  obj-fp2] extension c.");
  }
  if (strcmp(obj1.lc.str,MAGIC2) != 0) {
    errorKan1("%s\n","[(fp2clearReadBuf)  obj-fp2] extension c. obj-fp2 is not Sfile (FILE2).");
  }
  rob = KpoInteger(fp2dumpBuffer((FILE2 *) obj1.rc.voidp));
  rob = KpoInteger(fp2clearReadBuf((FILE2 *)obj1.rc.voidp));
}
else if (strcmp(key,"fp2fcloseInString") == 0) {
  if (size != 2) errorKan1("%s\n","[(fp2closeInString) obj-fp2] extension s.");
  obj1= getoa(obj,1);
  if (obj1.tag != Sfile) {
    errorKan1("%s\n","[(fp2fcloseInString)  obj-fp2] extension s.");
  }
  if (strcmp(obj1.lc.str,MAGIC2) != 0) {
    errorKan1("%s\n","[(fp2fcloseInString)  obj-fp2] extension s. obj-fp2 is not Sfile (FILE2).");
  }
  {
    int mysize;
    rob = KpoString(fp2fcloseInString((FILE2 *) obj1.rc.voidp,&mysize));
  }
}
else if (strcmp(key,"fp2pushfile") == 0) {
  if (size != 2) errorKan1("%s\n","[(fp2pushfile) name] extension array-of-int.");
  if (SecureMode) errorKan1("%s\n","Security violation for fp2pushfile.");
  obj1= getoa(obj,1);
  if (obj1.tag != Sdollar) {
    errorKan1("%s\n","[(fp2pushfile)  name] extension array-of-int.");
  }
  {
    FILE *fp;
    int n,i,c;
    fp = fopen(obj1.lc.str,"r");
    if (fp == NULL) errorKan1("%s\n","fp2pushfile : file not found.");
    n = 0;
    while ((c = fgetc(fp)) != EOF) n++;
    fclose(fp);
    fp = fopen(obj1.lc.str,"r");
    rob = newObjectArray(n);
    i = 0;
    while ((c = fgetc(fp)) != EOF) {
      putoa(rob,i,KpoInteger(c));
      i++;
    }
    fclose(fp);
  }
}
  




