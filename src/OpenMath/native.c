#include <signal.h>
#include <unistd.h>
#include "OMproxy.h"

JNIEXPORT void JNICALL Java_OMproxy_setNoInterrupt(JNIEnv *env, jobject obj){
  signal(SIGINT, SIG_IGN);
  return;
}
