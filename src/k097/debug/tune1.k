

def foo10() {
  local i,n,keys;
  sm1(" set_timer ");
  n = Length(Helplist);
  keys = [" " ];  /* This is a gate keeper for shell. */
  PSfor (i=0; i< n; i++ ) {
    keys = Append(keys,Helplist[i,0]);
  }
  sm1(" set_timer ");
  sm1(" set_timer ");
  keys = sm1(keys," shell ");
  sm1(" set_timer ");
}

def foo20() {
  local i,n,keys;
  sm1(" set_timer ");
  n = Length(Helplist);
  keys = [" " ];  /* This is a gate keeper for shell. */
  PSfor (i=0; i< n; i++ ) {
    keys = dummyf(keys,Helplist[i,0]);
  }
  sm1(" set_timer ");
}

def dummyf(a,b) {
  ;
}

/*   1997, 6/1 (日曜日)
    n = 68

In[13]=foo10();
User time: 0.866667 seconds, System time: 0.000000 seconds
User time: 0.766667 seconds, System time: 0.000000 seconds: shell sort の時間
In[14]=foo20();
User time: 0.633333 seconds, System time: 0.000000 seconds

    foo10 と foo20 では, 0.2 秒程度の差しかない.
    したがって, append では, Helplist[i,0] の取り出しと,
    局所変数の処理にほとんどの時間が使われていると予想できる.

*/
   
def foo30() {
  local i,n,keys;
  sm1(" set_timer ");
  n = Length(Helplist);
  keys = [" " ];  /* This is a gate keeper for shell. */
  PSfor (i=0; i< n; i++ ) {
    keys = dummyf(keys,i); /* 配列要素のとりだしなし */
  }
  sm1(" set_timer ");
}

/* foo30() は, 0.48 秒.  よって, 配列要素の取り出しに, 0.2 秒程度かかっている.
*/


def foo40() {
  local i,n,keys;
  n = Length(Helplist);
  keys = [" " ];  /* This is a gate keeper for shell. */
  sm1(" set_timer ");
  PSfor (i=0; i< n; i++ ) {
    keys = dummyf(keys,i); /* 配列要素のとりだしなし */
  }
  sm1(" set_timer ");
}
/* これは, 0.46 秒.  Length の計算は, 0.02 秒 */

/* 関数呼び出しは, 0.5 秒, 配列要素の取り出しが, 0.2秒 */


