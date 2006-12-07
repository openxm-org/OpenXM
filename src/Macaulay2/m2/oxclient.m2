ID=" $OpenXM: OpenXM/src/Macaulay2/m2/oxclient.m2,v 1.5 2006/10/27 01:38:17 takayama Exp $ "

-- This is configured for Macaulay2 0.9.95
load "oxcommon.m2"

oxSession = method()
oxSession(String) := (s) -> (
     try (F := oxStartSession(s))
     else ( error "Could not start the server " | s);
     F
     )

oxFlush = method()
oxFlush(OXSESSION, File, String) := (F, file, str) -> (
     if (F.FlushFlag == 0) then file << str << flush
     else (
	  read "Press Return to flush: ";
	  file << str << flush;
	  );     
     )

oxToggleFlushFlag = method()
oxToggleFlushFlag(OXSESSION, ZZ) := (F, n) -> (
     F.FlushFlag = n;
     )

oxEmptyHashTable = method()
oxEmptyHashTable(OXSESSION) := (F) -> (
     apply(keys F, i -> remove(F, i));
     )

oxasir = () -> ( oxStartSession "ox_asir" )

-- this method starts the server s.
oxStartSession = (s) -> (
     MINPORT := 1025;
     MAXPORT := 5000;
     MAXTRIES := 3; -- number of times to try to open new ports
     MAXTRIES = 1000; -- number of times to try to open new ports
     PASS := "M2";

     F := null;
     G := null;
     i := 0;
     while (i < MAXTRIES and (F === null or G === null)) do (
		  if not (F === null) then (close F; F = null);
		  if not (F === null) then (close G; G = null);
     	  cPort := random (MAXPORT - MINPORT) + MINPORT;
     	  dPort := random (MAXPORT - MINPORT) + MINPORT;
     	  if (cPort != dPort) then (
              try F = openListener(toString "$:"|cPort) else F = null;
              try G = openListener(toString "$:"|dPort) else G = null;
	       );
           i = i+1;
	  );
     << "F = " << F << " and " << " G = " << G << endl << flush;
     if (F === null or G === null) then 
         error "Couldn't connect to the ox server" | s;
     comm = toString "xterm -geometry 80x25-0-0 -e  ox -ox " | s | " -data " |
                      dPort | " -control " | cPort |
		      " -reverse -host localhost" | " -pass " | PASS | " &" ;
     comm = makeLaunchCommand(false,"localhost",getenv("USER"),"localhost",
	                      s,dPort,cPort,PASS);
     -- true : connection by ssh, false: only for the local host.
     print comm;
     if (run (comm) != 0) 
       then error "Couldn't start the ox server " | s;

     F = openInOut F;
     G = openInOut G;	 	   
     F << "\000" << flush ; -- set to network byte order
     G << "\000" << flush;

     FP = makeOXINPUT F;
     S := getBytes(FP,#PASS);
     if S != PASS then (
	  close F;
	  close G;
	  error "wrong one time password";
	  );
     GP = makeOXINPUT G;
     S = getBytes(GP,#PASS);
     if S != PASS then (
	  close F;
	  close G;
	  error "wrong one time password";
	  );
     getBytes(FP,2);
     getBytes(GP,2);
     
     -- need to change the following.
     new OXSESSION from {
	  ServerName => s,
	  ControlPort => F,
	  DataPort => G,
	  SerialNumber => 0,
	  FlushFlag => 0,
	  DataBuffer => makeOXINPUT G,
	  Buffer => "",
	  BufferPointer => 0}
     )

-- methods to access data fields of the OXSESSION class
oxGetDataPort = method()
oxGetDataPort(OXSESSION) := (F) -> (F.DataPort)
oxGetControlPort = method()
oxGetControlPort(OXSESSION) := (F) -> (F.ControlPort)
oxGetSerialNumber = method()
oxGetSerialNumber(OXSESSION) := (F) -> (F.SerialNumber)
oxGetFlushFlag = method()
oxGetFlushFlag(OXSESSION) := (F) -> (F.FlushFlag)

incrementSerialNumber = method()
incrementSerialNumber(OXSESSION) := (F) -> (
     F.SerialNumber = F.SerialNumber + 1;)

oxPushCMO = (F,s) -> (
     incrementSerialNumber(F);
     Fport := oxGetDataPort(F);
     Fport << OXDATA 
           << toNetwork(oxGetSerialNumber(F));
     CMOSerialize(Fport,s);
     oxFlush(F, Fport, "");
     )

oxData = method()
oxData(OXSESSION, String) :=
oxData(OXSESSION, ZZ) := 
oxData(OXSESSION, INT32) :=
oxData(OXSESSION, List) := oxPushCMO

-- method to set mathcap of mine to the server.
oxSetMathcap = method()
oxSetMathcap(OXSESSION) := (F) -> (
     incrementSerialNumber(F);
     Fport := oxGetDataPort(F);
     Fport << OXDATA << toNetwork(oxGetSerialNumber(F))
           << toNetwork(CMO'MATHCAP);
     CMOSerialize(Fport,MATHCAP'OF'M2);
     oxFlush(F, oxGetDataPort(F), "");
     oxCommand(F,SM'setMathCap);
     )

-- method to send a command to the data port
oxCommand = method()
oxCommand(OXSESSION, ZZ) := (F, TagNumber) -> (
     incrementSerialNumber(F);
     str := OXCOMMAND | toNetwork(oxGetSerialNumber(F)) | toNetwork(TagNumber);
     oxFlush(F, oxGetDataPort(F), str);
     )

-- method to send a command to the control port
oxControl = method()
oxControl(OXSESSION, ZZ) := (F, TagNumber) -> (
     incrementSerialNumber(F);
     str := OXCOMMAND | toNetwork(oxGetSerialNumber(F)) | toNetwork(TagNumber);
     oxFlush(F, oxGetControlPort(F), str);
     )

-- various commands
oxExecuteStringByLocalParser = method()
oxExecuteStringByLocalParser(OXSESSION) := (F) -> (
     oxCommand(F, SM'executeStringByLocalParser);
     )

oxExecuteFunction = method()
oxExecuteFunction(OXSESSION) := (F) -> (
     oxCommand(F, SM'executeFunction);
     )

oxPopCMO = method()
oxPopCMO(OXSESSION) := (F) -> (
     oxCommand(F, SM'popCMO);
     oxTag := get32bits(F.DataBuffer); -- read the OX tag
     serTag := get32bits(F.DataBuffer); -- read the Serial tag
     CMOfromBinary(F.DataBuffer)
     );

oxPopString = method()
oxPopString(OXSESSION) := (F) -> (
     oxCommand(F, SM'popString);
     oxTag := get32bits(F.DataBuffer); -- read the OX tag
     serTag := get32bits(F.DataBuffer); -- read the Serial tag
     CMOfromBinary(F.DataBuffer)
     )


oxKill = method()
oxKill(OXSESSION) := (F) -> (
     oxControl(F, SM'control'kill);     
     oxEmptyHashTable(F);
     )

oxRestart = method()
oxRestart(OXSESSION) := (F) -> (
     oxControl(F, SM'control'kill);
     tmp := oxStartSession(F.ServerName);
     scan(keys tmp, i -> (F#i = tmp#i));
     F.DataBuffer = makeOXINPUT (F.DataPort);
     F
     )

oxPopMathcap = method()
oxPopMathcap(OXSESSION) := (F) ->(
	 oxCommand(F,SM'mathcap);
	 oxPopCMO(F)
	 )

oxPrimDecomp = method()
oxPrimDecomp(OXSESSION, Ideal) := (F, I) -> (
     -- translate to asir notation
     s := toString flatten entries gens I;
     s = replace("\\{","[",s);   -- substring(2,5,s) 
     s = replace ("\\}","]",s);
     v = toString gens ring I;
     v = replace("\\{","[",v);
     v = replace("\\}","]",v);
     s = "primadec(" | s | "," | v | ");";
     << "sending" << s << endl;
     oxData(F, s);
     oxExecuteStringByLocalParser(F);
     str = oxPopString(F);
     -- BUG: oxPopCMO(F) is still buggy.     
     toList apply(value str, i -> {ideal(toList(i#0)), ideal(toList(i#1))})
     )

-- makeLaunchCommand generates an argument of run.
-- Replace comm = toString "xterm ...." in oxStartSession (oxcommon.m2) by 
--  comm = makeLauchCommand with proper arguments.
-- I think that the examples below speak itself.
-- Some arguments are not necessary when UseSSH is true. 
-- there may be a neat way to define optional arguments in M2.

makeLaunchCommand=(UseSSH,MyHostName,RemoteLoginName,RemoteHostName,OxServerName,dPort,cPort,PASS)->(
     commXterm := toString " xterm -geometry 80x25-0-0 " ;
     commOX :=toString " -e ox -ox " | 
               getenv("OpenXM_HOME") | "/bin/" |
               OxServerName | 
                       " -data " | dPort |
    		       " -control " | cPort |
		       " -reverse " |
		       " -host " | RemoteHostName | 
		       " -pass " | PASS   | "  ";
     if (UseSSH) then (
	  comm = toString "ssh -f -X -A -l " | RemoteLoginName | " " | 
	                   RemoteHostName |
	                   " '" | commXterm |
  		           --uncomment when X11 forwarding is not allowed.
			   -- " -display " | MyHostName | ":0 " | 
			   commOX | "'  >/dev/null ";
	  )
     else  (
	  comm = toString commXterm | commOX | " &";
	  );
     comm
     )

///
print makeLaunchCommand(false,"localhost","grobner","localhost","ox_asir",1300,1200,"M2")
print " "
print makeLaunchCommand(true,"localhost","grobner","localhost","ox_asir",1300,1200,"M2")
print " "
print makeLaunchCommand(true,"mymachine.edu","graver","oxservers.com","ox_m2",1300,1200,"M2")
print " "
print makeLaunchCommand(true,"hogehoge.com","goober","servers.openxm.org","ox_sm1",1300,1200,"M2");
print " "
///
