/* $OpenXM: OpenXM/src/k097/lib/ox/ox.k,v 1.1 2000/12/06 11:37:34 takayama Exp $  */

class OXcontrol extends Object {
  local control;
  def new0(c) {
     if (Tag(c) != 0) {
       control = c;
       return(this);
     }else{
       return(start());
     }
  }
  def start() {
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
    control = c;
    return(this);
  }

}


class OXengine extends OXcontrol {
  local engine;
  def start(control,name) {
    local c,e, pass, peer, data, s, stmp, engineID;
    this = super.new0(control);
    control = this.control;
    sm1(" [(oxGetPort1) (localhost)] extension /peer set ");
    sm1(" /data peer 1 get def ");
    control.pushCMO(data);
    control.pushCMO(name);
    control.pushCMD(SM_control_spawn_server);
    engineID = control.popCMO();
    sm1(" [(oxCreateEngine_RFC_101) peer 0 (Empty) engineID] extension /c set ");
    engine = c;
    return(this);
}

}

