/* This file is included in Kan/ext.c */

else if (strcmp(key,"sample")==0) {
  if (size != 3) {
    errorKan1("%s\n","[(sample) op-code [args]] extension");
  }
  obj1 = getoa(obj,1);
  obj2 = getoa(obj,2);
  if (obj1.tag != Sinteger) {
    errorKan1("%s\n","[(sample) INTEGER ... ] extension");
  }
  if (obj2.tag != Sarray) {
    errorKan1("%s\n","[(sample) op-code ARRAY ] extension");
  }
  return(KpoInteger(Kplugin_sample(KopInteger(obj1),obj2)));
}
