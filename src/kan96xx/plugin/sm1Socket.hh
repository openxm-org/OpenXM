/* $OpenXM: OpenXM/src/kan96xx/plugin/sm1Socket.hh,v 1.2 2000/01/16 07:55:48 takayama Exp $ */
  else if (strcmp(key,"sm1.socket") == 0) {
    if (size != 3)  {
      errorKan1("%s\n","[(sm1.socket) (key for sm1.socket) [ args ]] extension");
    }
	if (SecureMode) errorKan1("%s\n","Security violation for sm1.socket.");
    obj1 = getoa(obj,1);
    obj2 = getoa(obj,2);
    if (obj1.tag != Sdollar) {
      errorKan1("%s\n","[(sm1.socket) (key for sm1.socket) [ args ]] extension : the second argument must be a keyword.");
    }
    if (obj2.tag != Sarray) {
      errorKan1("%s\n","[(sm1.socket) (key for sm1.socket) [ args ]] extension : the third argument must be an array of argument.");
    }
    rob = Kplugin_sm1Socket(KopString(obj1),obj2);
  }

