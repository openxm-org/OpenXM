/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/OpenXMserver.java,v 1.3 2000/09/13 06:32:43 tam Exp $
 */
package ORG.openxm.tam;

import java.io.*;
import java.net.*;

/**
 * OpenXM クライアントからの接続を行なうソケットを作成する抽象クラス.
 * 新たな ox コマンドの実装により, 廃止になる予定.
 */
public abstract class OpenXMserver{
  private OpenXMserver server = null;
  private OpenXMstream control = null;
  private OpenXMstream stream = null;
  private Thread computeThread = null;
  final protected boolean debug = false;

  public OpenXMserver(){
    this("localhost",1200,1300);
  }

  public OpenXMserver(String host,int CtrlPort,int StreamPort){
    server = this;

    try{
      control = new OpenXMstream(host,CtrlPort,true);
      stream = new OpenXMstream(host,StreamPort,true);

      this.start();
    }catch(IOException e){}
  }

  final public void start(){ // control process of OpenXM
    try{
      control.sendByteOrder();

      debug("control: wait OX");
      computeThread = new Thread(){
	final public void run(){
	  try{
	    stream.sendByteOrder();
	  }catch(IOException e){}
	  server.computeProcess(stream);
	}
      };

      debug("control: compute process starting");
      computeThread.start();
      while(true){
	OXmessage message = control.receive();

	switch(message.getTag()){
	case OXmessage.OX_COMMAND:
	  switch(((SM)message.getBody()).getCode()){
	  case SM.SM_control_kill:
	    return;

	  case SM.SM_control_reset_connection:
	    //resetConnection();
	    break;
	  }
	  break;

	default:
	  break;
	}
      }
    }catch(IOException e){
      System.err.println(e.getMessage());
    }finally{
      debug("computer process interrupting...");
      computeThread.stop();
      debug("interrupted. OK! I'll shutdown.");
      try{stream.close();}catch(IOException e){}
      try{control.close();}catch(IOException e){}
    }
  }

  abstract protected void computeProcess(OpenXMstream DataStream);
  /*
  private void computeProcess(OpenXMstream DataStream){
    while(true){
      debug("test process executing");
      try{
	DataStream.receive();
      }catch(IOException e){}
    }
  }
  */

  /*
  final public synchronized void resetConnection(){
    debug("control: stopping computer process...");
    try{
      control.send(new CMO_INT32(0));
    }catch(IOException e){
    }catch(MathcapViolation e){}
    synchronized(stream){
      computeThread.stop();
    }
    debug("control: stopped.");

    try{
      debug("control: sending SYNC BALL.");
      stream.sendOX_SYNC_BALL();

      debug("control: waiting to receive SYNC BALL.");
      while(true){
	OXmessage message = stream.receive();

	debug("control: received "+ message);
	if(message.getTag() == OXmessage.OX_SYNC_BALL){
	  break;
	}
      }
      debug("control: received SYNC BALL and restart computer process");
      computeThread = new Thread(this);
      computeThread.start();
    }catch(IOException e){}catch(MathcapViolation e){}
  }
  */

  final private void debug(String str){
    if(debug){
      System.err.println(str);
    }
  }

  /*
  public static void main(String[] argv){
    String hostname = "localhost";
    int ControlPort = 1200, DataPort = 1300;
    OpenXMserver ox;

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

    try{
      ox = new OpenXMserver(hostname,ControlPort,DataPort);
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

    ox.start();

    System.err.println("breaking...");
  }
  */
}
