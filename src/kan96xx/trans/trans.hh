/* $OpenXM$ */
else if (strcmp(key,"treeToPolymake") == 0) {
  if (size != 2) errorKan1("%s\n","[(treeToPolymake)  obj] extension stro.");
  obj1 = getoa(obj,1);
  if (obj1.tag != Sclass) {
    errorKan1("%s\n","[(treeToPolymake) obj] extension stro.");
  }
  rob = KtreeToPolymakeS(obj1);
}
