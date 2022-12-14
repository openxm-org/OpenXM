/**
 * $OpenXM: OpenXM/src/OpenMath/OMproxy.java,v 1.41 2000/10/11 08:32:13 ohara Exp $
 */

import ORG.openxm.tam.*;
import java.util.Stack;
import java.io.*;

public class OMproxy extends OpenXMControlServer{
  private Stack stack = new Stack();
  protected boolean debug = false;
  final int version = 200006130;

  public OMproxy(String hostname,int ControlPort,int DataPort){
    super(hostname,ControlPort,DataPort);
  }

  public void computeProcess(OpenXMstream stream){
    OM2OXM P = new OM2OXM();

    debug("OMproxy started.");
    try{
      while(true){
	try{
	  OXmessage message = stream.receive();
	  int ox_tag = message.getTag();

	  switch(ox_tag){
	  case OXmessage.OX_COMMAND:
	    StackMachine((SM)message.getBody(),stream);
	    break;

	  case OXmessage.OX_DATA:
	    stack.push(message.getBody());
	    debug("push: "+ stack.peek());
	    break;
	  }
	}catch(RuntimeException e){
	  System.err.println(e.getMessage());
	  e.printStackTrace();
	  debug("error occured. stack was cleared.");
	  stack = new Stack();
	}
      }
    }catch(IOException e){
      System.err.println(e.getMessage());
      e.printStackTrace();
      System.err.println("error occured, and recovering processes seems to be impossible.");
    }catch(Exception e){
      System.err.println(e.getMessage());
      e.printStackTrace();
      System.err.println("error occured, and recovering processes seems to be impossible.");
    }finally{
      System.err.println("breaking...");
    }
  }

  /*
  public void stop(){
    System.err.println("OMproxy Stoping...");
    synchronized(ox){
      //this.stop();
      while(!stack.empty()){
	stack.pop();
      }
      System.err.println("OMproxy Stopped");
    }
  }
  */

  private void SM_popCMO(OpenXMstream stream) throws java.io.IOException{
    try{
      if(stack.empty()){
	stream.send(new CMO_NULL());
      }else{
	debug("sending CMO: "+ stack.peek());
	stream.send((CMO)stack.pop());
	debug("test");
      }
    }catch(MathcapViolation e){
      try{
	stream.send(new CMO_ERROR2(new CMO_STRING("MathcapViolation: "+
					      e.getMessage())));
      }catch(MathcapViolation tmp){}
    }
  }

  private void SM_executeFunction() throws java.io.IOException{
    String function_name;
    CMO[] argv;
    int argc;

    if(!(stack.peek() instanceof CMO_STRING)){
      stack.push(new CMO_ERROR2(new CMO_NULL()));
      return;
    }
    function_name = ((CMO_STRING)stack.pop()).getString();
    argc = ((CMO_INT32)stack.pop()).intValue();
    argv = new CMO[argc];
    for(int i=0;i<argc;i++){
      argv[i] = (CMO)stack.pop();
    }

    if(argc != 1){
      stack.push(new CMO_ERROR2(new CMO_NULL()));
      return;
    }

    if(function_name.equals("OMXML2CMO")){
      stack.push(OMXML2CMO(argv[0]));
      return;
    }else if(function_name.equals("CMO2OMXML")){
      stack.push(CMO2OMXML(argv[0]));
      return;
    }

    stack.push(new CMO_ERROR2(new CMO_NULL()));
    return;
  }

  private void SM_mathcap(OpenXMstream stream) throws java.io.IOException{
    CMO[] mathcap = new CMO[3];

    {
      CMO[] list = {new CMO_INT32(version),
		  new CMO_STRING("Ox_system=OMproxy.class"),
		  new CMO_STRING("Version=0."+ version),
		  new CMO_STRING("HOSTTYPE=JAVA")};
      mathcap[0] = new CMO_LIST(list);
    }

    {
      CMO[] list = {new CMO_INT32(SM.SM_popCMO),
		    new CMO_INT32(SM.SM_executeFunction),
		    new CMO_INT32(SM.SM_mathcap),
		    new CMO_INT32(SM.SM_setMathCap),
		    new CMO_INT32(SM.SM_control_kill),
		    new CMO_INT32(SM.SM_control_reset_connection)};
      mathcap[1] = new CMO_LIST(list);
    }

    {
      CMO[] CMOFormat = {new CMO_INT32(CMO.NULL),
			 new CMO_INT32(CMO.INT32),
			 new CMO_INT32(CMO.STRING),
			 new CMO_INT32(CMO.LIST),
			 new CMO_INT32(CMO.MONOMIAL32),
			 new CMO_INT32(CMO.ZZ),
			 new CMO_INT32(CMO.QQ),
			 new CMO_INT32(CMO.ZERO),
			 new CMO_INT32(CMO.DMS_GENERIC),
			 new CMO_INT32(CMO.RECURSIVE_POLYNOMIAL),
			 new CMO_INT32(CMO.DISTRIBUTED_POLYNOMIAL),
			 new CMO_INT32(CMO.POLYNOMIAL_IN_ONE_VARIABLE),
			 new CMO_INT32(CMO.BIGFLOAT),
			 new CMO_INT32(CMO.INDETERMINATE),
			 new CMO_INT32(CMO.TREE)};
      CMO[] DataFormat1 = {new CMO_INT32(OXmessage.OX_DATA),
			   new CMO_LIST(CMOFormat)};
      CMO[] list = {new CMO_LIST(DataFormat1)};

      mathcap[2] = new CMO_LIST(list);
    }

    stack = new Stack();
    stack.push(new CMO_MATHCAP(new CMO_LIST(mathcap)));
    debug("push: "+ stack.peek());
  }

