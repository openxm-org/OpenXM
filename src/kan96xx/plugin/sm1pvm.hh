  else if (strcmp(key,"start slaves")==0) {
    if (size != 3)
      errorKan1("%s\n","[(start slaves) (slave name) n] extension");
    obj1 = getoa(obj,1);
    obj2 = getoa(obj,2);
    if (obj1.tag != Sdollar || obj2.tag != Sinteger)
      errorKan1("%s\n","[(start slaves) (slave name) n] extension");
    if (KpvmStartSlaves(obj1.lc.str,obj2.lc.ival)<0)
      errorKan1("%s\n","Failed to start slaves.");
  }else if (strcmp(key,"stop slaves")==0) {
    if (KpvmStopSlaves() < 0)
      errorKan1("%s\n","Failed to stop slaves.");
  }else if (strcmp(key,"mcast") == 0) {
    if (size != 2)
      errorKan1("%s\n","[(mcast) (command)] extension");
    obj1 = getoa(obj,1);
    if (obj1.tag != Sdollar)
      errorKan1("%s\n","[(mcast) (command)] extension");
    if (KpvmMcast(obj1.lc.str)<0)
      errorKan1("%s\n","Failed to mcast to slaves.");
  }else if (strcmp(key,"job pool") == 0) {
    if (size != 2)
      errorKan1("%s\n","[(job pool) list] extension");
    obj1 = getoa(obj,1);
    if (obj1.tag != Sarray)
      errorKan1("%s\n","[(job pool) list] extension. Argumet must be an array.");
    m = getoaSize(obj1);
    for (i=0; i<m; i++) {
      obj2 = getoa(obj1,i);
      if (obj2.tag != Sdollar) {
	errorKan1("%s\n","[(job pool) list] extension. Elements must be strings.");
      }
    }
    rob = KpvmJobPool(obj1);
  }else if (strcmp(key,"change slaves") == 0) {
    if (KpvmChangeStateOfSlaves(0)<0) {
      errorKan1("%s\n","Failed to change the state of slaves.");
    }
  }
