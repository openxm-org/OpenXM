/**
 * $OpenXM: OpenXM/src/OpenMath/JP/ac/kobe_u/math/tam/OpenXM/OpenXM.java,v 1.10 2000/03/15 15:02:07 tam Exp $
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;
import java.net.*;

public class OpenXM implements Runnable{
  private OpenXMconnection control = null, stream = null;
  private Runnable process = null;
  private Thread thread = null;
  final protected boolean debug = false;

  // OX message_tag
  final public static int OX_COMMAND                  = 513;
  final public static int OX_DATA                     = 514;
  final public static int OX_SYNC_BALL                = 515;
  final public static int OX_PRIVATE                  = 0x7fff0000;

  public OpenXM(String host,int CtrlPort,int StreamPort) throws IOException{
    control = new OpenXMconnection(host,CtrlPort);

    try{
      Thread.sleep(100); // We need a few wait for starting up server.
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

      debug("control: wait OX");
      while(true){
	OXmessage message = control.receive();

	switch(message.getTag()){
	case OX_COMMAND:
	  switch(((SM)message.getBody()).getCode()){
	  case SM.SM_control_kill:
	    return;

	  case SM.SM_control_reset_connection:
	    this.resetConnection();
	    break;
	  }
	  break;

	case OX_DATA:
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
    }catch(IOException e){
    }catch(MathcapViolation e){}
    synchronized(stream){
      thread.stop();
    }
    debug("control: stopped.");

    try{
      debug("control: sending SYNC BALL.");
      stream.sendOX_SYNC_BALL();

      debug("control: waiting to receive SYNC BALL.");
      while(true){
	OXmessage message = stream.receive();

	debug("control: received "+ message);
	if(message.getTag() == OX_SYNC_BALL){
	  break;
	}
      }
      debug("control: received SYNC BALL and restart computer process");
      thread = new Thread(this.process);
      thread.start();
    }catch(IOException e){}catch(MathcapViolation e){}
  }

  public void send(OXbody object) throws IOException,MathcapViolation{
    stream.send(object);
  }

  public OXmessage receive() throws IOException{
    return stream.receive();
  }

  public void setMathCap(CMO_MATHCAP mathcap){
    stream.setMathCap(mathcap);
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
      ox.send(new SM(SM.SM_mathcap));
      ox.send(new SM(SM.SM_popString));

      //ox.send(new CMO_STRING("print(\"Hello world!!\");\n"));
      //ox.send(new SM(SM.SM_executeStringByLocalParser));

      //ox.send(new CMO_STRING("def sub(B,A){return B-A;}"));
      //ox.send(new SM(SM.SM_executeStringByLocalParser));
      //ox.send(new SM(SM.SM_popString));

      //ox.send(new CMO_STRING("diff((x+2*y)^2,x);\0 1+2;"));
      //ox.send(new SM(SM.SM_executeStringByLocalParser));
      //ox.send(new SM(SM.SM_popString));

      //ox.send(new CMO_ZZ("1"));
      ox.send(new CMO_ZZ("-2"));
      //ox.send(new CMO_INT32(2));
      //ox.send(new CMO_STRING("sub"));
      //ox.send(new SM(SM.SM_executeFunction));
      ox.send(new SM(SM.SM_popCMO));

      { int[] array = {1,2};
      ox.send(new CMO_MONOMIAL32(array,new CMO_ZZ("-2")));
      }
      ox.send(new SM(SM.SM_popCMO));

      ox.send(new SM(SM.SM_popString));
      //ox.send(new SM(SM.SM_popString));

      ox.send(new SM(SM.SM_control_kill));

      //os.flush();

      //サーバ側から送信された文字列を受信します。
      while(true){
	ox.receive();
      }

    }catch(Exception e){
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
