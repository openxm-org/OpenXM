/* $OpenXM: OpenXM/src/k097/lib/ox/ox.k,v 1.2 2000/12/06 23:45:32 takayama Exp $  */

Load_sm1(["ox.sm1",AddString([GetEnv("OpenXM_HOME"),"/lib/sm1/ox.sm1"])],
         "ox.sm1.loaded");
def void Error(s) {
  sm1(" s error ");
}

class OXchannel extends Object {
  local channel;
  /* Just add a tag. */
  def generate(c) {
    this = new();
    if (IsArray(c)) {
      if (c[0] != "client") {
         Error("OXchannel.generate(c) : c is not client object.");
      }
    }else{
      Error("OXchannel.generate(c) : c is not client object.");
    }
    channel = c;
    return(this);
  }

  def pushCMO(ob) {
    local cc;
    cc = channel;
    sm1(" cc ob oxpushcmo ");
  }

  def pushCMD(ob) {
    local cc;
    cc = channel;
    if (IsInteger(ob)) {
      ob = IntegerToSm1Integer(ob);
    }
    sm1(" cc ob oxpushcmd ");
  }
}

class OXcontrol extends Object {
  local control;
  def generate(c) {
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
    control = OXchannel.generate(c);
    return(this);
  }
  def pushCMO(a) {
    control.channel.pushCMO(a);
  }
  def pushCMD(a) {
    control.channel.pushCMD(a);
  }

}


class OX extends OXcontrol {
  local engine;
  /* OX.generate(null,"ox_sm1"); */
  def generate(control,name) {
    local c,e, pass, peer, data, s, stmp, engineID;
    this = new(super.generate(control));
    control = this.control;
    sm1(" [(oxGetPort1) (localhost)] extension /peer set ");
    sm1(" /data peer 1 get def ");
    control.pushCMO(data);
    control.pushCMO(name);
    control.pushCMD(SM_control_spawn_server);
    engineID = control.popCMO();
    sm1(" [(oxCreateEngine_RFC_101) peer 0 (Empty) engineID] extension /c set ");
    engine = OXchannel.generate(c);
    return(this);
  }

}

