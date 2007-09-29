開発 debug 環境.
   0. OpenXM は ${HOME}/OX4 の下にあると仮定している. cf. Makefile
   1. OpenXM/src で make install (debug install でも OK).
   2. make setup-cfep
     ( OpenXM home が build/debug 等の下へ link されるように)
   3. make install
      ( cfep 用ツールを OpenXM 以下 へインストール.)

配布版作成手順.
(参考, Xcode の利用法, video の記録. 2006年2月頃)

   1. OpenXM/src で make install (debug install でも OK).
   2. make -f Makefile-doc  usage に従い, tex 環境等を設定.
       (/usr/local/bin/ に Mac 用の ptex_package_2005v2.1.dmg でOK)
   3. make -f Makefile-doc clean
      make -f Makefile-doc build-utf8-asir
      ( OpenXM/doc/cfep に utf8 で html document を生成.
        OpenXM/src/asir-contrib/packages/doc で make install-html-doc 
        をもう一度やっておくといいかも.
       )
     [ install-pdf-to-cfep ターゲットを編集, cfep/Doc/Intro で next2.pdf を生成. ]
      make -f Makefile-doc install-pdf-to-cfep
   4. make install
      ( cfep 用ツールを OpenXM 以下 へインストール.)
   5. xcode で target を release にして build.
      (プロジェクト -> アクティブなビルド構成を設定 -> release)
   6. make dist-cfep
      (work の下に 配布版 cfep.app を作る)
