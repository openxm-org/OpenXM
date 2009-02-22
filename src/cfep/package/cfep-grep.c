#include <stdio.h>
#include "ox_pathfinder.h"

/*
  cfep-grep --inHex key   (key や folder 名等すべて 16進数, utf8 で) 
  cfep-grep --grep key    (grep のみ)
  cfep-grep --mdfind key  (mdfind のみ. spotlight)
  cfep-grep --mdfind --pdf key  (mdfind, kind:pdf のみ. spotlight)
  cfep-grep --folder folderName (default folder 以外の検索)

  grep の default folder は  プログラム中に 定数文字列の配列として埋め込んである.
    ${OpenXM_HOME}/doc/cfep/html-ja_JP.eucJP/html-ja
    ${OpenXM_HOME}/doc/cfep/html-ja_JP.eucJP/html-exp-ja
  spotlight の default folder は
    ${OpenXM_HOME}/doc/cfep

  出力は utf8 の html ファイル.
   google ツールバーがついてるといいね. cite: は OpenXM と RA journal.
*/

main(int argc, char *argv[]) {

}
