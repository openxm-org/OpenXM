/**
 * $OpenXM: OpenXM/src/OpenMath/ORG/openxm/tam/OpenXM.java,v 1.10 2002/10/27 10:39:32 takayama Exp $
 */
package ORG.openxm.tam;

import java.io.*;
import java.net.*;


/*&ja
    OpenXM サーバとの接続を行なうクラス. 
    クライアント側が使用する.     
    接続するサーバ毎に一つの OpenXM クラスが必要. 
*/
/**
 * OpenXM is a class to connect to OpenXM servers,
 * which are compliant to OpenXM RFC 100.
 * There is one-to-one correspondence between the instances
 * of the class OpenXM and the OpenXM servers.
 */
public class OpenXM{
  private OpenXMstream control = null, stream = null;
  final protected boolean debug = false;

  /*&ja
   * OpenXM サーバとの接続を TCP/IP ソケットを用いて行なう.
   * マシン名 host のポート番号 CtrlPort にコントロールを,
   * ポート番号 StreamPort にデータ用の接続を行なう.
   */
  /**
   * Connect to an OpenXM server via TCP/IP socket.
   * @param host  a machine name of the OpenXM server.
   * @param CtrlPort  the control port number of the OpenXM server.
   * @param StreamPort  the data port number of the OpenXM server.
   * As to details on the notion of control port and data port, see 
   *    Design and Implementation of OpenXM client server model and
   *        common mathematical object format (OpenXM-RFC 100,
   *                                            proposed standard) 
   *   @see <a href="http://www.openxm.org">OpenXM</a>
   */
  public OpenXM(String host,int CtrlPort,int StreamPort) throws IOException{
    control = new OpenXMstream(host,CtrlPort);

    try{
      Thread.sleep(100); // We need a few wait for starting up server.
    }catch(InterruptedException e){
      System.err.println(e.getMessage());
    }

    stream = new OpenXMstream(host,StreamPort);

    control.sendByteOrder();
    stream.sendByteOrder();
  }

  /*&ja
   * コマンド command を立ち上げ、
   * OpenXM サーバとの接続を TCP/IP ソケットを用いて行なう.
   * マシン名 host のポート番号 CtrlPort にコントロールを,
   * ポート番号 StreamPort にデータ用の接続を行なう.
   */
  /**
   * First, execute a command, which is usually an OpenXM server,
   * and next try to connect to the OpenXM server via TCP/IP.
   * @param command  a command.
   * @param host  a machine name of the OpenXM server.
   * @param CtrlPort  the control port number of the OpenXM server.
   * @param StreamPort  the data port number of the OpenXM server.
   */
  public OpenXM(String command,String host,int CtrlPort,int StreamPort)
       throws IOException{
    Runtime.getRuntime().exec(command);
    control = new OpenXMstream(host,CtrlPort);

    try{
      Thread.sleep(100); // We need a few wait for starting up server.
    }catch(InterruptedException e){
      System.err.println(e.getMessage());
    }

    stream = new OpenXMstream(host,StreamPort);

    control.sendByteOrder();
    stream.sendByteOrder();
  }

  public OpenXM(String host,int CtrlPort,int StreamPort,String ox_server,int oxd_port,String pass)
	   throws IOException{
		 // pass may be a null string.
    control = new OpenXMstream(host,CtrlPort,pass);
    stream = new OpenXMstream(host,StreamPort,pass);
    System.err.println("Listening...");
    System.err.println("Launch ox server with the reverse option, e.g., ox -ox ox_asir -reverse");
    // Launch ox_server by the oxd daemon (oxd_port).
    // BUG: It has not yet been implemented.
    control.OpenXMstreamAccept();
    System.err.println("Accepted the control port.");
    stream.OpenXMstreamAccept();
    System.err.println("Accepted the data port.");


    control.sendByteOrder();
    stream.sendByteOrder();
  }

public OpenXM(String ox_server) throws IOException {
	int oxdPort = 8089;
	String host = "localhost";
	oxdStream oxd = new oxdStream(oxdPort);
	int cport,dport;
	cport = oxd.startPhase1();
	dport = cport+1;
	try {
		control = new OpenXMstream(host,cport,"");
		stream = new OpenXMstream(host,dport,"");
	}catch( IOException e) {
		System.err.println("Could not open ports for client.");
		oxd.write("<bye/>");
	}

	System.err.println("Listenning...");

	oxd.startPhase2(ox_server,cport);
	
    control.OpenXMstreamAccept();
    System.err.println("Accepted the control port.");
    stream.OpenXMstreamAccept();
    System.err.println("Accepted the data port.");


    control.sendByteOrder();
    stream.sendByteOrder();
}


  /*&ja
   * サーバの計算中断を行なう. 現在は未実装.
   */
  /**
   * Resetting the engine process.  It has not yet been implemented.
   */
  public synchronized void resetConnection(){
    debug("control: stopping computer process...");
    debug("control: sending SYNC BALL.");
  }

  /*&ja
   * OpenXM メッセージをデータストリームに送信する.
   * このメソッドはメッセージのボディの部分だけでよい.
   * ヘッダ部分は自動で付加される.
   */
  /**
   * Send an OpenXM message object. 
   * @param object a message. For example, oxm.send(new CMO_STRING("Hello"))
   * sends a string "Hello" to the OpenXM server oxm in the CMO_STRING data
   * encoding.
   */
  public void send(OXbody object) throws IOException,MathcapViolation{
    stream.send(object);
  }

  /*&ja
   * データストリームから OpenXM メッセージを受け取る.
   */
  /**
   * Receive an OpenXM message.
   */
  public OXmessage receive() throws IOException{
    return stream.receive();
  }

  /*&ja
   * データストリームの MathCap を mathcap に設定する.
   * 以後, 送信するオブジェクトは mathcap に合っているかどうか
   * チェックが入る. 実際にチェックが入るかどうかは
   * OXbody クラスの派生クラスの実装による.
   * mathcap に反したオブジェクトを送ろうとした時には,
   * 以後 MathcapViolation が発生することが期待される.
   */
  /**
   * Set the mathcap.
   * If one tries to send an object which is prohibited to send by the mathcap,
   * the mathcapViolation exception is thrown.
   */
  public void setMathCap(CMO_MATHCAP mathcap){
    stream.setMathCap(mathcap);
  }

  private final void debug(String str){
    if(debug){
      System.err.println(str);
    }
  }

  public static void main(String[] argv){
    String hostname = "localhost";
    int ControlPort = 1200, DataPort = 1300;
    Runnable process = null;
    Thread thread;
    OpenXM ox;

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
      ox = new OpenXM(hostname,ControlPort,DataPort);

      thread = new Thread(process);
      thread.start();
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

    System.err.println("breaking...");

    try{
      // close connection
      ox.stream.close();
      ox.control.close();
    }catch(IOException e){
      e.printStackTrace();
    }
  }
}
