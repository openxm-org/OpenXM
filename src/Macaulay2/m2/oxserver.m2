--$OpenXM$
-- server commands
--ox'server'send := (OXSERVER, String) -> (...);
  -- This sends a sequence of bytes to the client.
  -- Note: a shutdown command is never received.

-- Implementation of the OX stack machine, using OX_COMMAND:


OX'commands = new MutableHashTable;

-- Local data for this stack machine.
ox'init'sm = () -> (
    ox'stack = new MutableList from splice {200:null};
    ox'sp = 0;  -- Number of elements currently contained.
    );

ox'view = () -> (
     if ox'sp === 0 then (stderr << "stack empty" << endl;)
     else
         scan(ox'sp, i -> (stderr << "at " << i << " have " << ox'stack#i << endl;)))

ox'popObject = (packetNum) -> (
     if ox'sp === 0 then 
         ox'error(packetNum,"stack underflow")
     else (
	  ox'sp = ox'sp-1;
	  val := ox'stack#ox'sp;
	  ox'stack#ox'sp = null;
	  val))

ox'pop = (packetNum,typ) -> (
     val := ox'popObject packetNum;
     if class val =!= typ then 
         ox'error(packetNum,"incorrect type on stack");
     val)

ox'push = (val) -> (
     if ox'sp > #ox'stack then (
	  -- increase the size of the stack
	  newstack := new MutableList from splice{2 * #ox'stack : null};
	  scan(#ox'stack, i -> newstack#i = ox'stack#i);
	  ox'stack = newstack;
	  );
     ox'stack#ox'sp = val;
     ox'sp = ox'sp + 1;
     )
	       
ox'error = (packetNum,s) -> ox'push ERROR{int32 packetNum,s}

OX'commands#SM'popCMO = (packetNum) -> (
     ox'popObject packetNum
     )

OX'commands#SM'popString = (packetNum) -> (
     v := ox'popObject packetNum;
     toString v
     )

OX'commands#SM'pops = (packetNum) -> (
     n := ox'stack#ox'sp;
     if n > ox'sp then n = ox'sp;
     scan(n, i -> ox'popObject packetNum))

OX'commands#SM'getsp = (packetNum) -> (
     -- Place the number of objects onto the stack.
     ox'push(INT32(ox'sp)))

OX'commands#SM'nop = (packetNum) -> null

OX'commands#SM'setName = (packetNum) -> (
     varname := ox'pop(packetNum,String);
     val := ox'popObject(packetNum);
     (value ("symbol " | varname)) <- val;
     )

OX'commands#SM'evalName = (packetNum) -> (
     varname := ox'pop(packetNum,String);
     val := value ("symbol " | varname);
     ox'push val;
     )

OX'commands#SM'executeStringByLocalParser = (packetNum) -> (
     str := ox'pop(packetNum,String);
     try (
        val := value str;
     	ox'push val;
     ) else
        ox'error(packetNum,"Error occured");
     )

OX'commands#SM'executeFunction = (packetNum) -> (
     fcn := ox'pop(PacketNum,String);
     n := ox'pop(PacketNum,INT32);
     n = n#0;
     args := toSequence apply(n, i -> ox'popObject(packetNum));
     try (
	  val := (value fcn)(args);
	  ox'push val;
     ) else
        ox'error(packetNum,"Error occured");
     )

OX'commands#SM'dupErrors = (packetNum) -> 
     ox'push select(ox'sp, i -> class i === ERROR)

OX'commands#SM'mathcap = (packetNum) -> (
     ox'push(MATHCAP'OF'M2'SERVER);
     )

OX'commands#SM'setMathCap = (packetNum) -> (
     val := ox'pop(packetNum,MATHCAP);
     -- BUG: we are ignoring the math cap!!
     )
-----------------
-- Server code --
-----------------

OXSERVER = new Type of MutableHashTable

ox'send'data = () -> (
     << OXDATA << toNetwork(ox'next'packet);
     ox'next'packet = ox'next'packet + 1;
     )

OXserver = () -> (
     -- initialize stack machine
     stderr << "entering OXserver" << endl << flush;
     ox'init'sm();
     ox'next'packet = 72;
     -- we use stdin, stdout as our two data connections
     -- handshake the network byte input and output
     << "\000\000" << flush;
     x := read stdio;  -- throw out the response.
     OX = makeOXINPUT stdio;
     stderr << "About to enter loop" << endl << flush;
     -- enter the read/eval/write loop
     while (true) do (
	  --readMoreData OX;
	  --if atEndOfFile(stdio) then (
	  --     stderr << "bye bye" << endl << flush;
	  --     return;);
	  typ := get32bits OX;
	  packetNum := get32bits OX;
	  if typ === OX'COMMAND then (
	      cmd := get32bits OX;
	      stderr << "Command = " << cmd << endl << flush;
	      val := OX'commands#cmd(packetNum);
	      stderr << "  Result = " << val << endl << flush;
	      if val =!= null then (
		   -- val is a CMO object
		   ox'send'data();
		   CMOSerialize(OX.F, val);
		   OX.F << flush;
		   );
	      )
	  else if typ === OX'DATA then (
	       v := CMOfromBinary OX;
	       stderr << "value = " << v << endl << flush;
	       ox'push(v)
	       )
	  else ox'error(packetNum, "expected OX_COMMAND or OX_DATA");
	  ox'view()
	  )
     )

///
restart
load "oxcommon.m2"
load "oxserver.m2"
ox'init'sm()
ox'push 3
ox'push 5
ox'push INT32 2
ox'push "binomial"
ox'view()
OX'commands#SM'executeFunction 32
ox'view()
ox'push INT32 12
OX'commands#SM'pops 33
ox'view()
ox'push "asdf = 1347"
OX'commands#SM'executeStringByLocalParser 34
ox'view()
ox'push "2*asdf"
OX'commands#SM'executeStringByLocalParser 34
ox'view()
///

OXserver()
