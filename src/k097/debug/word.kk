/* word.kk , 1997, 4/9? */
/*  see object.kk for pre-installed objects 
*/

class Word extends Object {
  local word;  /* word のリスト */
  def new0() {
    this = new();
    word = [ ];
    return(this);
  }
  def new1(a) {
    this = new();
    word = [a];
    return(this);
  }
  def void show() {
    local size,i;
    size = Length(word);
    for (i=0; i<size; i++) {
      Print(word[i]);   
        /* 多分これが, bug.  word は this 1 get と翻訳されるが, [i] で
           とりだせない. */
    }
  }
}

