/*
**    copyright (c) 1995  Birk Huber
*/

/* get user time
 * man 2 getrusage
 */

#include <math.h>
#include <stdio.h>
#include <time.h>
#include "utime.h"



int set_mark(){
  return clock();
}

int read_mark(int timeset){
 return (clock()-timeset) / CLOCKS_PER_SEC;
}





