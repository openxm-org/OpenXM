$OpenXM$
http://www.openxm.org 
(http://www.math.kobe-u.ac.jp/OpenXM)
の更新方法.

OpenXM/src で make install
OpenXM/src で make install-document
して, OpenXM/lib, OpenXM/doc 等を最新版にする必要がある.

また index.html 等を更新する必要がある.

(cd OpenXM/rc; make ; source dot.bashrc)  [ bash の場合]
をやって OpenXM 環境を利用できるようにしてから 
OpenXM-web で make を始めること.
make clean  (make clean-everything)
してから
make all
することにより /home/web/OpenXM/Current が更新される.

OpenXM-web で編集すべきファイル.
top.html, 
index.html (head)

Release 版の場合は 
cd /home/web/OpenXM
cp -r Current /home/web/OpenXM/x.y.z (version number)
/home/web/OpenXM/x.y.z/index.html  を編集する.
その後, /home/web/OpenXM/x.y.z は freeze する.

tar-ball release 版/ head 版の作成方法.
cd misc/packages/Generic/openxm
 make install-tar-to-ftp-area
 ftp.math.kobe-u.ac.jp/pub/OpenXM/head/openxm-head.tar.gz へコピーされる.
release 版の場合これの名前を変えて
 ftp.math.kobe-u.ac.jp/pub/OpenXM/x.y.z/openxm-x.y.z.tar.gz へもコピー.



