
#include <iostream.h>
#include <stdio.h>

extern "C" {
void GC_init(void);
void KSstart(void);
int KSexecuteString(char *s);
char *KSstringPop(void);
}
    
main() {
  char a[256];
  char *s;
  int result;
  GC_init();
  KSstart();

  while (1) {
    cout << "\n\n?";
    cin.getline(a,256);
    result = KSexecuteString(a);
    if (result != -1) {
      // If an error did not occured, pop the top
      //  element from the stack of kan/sm1
      KSexecuteString("  toString ");
      s = KSstringPop();
      if (s != (char *)NULL) {
	cout << s << "\n";
      }
    }
  }
}

