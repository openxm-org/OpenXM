/* $OpenXM: OpenXM/src/kan96xx/plugin/oxcgi.hh,v 1.1 2004/09/21 12:52:01 takayama Exp $ */
else if (strcmp(key,"cgiUrlEncodingToKeyValuePair") == 0) {
  if (size != 2) errorKan1("%s\n","[(cgiUrlEncodingToKeyValuePair)  obj] extension kv-pair.");
  obj1 = getoa(obj,1);
  if (obj1.tag != Sdollar) errorKan1("%s\n","cgiUrlEncodingTOKeyValuePair: argument must be a string.");
  rob = cgiUrlEncodingToKeyValuePair(KopString(obj1));
}
else if (strcmp(key,"cgiKeyValuePairToHttpString") == 0) {
  if (size != 2) errorKan1("%s\n","[(cgiKeyValuePairToHttpString) kv-pair] extension sobj");
  obj1= getoa(obj,1);
  rob = cgiKeyValuePairToHttpString(obj1);
}
else if (strcmp(key,"cgiKeyValuePairToUrlEncodingString") == 0) {
  if (size != 2) errorKan1("%s\n","[(cgiKeyValuePairToUrlEncodingString)  kv-pair] extension sobj.");
  obj1 = getoa(obj,1);
  rob = cgiKeyValuePairToUrlEncodingString(obj1);
}
else if (strcmp(key,"cgiHttpToKeyValuePair") == 0) {
  if (size != 2) errorKan1("%s\n","[(cgiHttpToKeyValuePair) sobj] extension kv-pair");
  obj1= getoa(obj,1);
  if (obj1.tag == Sdollar) {
	  rob = cgiHttpToKeyValuePair(KopString(obj1),strlen(KopString(obj1)));
  }else if (obj1.tag == SbyteArray) {
	  rob = cgiHttpToKeyValuePair(KopByteArray(obj1),getByteArraySize(obj1));
  }else errorKan1("%s\n","cgiHttpToKeyValuePair: argument must be a string or a byte aray.");
}
