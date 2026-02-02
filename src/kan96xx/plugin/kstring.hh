/* This file is included in Kan/ext.c */

else if (strcmp(key,"string")==0) {
  if (size < 3) {
    errorKan1("%s\n","[(string) functionName arg1 ...] extension");
  }
  obj1 = getoa(obj,1);
  obj2 = getoa(obj,2);
  if (obj1.tag != Sdollar) {
    errorKan1("%s\n","[(string) functionNameByString ... ] extension");
  }
  if (strcmp(KopString(obj1),"remove")==0) {
    if (size != 4) {
      errorKan1("%s\n","[(string) (remove) input_str regular_expression] extension");
    }
    obj3 = getoa(obj,3);
    if ((obj2.tag != Sdollar) || (obj3.tag != Sdollar)) {
      errorKan1("%s\n","[(string) (remove) input_str regular_expression ] extension");
    }
    rob= Kstring_remove(obj2,obj3);
    return(rob);
  }else{
    errorKan1("%s\n","Unknown functionName for [(string) functionName ...] extension");
  }
}
