/**
 * OpenXM.java
 */
package JP.ac.kobe_u.math.tam.OpenXM;

import java.io.*;
import java.net.*;

public class OpenXM implements Runnable{
  private OpenXMconnection control = null, stream = null;
  private Thread thread = null;

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
	 thread = new Thread(process);
	 thread.start();
  }

  public void run(){
    try{
      control.sendByteOrder();

      while(true){
	switch(control.receiveOXtag()){
	case OX_COMMAND:
	  switch(control.receiveSM().getCode()){
	  case SM.SM_control_reset_connection:
	    this.resetConnection();
	    break;

	  default:
	  }

	case OX_DATA:
	  control.receiveCMO();
	  break;

	case OX_SYNC_BALL:
	default:
	  break;
	}
      }
    }catch(IOException e){
      thread.stop();
      System.err.println(e.getMessage());
    }
  }

  public synchronized void resetConnection(){
    synchronized(stream){
      thread.stop();
    }

    try{
      stream.sendSM(new SM(SM.SM_control_reset_connection));

      while(true){
	int ox_tag = stream.receiveOXtag();

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
    }

    return "";
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
