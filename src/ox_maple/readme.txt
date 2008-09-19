/* $OpenXM$ */


以下の環境変数の設定が必要
  setenv MAPLE maple-home-dir

コンパイルに必要な情報は以下のヘルプ参照
  OpenMaple/C/Examples | External Code Examples  

あらかじめ以下の準備が必要
  % cd $(OpenXM_HOME)/src/ox_toolkit
  % make && make install
  % cd $(OpenXM_HOME)/src/ox_ntl
  % make && make install

以下の環境で動作確認
 - Linux 2.6.18-6-686 i686 GNU/Linux



memo.

debian の xterm の仕様で LD_LIBRARY_PATH がわたらない
http://www.math.kobe-u.ac.jp/OpenXM/1.2.2/OpenXM-FAQ-ja.txt

FAQ には /lib とあるが, $OpenXM/lib がわたってきていた.

> cd $OpenXM/lib
> ln -s /tool/maple11/bin.IBM_INTEL_LINUX/libmaplec.so
> ln -s /tool/maple11/bin.IBM_INTEL_LINUX/libmaple.so
> ln -s /tool/maple11/bin.IBM_INTEL_LINUX/libprocessor32.so
> ln -s /tool/maple11/bin.IBM_INTEL_LINUX/libhf.so


