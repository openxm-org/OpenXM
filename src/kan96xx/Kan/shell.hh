/* $OpenXM$ */
else if (strcmp(key,"shell:which") == 0) {
  rob = NullObject;
  if (size == 2) {
	obj1 = getoa(obj,1);
	if (obj1.tag != Sdollar) errorKan1("%s\n","[(shell:which) cmdname] extension");
	rob = KoxWhich(obj1,KpoInteger(0));
  }else if (size == 3) {
	obj1 = getoa(obj,1);
	if (obj1.tag != Sdollar) errorKan1("%s\n","[(shell:which) cmdname] extension");
	obj2 = getoa(obj,2);
	if (obj2.tag != Sdollar) errorKan1("%s\n","[(shell:which) cmdname path] extension");
	obj1=getoa(obj,1); obj2=getoa(obj,2);
	rob = KoxWhich(obj1,obj2);
  }else{
	errorKan1("%s\n","[(shell:which) cmdname] extension or [(shell:which) cmdnamd path] extension ");
  }
}
