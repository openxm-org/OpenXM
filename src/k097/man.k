/* $OpenXM$ */

def man(a) {
  local lang,n,m,i,s,astarisk;
  n = Length(Arglist);
  lang = GetEnv("LANG");
  astarisk = 42;
  if (n >= 1) {
    s = StringToAsciiArray(a);
    m = Length(s);
    for (i=0; i<m; i++) {
      if (s[i] == astarisk) {
        n = 0;
      }
    }
  } else {
    a = "*";
  }
  if (lang == "C" || Length(lang) == 0) {
    if (n < 1) {
      sm1(" [(cd $OpenXM_HOME/lib/k097/help/help-en/ ; ls -C ) a ( | more)] cat system ");
    }else {
      sm1(" [(more $OpenXM_HOME/lib/k097/help/help-en/) a] cat system ");
    }
  }else{
    if (n < 1) {
      sm1(" [(cd  $OpenXM_HOME/lib/k097/help/help-ja/ ; ls -C ) a ( | more)] cat system ");
    }else {
      sm1(" [(jless $OpenXM_HOME/lib/k097/help/help-ja/) a] cat system ");
    }
  }
}

