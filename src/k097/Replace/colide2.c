/*KSstart() {
  printf("Hello world from KSstart() of colide2.c .\n");
}
*/

/* linker は2重定義を許す。先に link された記号が有効となる。
 */


/* しかし、 library の中で呼ばれるべき関数を2重定義するとどうなるか? */
getokenSM() {
  printf("Getoken in colide2.c\n");
  exit(0);
}
/* どうやら安全のようだ。*/


