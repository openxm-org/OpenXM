/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/OpenXM.java,v 1.5 1999/11/11 20:17:18 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;
import java.net.*;

public class OpenXM implements Runnable{
  private OpenXMconnection control = null, stream = null;
  private Runnable process = null;
  private Thread thread = null;
  final boolean debug = true;

  // OX message_tag
  final public static int OX_COMMAND                  = 513;
  final public static int OX_DATA                     = 514;
  final public static int OX_SYNC_BALL                = 515;
  final public static int OX_PRIVATE                  = 0x7fff0000;

  public OpenXM(String host,int CtrlPort,int StreamPort) throws IOException{
    control = new OpenXMconnection(host,CtrlPort);

    try{
      Thread.sleep(100); // server がすぐに反応できないので wait が必要
    }catch(InterruptedException e){
      System.err.println(e.getMessage());
    }

    stream = new OpenXMconnection(host,StreamPort);

    control.sendByteOrder();
    stream.sendByteOrder();
  }

  public OpenXM(Runnable process,String host,int CtrlPort,int StreamPort)
       throws IOException{
	 control = new OpenXMconnection(host,CtrlPort,true);
	 new Thread(this).start();

	 stream = new OpenXMconnection(host,StreamPort,true);
	 stream.sendByteOrder();
	 this.process = process;
	 thread = new Thread(this.process);
	 thread.start();
  }

  public void run(){
    try{
      control.sendByteOrder();

      while(true){
	debug("control: wait OX");
	switch(control.receiveOXtag()){
	case OX_DATA:
	  control.receiveCMO();
	  break;

	case OX_COMMAND:
	  switch(control.receiveSM().getCode()){
	  case SM.SM_control_kill:
	    return;

	  case SM.SM_control_reset_connection:
	    this.resetConnection();
	    break;

	  default:
	  }
	  break;

	case OX_SYNC_BALL:
	default:
	  break;
	}
      }
    }catch(IOException e){
      System.err.println(e.getMessage());
    }finally{
      debug("computer process interrupting...");
      thread.stop();
      debug("interrupted. OK! I'll shutdown.");
      try{stream.close();}catch(IOException e){}
      try{control.close();}catch(IOException e){}
    }
  }

  public synchronized void resetConnection(){
    debug("control: stopping computer process...");
    try{
      control.sendCMO(new CMO_INT32(0));
    }catch(IOException e){}
    synchronized(stream){
      thread.stop();
    }
    debug("control: stopped.");

    try{
      debug("control: sending SYNC BALL.");
      stream.sendOX_SYNC_BALL();

      debug("control: waiting to receive SYNC BALL.");
      while(true){
	int ox_tag = stream.receiveOXtag();

	debug("control: received "+ toString(ox_tag));
	if(ox_tag == OX_SYNC_BALL){
	  break;
	}

	switch(ox_tag){
	case OpenXM.OX_COMMAND:
	  stream.receiveSM();
	  break;

	case OpenXM.OX_DATA:
	  stream.receiveCMO();
	  break;
	}
      }
      debug("control: received SYNC BALL and restart computer process");
      thread = new Thread(this.process);
      thread.start();
    }catch(IOException e){}
  }

  public void sendSM(SM code) throws IOException{
    stream.sendSM(code);
  }

  public void sendCMO(CMO object) throws IOException{
    stream.sendCMO(object);
  }

  public void send(Object object) throws IOException{
    stream.send(object);
  }

  public CMO receiveCMO() throws IOException{
    return stream.receiveCMO();
  }

  public SM receiveSM() throws IOException{
    return stream.receiveSM();
  }

  public int receiveOXtag() throws IOException{
    return stream.receiveOXtag();
  }

  public void receive() throws IOException{
    int tag = stream.receiveOXtag();

    System.out.print("("+ toString(tag) +",");
    //System.out.print("serial="+ dis.readInt() +"," );

    switch(tag){
    case OX_COMMAND:
      System.out.print(""+ stream.receiveSM());
      break;

    case OX_DATA:
      System.out.print(""+ stream.receiveCMO());
      break;
    }

    System.out.println(")");
  }

  public static String toString(int tag){
    switch(tag){
    case OX_COMMAND:
      return "OX_COMMAND";

    case OX_DATA:
      return "OX_DATA";

    case OX_SYNC_BALL:
      return "OX_SYNC_BALL";

    default:
    }
    return "unknown code("+ tag +")";
  }

  private final void debug(String str){
    if(debug){
      System.err.println(str);
    }
  }

  public static void main(String[] args){
    OpenXM ox;

    try{
      ox = new OpenXM("localhost",1200,1300);
    }catch(UnknownHostException e){
      System.err.println("host unknown.");
      System.err.println(e.getMessage());
      return;
    }catch(IOException e){
      System.err.println("connection failed.");
      System.err.println("IOException occuer !!");
      System.err.println(e.getMessage());
      return;
    }

    try{
      //サーバ側へ文字列を送信します。
      ox.sendSM(new SM(SM.SM_mathcap));
      ox.sendSM(new SM(SM.SM_popString));

      //ox.sendCMO(os,new CMO_STRING("print(\"Hello world!!\");\n"));
      //ox.sendSM(os,new SM(SM.SM_executeStringByLocalParser));

      //ox.sendCMO(os,new CMO_STRING("def sub(B,A){return B-A;}"));
      //ox.sendSM(os,new SM(SM.SM_executeStringByLocalParser));
      //ox.sendSM(os,new SM(SM.SM_popString));

      //ox.sendCMO(os,new CMO_STRING("diff((x+2*y)^2,x);\0 1+2;"));
      //ox.sendSM(os,new SM(SM.SM_executeStringByLocalParser));
      //ox.sendSM(os,new SM(SM.SM_popString));

      //ox.sendCMO(os,new CMO_ZZ("1"));
      ox.sendCMO(new CMO_ZZ("-2"));
      //ox.sendCMO(os,new CMO_INT32(2));
      //ox.sendCMO(os,new CMO_STRING("sub"));
      //ox.sendSM(os,new SM(SM.SM_executeFunction));
      ox.sendSM(new SM(SM.SM_popCMO));

      { int[] array = {1,2};
      ox.sendCMO(new CMO_MONOMIAL32(array,new CMO_ZZ("-2")));
      }
      ox.sendSM(new SM(SM.SM_popCMO));

      ox.sendSM(new SM(SM.SM_popString));
      //ox.sendSM(os,new SM(SM.SM_popString));

      ox.sendSM(new SM(SM.SM_control_kill));

      //os.flush();

      //サーバ側から送信された文字列を受信します。
      while(true){
	ox.receive();
      }

    }catch(IOException e){
      e.printStackTrace();
    }

    System.out.println("breaking...");

    try{
      // close connection
      ox.stream.close();
      ox.control.close();
    }catch(IOException e){
      e.printStackTrace();
    }
  }
}
