/*$OpenXM: OpenXM/src/kan96xx/plugin/oxx.hh,v 1.5 2000/12/03 08:19:55 takayama Exp $*/
else if (strcmp(key,"oxCreateClient") == 0) {
  if (size != 4) errorKan1("%s\n","[(oxCreateClient) ip dataport controlport] extension client.");
  if (SecureMode) errorKan1("%s\n","Security violation for oxCreateClient.");
  rob = KoxCreateClient(getoa(obj,1),getoa(obj,2),getoa(obj,3));
}
else if (strcmp(key,"oxCreateClientFile") == 0) {
  if (size != 3) errorKan1("%s\n","[(oxCreateClientFile) fname mode] extension client.");
  if (SecureMode) errorKan1("%s\n","Security violation for oxCreateClientFile.");
  rob = KoxCreateClient(getoa(obj,1),getoa(obj,2),getoa(obj,2));
                                                 /* dummy */
}
else if (strcmp(key,"oxReq") == 0) {
  if (size == 4) {
    lockCtrlCForOx();
    rob = KoxReq(getoa(obj,1),getoa(obj,2),getoa(obj,3));
    unlockCtrlCForOx();
  }else if (size == 3) {
    lockCtrlCForOx();
    rob = KoxReq(getoa(obj,1),getoa(obj,2),rob); /* rob is a dummy. */
    unlockCtrlCForOx();
  }else {
    errorKan1("%s\n","[(oxReq) client func arg] extension result.");
  }
}
else if (strcmp(key,"oxGet") == 0) {
  if (size != 2) errorKan1("%s\n","[(oxGet)  client] extension obj.");
  lockCtrlCForOx();
  obj1 = getoa(obj,1);
  rob = KoxGet(obj1);  /* Note that lock and unlock are also temporary done in
			  oxGetHeader(). */
  unlockCtrlCForOx();
}
else if (strcmp(key,"oxGetFromControl") == 0) {
  if (size != 2) errorKan1("%s\n","[(oxGetFromControl)  client] extension obj.");
  obj1 = getoa(obj,1);
  rob = KoxGetFromControl(obj1);
}
else if (strcmp(key,"oxMultiSelect") == 0) {
  if (size != 3) errorKan1("%s\n","[(oxMultiSelect) client t] extension obj.");
  rob = KoxMultiSelect(getoa(obj,1),getoa(obj,2));
}
else if (strcmp(key,"oxWatch") == 0) {
  if (size > 2) errorKan1("%s\n","[(oxWatch) client] extension obj");
  if (size == 2) {
	rob = KoxWatch(getoa(obj,1),rob); /* rob is dummy for now. */
  }else {
	rob = KoxWatch(KpoInteger(1),rob);
  }
}
else if (strcmp(key,"oxCloseClient") == 0) {
  if (size != 2) errorKan1("%s\n","[(oxCloseClient) client] extension obj");
  rob = KoxCloseClient(getoa(obj,1));
}
else if (strcmp(key,"oxSerial") == 0) {
  if (size != 1) errorKan1("%s\n","[(oxSerial)] extension obj");
  rob = KpoInteger(SerialOX);
}
else if (strcmp(key,"oxGenPortFile") == 0) {
  if (size != 1) errorKan1("%s\n","[(oxGenPortFile)] extension obj");
  rob = KoxGenPortFile();
}
else if (strcmp(key,"oxRemovePortFile") == 0) {
  if (size != 1) errorKan1("%s\n","[(oxRemovePortFile)] extension obj");
  KoxRemovePortFile();
}
else if (strcmp(key,"oxGenPass") == 0) {
  if (size != 1) errorKan1("%s\n","[(oxGenPass)] extension obj");
  rob = KoxGenPass();
}
else if (strcmp(key,"oxGetPort") == 0) {
  if (size != 2) errorKan1("%s\n","[(oxGetPort) host] extension obj");
  if (SecureMode) errorKan1("%s\n","Security violation for oxGetPort.");
  rob = KoxGetPort(getoa(obj,1));
}
else if (strcmp(key,"oxGetPort1") == 0) {
  if (size != 2) errorKan1("%s\n","[(oxGetPort1) host] extension obj");
  if (SecureMode) errorKan1("%s\n","Security violation for oxGetPort.");
  rob = KoxGetPort(getoa(obj,1));
}
else if (strcmp(key,"oxCreateClient2") == 0) {
  if (size != 4) errorKan1("%s\n","[(oxCreateClient2) peer ipmask pass] extension client.");
  if (SecureMode) errorKan1("%s\n","Security violation for oxCreateClient2.");
  rob = KoxCreateClient2(getoa(obj,1),getoa(obj,2),getoa(obj,3));
}
else if (strcmp(key,"oxPrintMessage") == 0) {
  if (size == 1) {
    rob = KpoInteger(OXprintMessage);
  }else{
    if (size != 2) errorKan1("%s\n","[(oxPrintMessge) level] extension obj");
    obj1 = getoa(obj,1);
    if (obj1.tag != Sinteger)
      errorKan1("%s\n","[(oxPrintMessge) level] extension obj, integer level");
    OXprintMessage = KopInteger(obj1);
    rob = KpoInteger(OXprintMessage);
  }
}
else if (strcmp(key,"oxPrintMessage") == 0) {
  if (size == 1) {
    rob = KpoInteger(OXprintMessage);
  }else{
    if (size != 2) errorKan1("%s\n","[(oxPrintMessge) level] extension obj");
    obj1 = getoa(obj,1);
    if (obj1.tag != Sinteger)
      errorKan1("%s\n","[(oxPrintMessge) level] extension obj, integer level");
    OXprintMessage = KopInteger(obj1);
    rob = KpoInteger(OXprintMessage);
  }
}
else if (strcmp(key,"oxPushCMD") == 0) {
  if (size == 3) {
    lockCtrlCForOx();
    rob = KoxPushCMD(getoa(obj,1),getoa(obj,2)); 
    unlockCtrlCForOx();
  }else {
    errorKan1("%s\n","[(oxPushCMD) client SM_id] extension result.");
  }
}
else if (strcmp(key,"oxPushCMO") == 0) {
  if (size == 3) {
    lockCtrlCForOx();
    rob = KoxPushCMO(getoa(obj,1),getoa(obj,2));
    unlockCtrlCForOx();
  }else {
    errorKan1("%s\n","[(oxPushCMO) client obj] extension result.");
  }
}
else if (strcmp(key,"oxCreateControl_RFC_101") == 0) {
  if (size != 4) errorKan1("%s\n","[(oxCreateControl_RFC_101) peer ipmask pass] extension client.");
  if (SecureMode) errorKan1("%s\n","Security violation for oxCreateControl_RFC_101.");
  rob = KoxCreateControl_RFC_101(getoa(obj,1),getoa(obj,2),getoa(obj,3));
}


