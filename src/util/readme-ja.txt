$OpenXM$

使いかたの簡単な解説をとりあえずこのファイルに書く.

oxgentexi:   関数の仕様から texi ファイルを生成するツール.
             asir-contrib のマニュアル作成に使われている.
oxreplace:   文字列の置き換えを行なうツール.
             latex2html で作られたファイルに細工するのに使われている.
oxweave:     一つのファイルに英語のドキュメントや日本語のドキュメント書いて
            おいてそれを oxweave で分解する. oxgentexi と組でも使う.
            speficication や asir-contrib のマニュアルで使ってる.

[oxgentexi]
  oxgentexi への入力は item という単位で管理されている.
  一つの item は キーワード begin:  ではじまり,
  end: でおわる.
  begin: ~ end: の外のデータはすべて読み飛ばされる.
  : (コロン) でおわる単語は oxgentexi ではキーワードとなる.
  コロンを入力するには @colon を用いる.

  コロンでおわるキーワードの次にはかならず空白か改行をいれること.

  item は現在のところ asir関数item と verbatim item の2種類がある.

  verbatim item は次のように書く.
  begin:  key|
    texinfo 形式の入力.
  end:

  key は英数字の並びである. key については後述.
  | のあとに texinfo 形式の入力を書くとそれがそのまま出力される.

  asir関数item は次のように書く.
  begin:  fname(arg1,arg2,...)
     short description of fname.
  description:
     explain about fname. It accepts texinfo inputs.
  example:
     X=fname(2,3);
     Y=fctr(X);
  exampleDescription:
    explain about the example.
  ref:
    xxx yyy
  end:
  
  example: の中では { } や @ は texinfo の特殊記号としては扱われない.  

  item は key により ソートされてから出力される.
  asir関数item の場合は fname が key となる.
  key を変更するには sortKey: キーワードで指定する.

[オプション|oxgentexi]

  --noSorting    key でソートしない.
  --title        texinfo ファイルの title を指定する. これの指定があると,
                 表題も出力して完全な texinfo ファイルを出力する.
  --author       表題にあらわれる著者名.
  --infoFileName  info のファイル名.
  --upnode
  --category

[例1|oxgentexi]
  以下の例は
  nkf -e readme-ja.txt |oxweave ja| oxgentexi --title 'Test of oxgentexi' >test.texi
  ptex test.texi
  と処理.
/*&ja

  begin:  igcd(F,G)
    igcd は整数 {F} と {G} の最大公約数を計算する.
  example:
    igcd(5,3);
  example_description:
    5 と 3 の gcd の計算. 答えは 1.
  example:
    igcd(10,15);
  example_description:
    10 と 15 の gcd の計算. 答えは 5.

  end:

  begin:  fctr(F)
    fctr は多項式{F} を因数分解する.
  description:
   @table @b
   @item アルゴリズム
    因数分解には Berlkamp のアルゴリズムを用いる.
   @item 注意
    ナップサックアルゴリズムはまだ実装してない.
   @end table
  example:
   fctr(x^10-1);

  end:

*/

[例2|oxgentexi]
  以下の例は
  nkf -e readme-ja.txt |oxweave ja2| oxgentexi --noSorting --title 'Test 2 of oxgentexi' >test2.texi

  ptex test2.texi
  と処理.
/*&ja2
  begin: start|
  @section 関数の紹介.

   この節では初歩的な関数を紹介する.

  end:

  begin:  igcd(F,G)
    igcd は整数 {F} と {G} の最大公約数を計算する.
  example:
    igcd(5,3);
  example_description:
    5 と 3 の gcd の計算. 答えは 1.
  example:
    igcd(10,15);
  example_description:
    10 と 15 の gcd の計算. 答えは 5.

  end:

  begin:  fctr(F)
    fctr は多項式{F} を因数分解する.
  description:
   @table @b
   @item アルゴリズム
    因数分解には Berlkamp のアルゴリズムを用いる.
   @item 注意
    ナップサックアルゴリズムはまだ実装してない.
   @end table
  example:
   fctr(x^10-1);

  end:

  begin: owari|

  @section 終りの章
  これでおしまい.

  end:
*/


