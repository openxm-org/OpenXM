else if (strcmp(key,"cmoObjectToCmo") == 0) {
  if (size != 2) errorKan1("%s\n","[(cmoObjectToCmo)  obj] extension cmo-obj.");
  obj1 = getoa(obj,1);
  rob = cmoObjectToCmo(obj1);
}
else if (strcmp(key,"cmoDumpCmo") == 0) {
  if (size != 2) errorKan1("%s\n","[(cmoDumpCmo) cmo-obj] extension 0.");
  cmoDumpCmo(getoa(obj,1));
  rob = KpoInteger(0);
}
else if (strcmp(key,"cmoCmoToObject") == 0) {
  if (size != 2) errorKan1("%s\n","[(CmoToObject)  cmo-obj] extension obj.");
  obj1 = getoa(obj,1);
  rob = cmoCmoToObject(obj1);
}
else if (strcmp(key,"cmoCmoToStream") == 0) {
  if (size != 3) errorKan1("%s\n","[(cmoCmoToStream)  cmo-obj stream] extension obj.");
  obj1 = getoa(obj,1);
  obj2 = getoa(obj,2);
  cmoToStream(obj1,obj2);
  rob = KpoInteger(0);
}
else if (strcmp(key,"cmoStreamToCmo") == 0) {
  if (size != 2) errorKan1("%s\n","[(cmoStreamToCmo)  stream] extension obj.");
  obj1 = getoa(obj,1);
  rob = streamToCmo(obj1);
}
else if (strcmp(key,"cmoToStream") == 0) {
  if (size != 3) errorKan1("%s\n","[(cmoToStream)  cmo-obj stream] extension obj.");
  obj1 = getoa(obj,1);
  obj2 = getoa(obj,2);
  rob = cmoObjectToStream(obj1,obj2);
}
else if (strcmp(key,"cmoFromStream") == 0) {
  if (size != 2) errorKan1("%s\n","[(cmoFromStream)  stream] extension obj.");
  obj1 = getoa(obj,1);
  rob = cmoObjectFromStream(obj1);
}
else if (strcmp(key,"cmoMathCap") == 0) {
  if (size != 1) errorKan1("%s\n","[(cmoMathCap)] extension obj.");
  rob = KSmathCap();
}
else if (strcmp(key,"cmoDebugCMO") == 0) {
  if (size != 2) errorKan1("%s\n","[(cmoDebugCMO) obj] extension cmo-obj.");
  obj1 = getoa(obj,1);
  if (obj1.tag != Sinteger) errorKan1("%s\n","[(cmoDebugCMO) integer] extension cmo-obj.");
  DebugCMO = KopInteger(obj1);
  rob = KpoInteger(DebugCMO);
}
else if (strcmp(key,"cmoOxSystemVersion") == 0) {
  extern char *OxSystemVersion;
  if (size != 2) errorKan1("%s\n","[(cmoOxSystemVersion)  string] extension obj.");
  obj1 = getoa(obj,1);
  if (obj1.tag != Sdollar) errorKan1("%s\n","OxSystemVersion must be given by a string.");
  OxSystemVersion = KopString(obj1);
  rob = obj1;
}
else if (strcmp(key,"cmoOxSystem") == 0) {
  extern char *OxSystem;
  if (size != 2) errorKan1("%s\n","[(cmoOxSystem)  string] extension obj. Set the OxSystemName for mathcap.");
  obj1 = getoa(obj,1);
  if (obj1.tag != Sdollar) errorKan1("%s\n","OxSystem must be given by a string.");
  OxSystem = KopString(obj1);
  rob = obj1;
}
else if (strcmp(key,"cmoLispLike") == 0) {
  extern int Lisplike;
  if (size != 2) errorKan1("%s\n","[(cmoLispLike)  int] extension obj. Output lisplike expressions for debugging when decoding cmo to an object.");
  obj1 = getoa(obj,1);
  if (obj1.tag != Sinteger) errorKan1("%s\n","Argument must be an integer.");
  Lisplike = KopInteger(obj1);
  rob = obj1;
}


