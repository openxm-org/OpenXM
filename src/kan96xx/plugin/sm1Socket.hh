  else if (strcmp(key,"sm1.socket") == 0) {
    if (size != 3)  {
      errorKan1("%s\n","[(sm1.socket) (key for sm1.socket) [ args ]] extension");
    }
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

