/* $OpenXM: OpenXM/src/kan96xx/Kan/Kclass/tree.hh,v 1.1 2003/11/24 23:12:28 takayama Exp $ */
else if (strcmp(key,"treeGetDocumentElement") == 0) {
  if (size != 2) errorKan1("%s\n","[(treeGetDocumentElement)  treeobj] extension strobj.");
  obj1 = getoa(obj,1);
  if (obj1.tag != Sclass) {
    errorKan1("%s\n","[(treeGetDocumentElement) treeobj] extension strobj.");
  }
  if (ectag(obj1) != CLASSNAME_tree) {
    errorKan1("%s\n","getDocumentElement: the argument must be a tree object.");
  }
  rob = KtreeGetDocumentElement(obj1);
}
else if (strcmp(key,"treeGetAttributes") == 0) {
  if (size != 2) errorKan1("%s\n","[(treeGetAttributes)  treeobj] extension strobj.");
  obj1 = getoa(obj,1);
  if (obj1.tag != Sclass) {
    errorKan1("%s\n","[(treeGetAttributes) treeobj] extension strobj.");
  }
  if (ectag(obj1) != CLASSNAME_tree) {
    errorKan1("%s\n","getAttributes: the argument must be a tree object.");
  }
  rob = KtreeGetAttributes(obj1);
}
else if (strcmp(key,"treeGetChildNodes") == 0) {
  if (size != 2) errorKan1("%s\n","[(treeGetChildNodes)  treeobj] extension strobj.");
  obj1 = getoa(obj,1);
  if (obj1.tag != Sclass) {
    errorKan1("%s\n","[(treeGetChildNodes) treeobj] extension strobj.");
  }
  if (ectag(obj1) != CLASSNAME_tree) {
    errorKan1("%s\n","getChildNodes: the argument must be a tree object.");
  }
  rob = KtreeGetChildNodes(obj1);
}
else if (strcmp(key,"treeCreateElement") == 0) {
  if (size != 2) errorKan1("%s\n","[(treeCreateElement) strobj] extension treeobj.");
  obj1 = getoa(obj,1);
  if (obj1.tag != Sdollar) {
    errorKan1("%s\n","[(treeCreateElement) strobj] extension treeobj.");
  }
  rob = KtreeCreateElement(obj1);
}