  private void SM_setMathCap(OpenXMstream stream)
       throws java.io.IOException{
    Object mathcap = stack.pop();

    if(mathcap instanceof CMO_MATHCAP){
      stack.push(new CMO_ERROR2(new CMO_NULL()));
    }
    stream.setMathCap((CMO_MATHCAP)mathcap);
  }

  private void StackMachine(SM mesg,OpenXMstream stream)
       throws java.io.IOException{
    debug("receive: "+mesg);

    switch(mesg.getCode()){
    case SM.SM_popCMO:
      SM_popCMO(stream);
      break;

    case SM.SM_executeFunction:
      SM_executeFunction();
      break;

    case SM.SM_mathcap:
      SM_mathcap(stream);
      break;

    case SM.SM_setMathCap:
      SM_setMathCap(stream);
      break;

    default:
      System.err.println("received "+ mesg);
    }
  }

  private CMO CMO2OMXML(CMO obj){
    String str;

    try{
      str = OM2OXM.CMO2OM(obj);
    }catch(NumberFormatException e){
      debug("CMO2OMXML occuered error in trans");
      return new CMO_ERROR2(new CMO_STRING(e.toString()));
    }

    return new CMO_STRING(str);
  }

  private CMO OMXML2CMO(CMO obj){
    OM2OXM trans = new OM2OXM();
    //StringBufferInputStream stream;
    ByteArrayInputStream stream;
    CMO ret;

    debug("OMXML2CMO called: "+obj);
    if(!(obj instanceof CMO_STRING)){
      return new CMO_ERROR2(new CMO_STRING("It's not CMO_STRING."));
    }

    try{
      stream = new ByteArrayInputStream(((CMO_STRING)obj).getString().getBytes());
      ret = trans.parse(stream);
    }catch(IOException e){
      debug("OMXML2CMO occuered error in trans");
      return new CMO_ERROR2(new CMO_STRING(e.toString()));
    }catch(NumberFormatException e){
      debug("OMXML2CMO occuered error in trans");
      return new CMO_ERROR2(new CMO_STRING(e.toString()));
    }

    debug("push: "+ret);
    return ret;
  }

  private void debug(String str){
    if(debug){
      System.err.println(str);
    }
  }

  private static String usage(){
    String ret = "";

    ret += "usage\t: java OMproxy [options]\n";
    ret += "options\t:\n";
    ret += "\t -h \t show this message\n";
    ret += "\t -host hostname \t (default localhost)\n";
    ret += "\t -data port \t (default 1300)\n";
    ret += "\t -control port \t (default 1200)\n";
    ret += "\t -insecure \t this version ignore this option\n";
    ret += "\t -debug \t display debug messages\n";

    return ret;
  }

  public static void main(String[] argv){
    String hostname = "localhost";
    int ControlPort = 1200, DataPort = 1300;
    OpenXMControlServer ox;

    for(int i=0;i<argv.length;i++){
      if(argv[i].equals("-h")){
        System.err.println("");
        System.exit(0);
      }else if(argv[i].equals("-host")){
        hostname = argv[++i];
      }else if(argv[i].equals("-data")){
        DataPort = Integer.valueOf(argv[++i]).intValue();
      }else if(argv[i].equals("-control")){
        ControlPort = Integer.valueOf(argv[++i]).intValue();
      }else{
        System.err.println("unknown option :"+ argv[i]);
        System.exit(1);
      }
    }

    //ox = new OpenXMserver(hostname,ControlPort,DataPort);
    ox = new OMproxy(hostname,ControlPort,DataPort);
    /*
    try{
    }catch(java.net.UnknownHostException e){
      System.err.println("host unknown.");
      System.err.println(e.getMessage());
      return;
    }catch(IOException e){
      System.err.println("connection failed.");
      System.err.println("IOException occuer !!");
      System.err.println(e.getMessage());
      return;
    }
    */

    ox.start();

    System.err.println("breaking...");
  }
}
