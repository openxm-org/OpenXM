/* $OpenXM$  */

class OX extends Object {
  local client,
        engine;
  def start(name) {
    local c,e, pass, peer, data, s, stmp;
    this = new(super.new0());
    sm1(" [(oxGenPass)] extension /pass set ");
    sm1(" [(oxGetPort1) (localhost)] extension /peer set ");
    sm1(" /data peer 1 get toString def ");
    s = AddString(
      ["/home/nobuki/OpenXM/src/oxc/oxc", 
       " -c ", pass, " -h "," localhost ", " -p ", data, " -x ",
       " & "]);
    sm1(" [ (/bin/csh -c \") s (\") ] cat /stmp set ");
    sm1(" stmp message ");
    sm1(" stmp system  ");
    sm1(" [(oxCreateControl_RFC_101) peer 0 pass] extension /c set ");
    client = c;
    return(this);
}



}