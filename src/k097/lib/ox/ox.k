/* $OpenXM: OpenXM/src/k097/lib/ox/ox.k,v 1.4 2000/12/11 13:53:28 takayama Exp $  */

/* Load_sm1(["ox.sm1",AddString([GetEnv("OpenXM_HOME"),"/lib/sm1/ox.sm1"])],
         "ox.sm1.loaded"); */
Load_sm1(["oxasir.sm1",AddString([GetEnv("OpenXM_HOME"),
          "/lib/sm1/oxasir.sm1"])],
         "oxasir.sm1.loaded");

class OXchannel extends Object {
  local channel;
  /* Just add a tag. */
  def getChannel() {
    return(channel);
  }
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
    sm1(" [(oxPushCMD) cc ob] extension ");
  }

  def popCMO() {
    local cc,r;
    cc = channel;
    sm1(" cc oxpopcmo /r set ");
    return(r);
  }

}

class OXcontrol extends Object {
  local control;
  def getControl() {
    return(control);
  }
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
      [GetEnv("OpenXM_HOME"),
       "/src/oxc/oxc", 
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
  def popCMO() {
    return(control.channel.popCMO());
  }

}

/* How to debug?
   sm1(" OX { vname } sendmsg /aaa set ");
   Get the value of vname in the context OX and set it to aaa.
*/

class OX extends OXcontrol {
  local engine, engineIDs;
  def getEngine() {
     return(engine);
  }
  def getEngineIDs() {
     return(engineIDs);
  }
  /* OX.generate("ox_sm1"); or OX.generate("ox_sm1",null) */
  def generate(name,control) {
    local c,e, pass, peer, data, s, stmp, engineID;
    if (Length(Arglist) == 0) {
      name = "ox_sm1";  control = null;
    }
    if (Length(Arglist) == 1) {
      control = null;
    }
    this = new(super.generate(control));
    control = super.getControl();
    sm1(" [(oxGetPort1) (localhost)] extension /peer set ");
    sm1(" /data peer 1 get def ");
    control.pushCMO(data); /* BUG: control.pushCMO(data) does not work. 
                              Use control = super.getControl(); */
    control.pushCMO(name);
    control.pushCMD(SM_control_spawn_server);
    engineID = control.popCMO();
    print("engineID=");Println(engineID);
    if (Tag(engineID) == 257 ) {  /* error object */
       Error("Server could be found.");
    }
    engineIDs = [engineID];
    sm1(" [(oxCreateEngine_RFC_101) peer 0 (Empty) engineID] extension /c set ");
    engine = OXchannel.generate(c);
    return(this);
  }
  def pushCMO(a) {
    engine.channel.pushCMO(a);
  }
  def pushCMD(a) {
    engine.channel.pushCMD(a);
  }
  def popCMO() {
    return(engine.channel.popCMO());
  }

  def rpc(func,ar) {
     local n,i,one,comm;
     if (! IsString(func)) {
       Error("The first argument of rpc must be a string.");
     }
     if (! IsArray(ar)) {
       Error("The second argument of rpc must be a list.");
     }
     one = IntegerToSm1Integer(1);
     sm1(" getVlist2 toString /comm set ");
     comm = AddString(["OxVlist=",comm,";"]);
     this.pushCMO(comm);
     this.pushCMD(SM_executeStringByLocalParser);

     this.pushCMO("print(OxVlist);");
     this.pushCMD(SM_executeStringByLocalParser);

     n = Length(ar);
     for (i=n-1; i>=0; i--) {
       this.pushCMO(ar[i]);
       this.pushCMO(one);
       this.pushCMO("ox_dtop");
       this.pushCMD(SM_executeFunction);
     }
     this.pushCMO(IntegerToSm1Integer(n));
     this.pushCMO(func);
     this.pushCMD(SM_executeFunction);

     this.pushCMO(one);
     this.pushCMO("ox_ptod");
     this.pushCMD(SM_executeFunction);
     return(this.popCMO());
  }

}

/*
  Example:
     a = startAsir();
     f = a.rpc("fctr",[x^10-1]);
     Println(f);
*/
def startAsir() {
  local a,asirpeer;
  a = OX.generate("ox_asir");
  asirpeer = (a.getEngine()).getChannel();
  Println(asirpeer);
  sm1(" [asirpeer oxasirpath.asirlib] loadAsirFile ");
  return(a);
}

