/* $OpenXM$ */
else if (strcmp(key,"Asir_ExecuteString") == 0) {
  if (size != 2) errorKan1("%s\n","[(Asir_ExecuteString)  string] extension result.");
  obj1 = getoa(obj,1);
  if (obj1.tag != Sdollar) errorKan1("%s\n","[(Asir_ExecuteString)  string] extension result");
  rob = KpoInteger(Asir_ExecuteString(obj1.lc.str));
}
else if (strcmp(key,"Asir_PopString") == 0) {
  if (size != 1) errorKan1("%s\n","[(Asir_PopString)] extension result.");
  rob = KpoString(Asir_PopString());
}
else if (strcmp(key,"Asir_Start") == 0) {
  if (size != 1) errorKan1("%s\n","[(Asir_Start)] extension result.");
  rob = KpoInteger(Asir_Start());
}
else if (strcmp(key,"Asir_to_kan") == 0) {
  if (size != 2) errorKan1("%s\n","[(Asir_to_kan)  string] extension result.");
  obj1 = getoa(obj,1);
  if (obj1.tag != Sdollar) errorKan1("%s\n","[(Asir_to_kan)  string] extension result");
  rob = KpoString(KasirKanConvert(obj1.lc.str));
}
else if (strcmp(key,"Asir_Set") == 0) {
  if (size != 2) errorKan1("%s\n","[(Asir_Set)  string] extension result.");
  obj1 = getoa(obj,1);
  if (obj1.tag != Sdollar) errorKan1("%s\n","[(Asir_Set)  string] extension result");
  rob = KpoInteger(Asir_Set(obj1.lc.str));
}
else if (strcmp(key,"Asir_PushBinary") == 0) {
  if (size != 2) errorKan1("%s\n","[(Asir_PushCmo) cmo-obj] extension result.");
  obj1 = getoa(obj,1);
  if (obj1.tag != CMO) {
    errorKan1("%s\n","Argument must be cmo-object.");
  }
  rob=KpoInteger(Asir_PushBinary(((struct cmoBuffer *)(obj1.lc.voidp))->size,
                                 (((struct cmoBuffer *)(obj1.lc.voidp))->buf)));
}
else if (strcmp(key,"Asir_PopBinary") == 0) {
  if (size != 1) errorKan1("%s\n","[(Asir_PopCmo)] extension cmo-obj.");
  rob.tag =CMO;
  rob.lc.voidp = GC_malloc(sizeof(struct cmoBuffer));
  if (rob.lc.voidp == NULL) {
    fprintf(stderr,"No more memory.");
  }
  ((struct cmoBuffer *)(rob.lc.voidp))->buf = Asir_PopBinary(&size);
  ((struct cmoBuffer *)(rob.lc.voidp))->size = size;
  ((struct cmoBuffer *)(rob.lc.voidp))->pos = size;
  ((struct cmoBuffer *)(rob.lc.voidp))->rpos = 0;
}




